#pragma once
#include <iostream>
#include <vector>
#include <functional>
#include "setting.hpp"
#include "common.hpp"
#include "board.hpp"
#include "evaluate.hpp"
#include "search.hpp"
#include "move_ordering.hpp"
#include "probcut.hpp"
#include "transpose_table.hpp"
#if USE_MULTI_THREAD
    #include "thread_pool.hpp"
    #include "ybwc.hpp"
#endif
#include "util.hpp"

using namespace std;

int nega_alpha_end_nomemo(Search *search, int alpha, int beta, int depth, bool skipped){
    if (!global_searching)
        return SCORE_UNDEFINED;
    if (depth <= MID_FAST_DEPTH)
        return nega_alpha(search, alpha, beta, depth, skipped);
    //if (depth == 1)
    //    return nega_alpha_eval1(search, alpha, beta, skipped);
    ++(search->n_nodes);
    #if USE_END_SC
        int stab_res = stability_cut(search, &alpha, &beta);
        if (stab_res != SCORE_UNDEFINED)
            return stab_res;
    #endif
    uint64_t legal = search->board.get_legal();
    int g, v = -INF;
    if (legal == 0ULL){
        if (skipped)
            return end_evaluate(&search->board);
        search->board.pass();
            v = -nega_alpha_end_nomemo(search, -beta, -alpha, depth, true);
        search->board.pass();
        return v;
    }
    #if USE_MID_MPC
        if (search->use_mpc){
            if (mpc(search, alpha, beta, depth, false, &v))
                return v;
        }
    #endif
    const int canput = pop_count_ull(legal);
    vector<Flip> move_list(canput);
    int idx = 0;
    for (uint_fast8_t cell = first_bit(&legal); legal; cell = next_bit(&legal))
        calc_flip(&move_list[idx++], &search->board, cell);
    move_ordering(search, move_list, depth, alpha, beta, false);
    for (const Flip &flip: move_list){
        search->board.move(&flip);
            g = -nega_alpha_end_nomemo(search, -beta, -alpha, depth - 1, false);
        search->board.undo(&flip);
        alpha = max(alpha, g);
        v = max(v, g);
        if (beta <= alpha)
            break;
    }
    return v;
}

inline int last1(Search *search, int alpha, int beta, int p0){
    ++search->n_nodes;
    int score = HW2 - 2 * search->board.count_opponent();
    int n_flip = count_last_flip(search->board.player, search->board.opponent, p0);
    if (n_flip == 0){
        ++search->n_nodes;
        n_flip = count_last_flip(search->board.opponent, search->board.player, p0);
        if (n_flip == 0){
            if (score < 1)
                score -= 2;
        } else
            score -= 2 * n_flip + 2;
    } else
        score += 2 * n_flip;
    return score_to_value(-score);
}

/*
inline int last1(Search *search, int alpha, int beta, int p0){
    ++search->n_nodes;
    int score = HW2 - 2 * search->board.count_opponent();
    int n_flip;
    n_flip = count_last_flip(search->board.player, search->board.opponent, p0);
    if (n_flip == 0){
        ++search->n_nodes;
        if (score <= 0){
            score -= 2;
            if (score >= alpha){
                n_flip = count_last_flip(search->board.opponent, search->board.player, p0);
                score -= 2 * n_flip;
            }
        } else{
            if (score >= alpha){
                n_flip = count_last_flip(search->board.opponent, search->board.player, p0);
                if (n_flip)
                    score -= 2 * n_flip + 2;
            }
        }
        
    } else
        score += 2 * n_flip;
    return score;
}
*/
inline int last2(Search *search, int alpha, int beta, int p0, int p1, bool skipped){
    ++search->n_nodes;
    int v = -INF, g;
    Flip flip;
    calc_flip(&flip, &search->board, p0);
    if (flip.flip){
        search->board.move(&flip);
            g = -last1(search, -beta, -alpha, p1);
        search->board.undo(&flip);
        alpha = max(alpha, g);
        if (beta <= alpha)
            return alpha;
        v = max(v, g);
    }
    calc_flip(&flip, &search->board, p1);
    if (flip.flip){
        search->board.move(&flip);
            g = -last1(search, -beta, -alpha, p0);
        search->board.undo(&flip);
        alpha = max(alpha, g);
        if (beta <= alpha)
            return alpha;
        v = max(v, g);
    }
    if (v == -INF){
        if (skipped)
            v = end_evaluate(&search->board);
        else{
            search->board.pass();
                v = -last2(search, -beta, -alpha, p0, p1, true);
            search->board.pass();
        }
    }
    return v;
}

