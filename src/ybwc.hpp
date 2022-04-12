#pragma once
#include <iostream>
#include "setting.hpp"
#include "common.hpp"
#include "transpose_table.hpp"
#include "search.hpp"
#include "midsearch.hpp"
#include "endsearch.hpp"
#include "thread_pool.hpp"

//#define YBWC_SPLIT_DIV 7
#define YBWC_MID_SPLIT_MIN_DEPTH 6
//#define YBWC_END_SPLIT_MIN_DEPTH 6
//#define YBWC_MAX_SPLIT_COUNT 3
//#define YBWC_PC_OFFSET 3

int nega_alpha_ordering(Search *search, int alpha, int beta, int depth, bool skipped, uint64_t legal, bool is_end_search, const bool *searching);
int nega_alpha_end(Search *search, int alpha, int beta, bool skipped, uint64_t legal, const bool *searching);
int nega_scout(Search *search, int alpha, int beta, int depth, bool skipped, uint64_t legal, bool is_end_search);

inline pair<int, uint64_t> ybwc_do_task(Search search, int alpha, int beta, int depth, uint64_t legal, bool is_end_search, const bool *searching, int policy){
    int g = -nega_alpha_ordering(&search, alpha, beta, depth, false, legal, is_end_search, searching);
    if (*searching)
        return make_pair(g, search.n_nodes);
    return make_pair(SCORE_UNDEFINED, search.n_nodes);
}

inline bool ybwc_split(Search *search, const Flip *flip, int alpha, int beta, const int depth, uint64_t legal, bool is_end_search, const bool *searching, int policy, const int pv_idx, const int canput, const int split_count, vector<future<pair<int, uint64_t>>> &parallel_tasks){
    if (pv_idx > 0 && 
        /* pv_idx > canput / YBWC_SPLIT_DIV && */ 
        /* pv_idx < canput - 1 && */ 
        depth >= YBWC_MID_SPLIT_MIN_DEPTH /*&&*/
        /* split_count < YBWC_MAX_SPLIT_COUNT */ ){
        if (thread_pool.n_idle()){
            Search copy_search;
            search->board.move(flip);
                /*
                if (pv_idx == 0){
                    int bound = alpha - ceil(0.5 * probcut_sigma(search->board.n));
                    if (bound > HW2){
                        search->board.undo(flip);
                        return false;
                    }
                    bool not_split = false;
                    switch(mpcd[depth]){
                        case 0:
                            not_split = mid_evaluate(&search->board) <= bound;
                            break;
                        case 1:
                            not_split = nega_alpha_eval1(search, bound, bound + 1, false) <= bound;
                            break;
                        default:
                            if (mpcd[depth] <= MID_FAST_DEPTH)
                                not_split = nega_alpha(search, bound, bound + 1, mpcd[depth], false) <= bound;
                            else{
                                //bool use_mpc = search->use_mpc;
                                //search->use_mpc = false;
                                    not_split = nega_alpha_ordering_nomemo(search, bound, bound + 1, mpcd[depth], false, legal) <= bound;
                                //search->use_mpc = use_mpc;
                            }
                            break;
                    }
                    if (not_split){
                        search->board.undo(flip);
                        return false;
                    }
                }
                */
                search->board.copy(&copy_search.board);
            search->board.undo(flip);
            copy_search.use_mpc = search->use_mpc;
            copy_search.mpct = search->mpct;
            copy_search.n_nodes = 0;
            //copy_search.p = search->p;
            parallel_tasks.emplace_back(thread_pool.push(bind(&ybwc_do_task, copy_search, alpha, beta, depth, legal, is_end_search, searching, policy)));
            return true;
        }
    }
    return false;
}

inline int ybwc_wait_all(Search *search, vector<future<pair<int, uint64_t>>> &parallel_tasks){
    int g = -INF;
    pair<int, uint64_t> got_task;
    for (future<pair<int, uint64_t>> &task: parallel_tasks){
        //if (task.valid()){
        //if (task.wait_for(chrono::seconds(0)) == future_status::ready){
        got_task = task.get();
        //if (got_task.first != SCORE_UNDEFINED)
        g = max(g, got_task.first);
        search->n_nodes += got_task.second;
        //}
        //}
    }
    return g;
}
