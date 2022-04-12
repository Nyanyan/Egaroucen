#pragma once
#include <iostream>
#include <random>
#include <string>

using namespace std;

#define HW 8
#define HW_M1 7
#define HW_P1 9
#define HW2 64
#define HW2_M1 63
#define HW2_P1 65

#define N_8BIT 256
#define N_DIAG_LINE 11
#define N_DIAG_LINE_M1 10

#define BLACK 0
#define WHITE 1
#define VACANT 2

#define N_PHASES 6
#define PHASE_N_STONES 10

#define INF 100000000

int xorx=123456789, xory=362436069, xorz=521288629, xorw=88675123;
inline int raw_myrandom(){
    int t = (xorx ^ (xorx << 11));
    xorx = xory;
    xory = xorz;
    xorz = xorw;
    xorw = (xorw ^ (xorw >> 19)) ^ (t ^ (t >> 8));
    return xorw;
}

inline double myrandom(){
    return (double)raw_myrandom() / 2147483648.0;
}

inline int32_t myrandrange(int32_t s, int32_t e){
    return s +(int)((e - s) * myrandom());
}

inline uint64_t myrand_uint(){
    return (uint64_t)raw_myrandom();
}

inline uint64_t myrand_ull(){
    return ((uint64_t)raw_myrandom() << 32) | (uint64_t)raw_myrandom();
}

bool global_searching = true;