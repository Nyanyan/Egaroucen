#include <iostream>
#include "./../ai.hpp"

using namespace std;

int main(int argc, char *argv[]){
    bit_init();
    flip_init();
    board_init();
    evaluate_init();
    parent_transpose_table.first_init();
    child_transpose_table.first_init();
    int level = 5;
    if (argc >= 2)
        level = atoi(argv[1]);
    Board board;
    Search_result result;
    while (true){
        board = input_board();
        result = random_ai(board);
        //result = ai(board, level);
        cout << result.value << " " << idx_to_coord(result.policy) << endl;
    }

    return 0;
}