inline int last3(Search *search, int alpha, int beta, int p0, int p1, int p2, bool skipped){
    ++search->n_nodes;
    uint64_t legal = search->board.get_legal();
    int v = -INF, g;
    if (legal == 0ULL){
        if (skipped)
            v = end_evaluate(&search->board);
        else{
            search->board.pass();
                v = -last3(search, -beta, -alpha, p0, p1, p2, true);
            search->board.pass();
        }
        return v;
    }
    Flip flip;
    if (1 & (legal >> p0)){
        calc_flip(&flip, &search->board, p0);
        search->board.move(&flip);
            g = -last2(search, -beta, -alpha, p1, p2, false);
        search->board.undo(&flip);
        alpha = max(alpha, g);
        if (beta <= alpha)
            return alpha;
        v = max(v, g);
    }
    if (1 & (legal >> p1)){
        calc_flip(&flip, &search->board, p1);
        search->board.move(&flip);
            g = -last2(search, -beta, -alpha, p0, p2, false);
        search->board.undo(&flip);
        alpha = max(alpha, g);
        if (beta <= alpha)
            return alpha;
        v = max(v, g);
    }
    if (1 & (legal >> p2)){
        calc_flip(&flip, &search->board, p2);
        search->board.move(&flip);
            g = -last2(search, -beta, -alpha, p0, p1, false);
        search->board.undo(&flip);
        alpha = max(alpha, g);
        if (beta <= alpha)
            return alpha;
        v = max(v, g);
    }
    return v;
}

inline int last4(Search *search, int alpha, int beta, int p0, int p1, int p2, int p3, bool skipped){
    ++search->n_nodes;
    uint64_t legal = search->board.get_legal();
    int v = -INF, g;
    if (legal == 0ULL){
        if (skipped)
            v = end_evaluate(&search->board);
        else{
            search->board.pass();
                v = -last4(search, -beta, -alpha, p0, p1, p2, p3, true);
            search->board.pass();
        }
        return v;
    }
    Flip flip;
    if (1 & (legal >> p0)){
        calc_flip(&flip, &search->board, p0);
        search->board.move(&flip);
            g = -last3(search, -beta, -alpha, p1, p2, p3, false);
        search->board.undo(&flip);
        alpha = max(alpha, g);
        if (beta <= alpha)
            return alpha;
        v = max(v, g);
    }
    if (1 & (legal >> p1)){
        calc_flip(&flip, &search->board, p1);
        search->board.move(&flip);
            g = -last3(search, -beta, -alpha, p0, p2, p3, false);
        search->board.undo(&flip);
        alpha = max(alpha, g);
        if (beta <= alpha)
            return alpha;
        v = max(v, g);
    }
    if (1 & (legal >> p2)){
        calc_flip(&flip, &search->board, p2);
        search->board.move(&flip);
            g = -last3(search, -beta, -alpha, p0, p1, p3, false);
        search->board.undo(&flip);
        alpha = max(alpha, g);
        if (beta <= alpha)
            return alpha;
        v = max(v, g);
    }
    if (1 & (legal >> p3)){
        calc_flip(&flip, &search->board, p3);
        search->board.move(&flip);
            g = -last3(search, -beta, -alpha, p0, p1, p2, false);
        search->board.undo(&flip);
        alpha = max(alpha, g);
        if (beta <= alpha)
            return alpha;
        v = max(v, g);
    }
    return v;
}

inline void pick_vacant(Search *search, uint_fast8_t cells[]){
    int idx = 0;
    uint64_t empties = ~(search->board.player | search->board.opponent);
    for (uint_fast8_t cell = first_bit(&empties); empties; cell = next_bit(&empties))
        cells[idx++] = cell;
}

