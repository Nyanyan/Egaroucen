#pragma once
#include <iostream>
#include "bit.hpp"
#include "setting.hpp"
/*
uint64_t calc_legal(uint64_t p, uint64_t o){
    uint64_t p1 = (p & 0x7F7F7F7F7F7F7F7FULL) << 1;
    uint64_t res = ~(p1 | o) & (p1 + (o & 0x7F7F7F7F7F7F7F7FULL));
    uint64_t rev_p = horizontal_mirror(p), rev_o = horizontal_mirror(o);
    p1 = (rev_p & 0x7F7F7F7F7F7F7F7FULL) << 1;
    uint64_t rev_res = ~(p1 | rev_o) & (p1 + (rev_o & 0x7F7F7F7F7F7F7F7FULL));

    uint64_t ph = black_line_mirror(p), oh = black_line_mirror(o);
    p1 = (ph & 0x7F7F7F7F7F7F7F7FULL) << 1;
    res |= black_line_mirror(~(p1 | oh) & (p1 + (oh & 0x7F7F7F7F7F7F7F7FULL)));
    ph = white_line_mirror(p), oh = white_line_mirror(o);
    p1 = (ph & 0x7F7F7F7F7F7F7F7FULL) << 1;
    res |= white_line_mirror(~(p1 | oh) & (p1 + (oh & 0x7F7F7F7F7F7F7F7FULL)));

    uint64_t pdiag = rotate_45(p), odiag = rotate_45(o);
    p1 = (pdiag & 0x5F6F777B7D7E7F3FULL) << 1;
    res |= unrotate_45(~(p1 | odiag) & (p1 + (odiag & 0x5F6F777B7D7E7F3FULL)));
    pdiag = rotate_45(rev_p);
    odiag = rotate_45(rev_o);
    p1 = (pdiag & 0x7D7B776F5F3F7F7EULL) << 1;
    rev_res |= unrotate_45(~(p1 | odiag) & (p1 + (odiag & 0x7D7B776F5F3F7F7EULL)));

    pdiag = rotate_135(p);
    odiag = rotate_135(o);
    p1 = (pdiag & 0x7D7B776F5F3F7F7EULL) << 1;
    res |= unrotate_135(~(p1 | odiag) & (p1 + (odiag & 0x7D7B776F5F3F7F7EULL)));
    pdiag = rotate_135(rev_p);
    odiag = rotate_135(rev_o);
    p1 = (pdiag & 0x5F6F777B7D7E7F3FULL) << 1;
    rev_res |= unrotate_135(~(p1 | odiag) & (p1 + (odiag & 0x5F6F777B7D7E7F3FULL)));

    return ~(p | o) & (res | horizontal_mirror(rev_res));
}
*/

inline uint64_t calc_some_mobility(uint64_t p, uint64_t o){
    uint64_t p1 = (p & 0x7F7F7F7F7F7F7F7FULL) << 1;
    uint64_t res = ~(p1 | o) & (p1 + (o & 0x7F7F7F7F7F7F7F7FULL));
    p = horizontal_mirror(p);
    o = horizontal_mirror(o);
    p1 = (p & 0x7F7F7F7F7F7F7F7FULL) << 1;
    return res | horizontal_mirror(~(p1 | o) & (p1 + (o & 0x7F7F7F7F7F7F7F7FULL)));
}

inline uint64_t calc_some_mobility_diag9(uint64_t p, uint64_t o){
    uint64_t p1 = (p & 0x5F6F777B7D7E7F3FULL) << 1;
    uint64_t res = ~(p1 | o) & (p1 + (o & 0x5F6F777B7D7E7F3FULL));
    p = horizontal_mirror(p);
    o = horizontal_mirror(o);
    p1 = (p & 0x7D7B776F5F3F7F7EULL) << 1;
    return res | horizontal_mirror(~(p1 | o) & (p1 + (o & 0x7D7B776F5F3F7F7EULL)));
}

inline uint64_t calc_some_mobility_diag7(uint64_t p, uint64_t o){
    uint64_t p1 = (p & 0x7D7B776F5F3F7F7EULL) << 1;
    uint64_t res = ~(p1 | o) & (p1 + (o & 0x7D7B776F5F3F7F7EULL));
    p = horizontal_mirror(p);
    o = horizontal_mirror(o);
    p1 = (p & 0x5F6F777B7D7E7F3FULL) << 1;
    return res | horizontal_mirror(~(p1 | o) & (p1 + (o & 0x5F6F777B7D7E7F3FULL)));
}

inline uint64_t calc_legal(uint64_t p, uint64_t o){
    uint64_t res = 
        calc_some_mobility(p, o) | 
        black_line_mirror(calc_some_mobility(black_line_mirror(p), black_line_mirror(o))) | 
        unrotate_45(calc_some_mobility_diag9(rotate_45(p), rotate_45(o))) | 
        unrotate_135(calc_some_mobility_diag7(rotate_135(p), rotate_135(o)));
    return res & ~(p | o);
}