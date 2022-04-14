#pragma once
#include <iostream>
#include <fstream>
#include <algorithm>
#include "setting.hpp"
#include "common.hpp"
#include "board.hpp"
#include "evaluate.hpp"
#include "transpose_table.hpp"

using namespace std;

#define MID_FAST_DEPTH 1
#define END_FAST_DEPTH 7
#define MID_TO_END_DEPTH 13

#define SCORE_UNDEFINED -INF

struct Search_result{
    int policy;
    int value;
    int depth;
    int nps;
    uint64_t nodes;
};

struct Search{
    Board board;
    bool use_mpc;
    double mpct;
    uint64_t n_nodes;
};

inline int stability_cut(Search *search, int *alpha, int *beta){
    /*
    int stab_player, stab_opponent;
    calc_stability(&search->board, &stab_player, &stab_opponent);
    int n_alpha = 2 * stab_player - HW2;
    int n_beta = HW2 - 2 * stab_opponent;
    if (*beta <= n_alpha)
        return n_alpha;
    if (n_beta <= *alpha)
        return n_beta;
    if (n_beta <= n_alpha)
        return n_alpha;
    *alpha = max(*alpha, n_alpha);
    *beta = min(*beta, n_beta);
    */
    return SCORE_UNDEFINED;
}
