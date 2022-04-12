#pragma once
#include <iostream>
#include <vector>
#include "common.hpp"
#include "board.hpp"
#include "search.hpp"
#include "flip.hpp"
#include "transpose_table.hpp"
#include "midsearch.hpp"
#include "probcut.hpp"

#define N_MOVE_ORDERING_PATTERNS 10
#define MAX_MOVE_ORDERING_EVALUATE_IDX 65536
#define MOVE_ORDERING_PHASE_DIV 10
#define N_MOVE_ORDERING_PHASE 6

#define W_BEST_MOVE 900000000

#define W_WIPEOUT 1000000000

#define MOVE_ORDERING_VALUE_OFFSET 14

#define W_END_MOBILITY 64
#define W_END_PARITY 14

int nega_alpha_eval1(Search *search, int alpha, int beta, bool skipped);
int nega_alpha(Search *search, int alpha, int beta, int depth, bool skipped);
int nega_alpha_ordering_nomemo(Search *search, int alpha, int beta, int depth, bool skipped, uint64_t legal);
int nega_scout(Search *search, int alpha, int beta, int depth, bool skipped, uint64_t legal, bool is_end_search);

inline void move_sort_top(vector<Flip> &move_list, int best_idx){
    if (best_idx != 0)
        swap(move_list[best_idx], move_list[0]);
}

bool cmp_move_ordering(Flip &a, Flip &b){
    return a.value > b.value;
}

inline void move_evaluate(Search *search, Flip *flip, const int alpha, const int beta, const int depth){
    if (flip->flip == search->board.opponent)
        flip->value = W_WIPEOUT;
    else{
        search->board.move(flip);
            switch(depth){
                case 0:
                    flip->value += (HW2 - value_to_score_int(mid_evaluate(&search->board)));
                    break;
                case 1:
                    flip->value += (HW2 - value_to_score_int(nega_alpha_eval1(search, alpha, beta, false)));
                    break;
                default:
                    if (depth <= MID_FAST_DEPTH)
                        flip->value += (HW2 - value_to_score_int(nega_alpha(search, alpha, beta, depth, false)));
                    else{
                        bool use_mpc = search->use_mpc;
                        search->use_mpc = false;
                            flip->value += (HW2 - value_to_score_int(nega_alpha_ordering_nomemo(search, alpha, beta, depth, false, flip->n_legal)));
                        search->use_mpc = use_mpc;
                    }
                    break;
            }
        search->board.undo(flip);
    }
}

inline void move_ordering(Search *search, vector<Flip> &move_list, int depth, int alpha, int beta, bool is_end_search){
    if (move_list.size() < 2)
        return;
    int eval_alpha = -min(SCORE_MAX, beta + MOVE_ORDERING_VALUE_OFFSET);
    int eval_beta = -max(-SCORE_MAX, alpha - MOVE_ORDERING_VALUE_OFFSET);
    int eval_depth = depth / 8;
    for (Flip &flip: move_list)
        move_evaluate(search, &flip, eval_alpha, eval_beta, eval_depth);
    sort(move_list.begin(), move_list.end(), cmp_move_ordering);
}
