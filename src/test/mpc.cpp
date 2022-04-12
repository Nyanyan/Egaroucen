#include <iostream>
#include "./../midsearch.hpp"
#include "./../util.hpp"

using namespace std;

int main(){
    bit_init();
    flip_init();
    board_init();
    evaluate_init();
    parent_transpose_table.first_init();
    child_transpose_table.first_init();
    #if USE_MULTI_THREAD
        thread_pool.resize(16);
    #endif
    int depth;
    //bool searching = true;
    Search search;
    while (true){
        search.board = input_board();
        search.n_nodes = 0;
        cin >> depth;
        cin >> search.use_mpc;
        cin >> search.mpct;
        search.p = (search.board.p + depth) % 2;
        child_transpose_table.init();
        if (depth >= 20){
            parent_transpose_table.init();
            nega_scout(&search, -HW2, HW2, depth / 2, false, LEGAL_UNDEFINED, false);
        }
        parent_transpose_table.init();
        cout << nega_scout(&search, -HW2, HW2, depth, false, LEGAL_UNDEFINED, false) << endl;
        //cout << nega_alpha_ordering(&search, -HW2, HW2, depth, false, false, &searching) << endl;
        //cout << nega_alpha(&search, -HW2, HW2, depth, false) << endl;
        //cout << nega_alpha(&search, -HW2, HW2, depth, false) << endl;
    }

    return 0;
}