int nega_alpha_end_fast(Search *search, int alpha, int beta, bool skipped){
    if (!global_searching)
        return SCORE_UNDEFINED;
    if (search->board.n == HW2)
        return end_evaluate(&search->board);
    /*
    if (search->board.n == 60){
        uint_fast8_t cells[4];
        pick_vacant(search, cells);
        return last4(search, alpha, beta, cells[0], cells[1], cells[2], cells[3], skipped);
    }
    */
    ++search->n_nodes;
    #if USE_END_SC
        int stab_res = stability_cut(search, &alpha, &beta);
        if (stab_res != SCORE_UNDEFINED)
            return stab_res;
    #endif
    uint64_t legal = search->board.get_legal();
    int g, v = -INF;
    if (legal == 0ULL){
        if (skipped)
            return end_evaluate(&search->board);
        search->board.pass();
            v = -nega_alpha_end_fast(search, -beta, -alpha, true);
        search->board.pass();
        return v;
    }
    Flip flip;
    for (uint_fast8_t cell = first_bit(&legal); legal; cell = next_bit(&legal)){
        calc_flip(&flip, &search->board, cell);
        search->board.move(&flip);
            g = -nega_alpha_end_fast(search, -beta, -alpha, false);
        search->board.undo(&flip);
        alpha = max(alpha, g);
        if (beta <= alpha)
            return alpha;
        v = max(v, g);
    }
    return v;
}

int nega_alpha_end(Search *search, int alpha, int beta, bool skipped, const bool *searching){
    if (!global_searching || !(*searching))
        return SCORE_UNDEFINED;
    if (search->board.n >= HW2 - END_FAST_DEPTH)
        return nega_alpha_end_fast(search, alpha, beta, skipped);
    ++search->n_nodes;
    #if USE_END_SC
        int stab_res = stability_cut(search, &alpha, &beta);
        if (stab_res != SCORE_UNDEFINED)
            return stab_res;
    #endif
    uint32_t hash_code = search->board.hash() & TRANSPOSE_TABLE_MASK;
    #if USE_END_TC
        int l, u;
        parent_transpose_table.get(&search->board, hash_code, &l, &u);
        if (u == l)
            return u;
        if (l >= beta)
            return l;
        if (alpha >= u)
            return u;
        alpha = max(alpha, l);
        beta = min(beta, u);
    #endif
    uint64_t legal = search->board.get_legal();
    int g, v = -INF;
    if (legal == 0ULL){
        if (skipped)
            return end_evaluate(&search->board);
        search->board.pass();
            v = -nega_alpha_end(search, -beta, -alpha, true, searching);
        search->board.pass();
        return v;
    }
    #if USE_END_MPC
        if (search->use_mpc){
            if (mpc(search, alpha, beta, HW2 - search->board.n, false, &v))
                return v;
        }
    #endif
    int best_move = child_transpose_table.get(&search->board, hash_code);
    int f_best_move = best_move;
    if (best_move != TRANSPOSE_TABLE_UNDEFINED){
        Flip flip;
        calc_flip(&flip, &search->board, best_move);
        search->board.move(&flip);
            g = -nega_alpha_end(search, -beta, -alpha, false, searching);
        search->board.undo(&flip);
        alpha = max(alpha, g);
        v = g;
        legal ^= 1ULL << best_move;
    }
    if (alpha < beta){
        const int canput = pop_count_ull(legal);
        vector<Flip> move_list(canput);
        int idx = 0;
        for (uint_fast8_t cell = first_bit(&legal); legal; cell = next_bit(&legal))
            calc_flip(&move_list[idx++], &search->board, cell);
        move_ordering(search, move_list, HW2 - search->board.n, alpha, beta, true);
        for (const Flip &flip: move_list){
            search->board.move(&flip);
                g = -nega_alpha_end(search, -beta, -alpha, false, searching);
            search->board.undo(&flip);
            alpha = max(alpha, g);
            if (v < g){
                v = g;
                best_move = flip.pos;
            }
            if (beta <= alpha)
                break;
        }
    }
    if (best_move != f_best_move)
        child_transpose_table.reg(&search->board, hash_code, best_move);
    #if USE_END_TC
        if (beta <= v && l < v)
            parent_transpose_table.reg(&search->board, hash_code, v, u);
        else if (v <= alpha && v < u)
            parent_transpose_table.reg(&search->board, hash_code, l, v);
        else if (alpha < v && v < beta)
            parent_transpose_table.reg(&search->board, hash_code, v, v);
    #endif
    return v;
}
