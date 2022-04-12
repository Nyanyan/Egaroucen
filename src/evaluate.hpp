#pragma once
#include <iostream>
#include <fstream>
#include "setting.hpp"
#include "common.hpp"
#include "board.hpp"
#include "util.hpp"

using namespace std;

#define N_PATTERNS 14
#define N_CANPUT_PATTERNS 4
#define MAX_EVALUATE_IDX 59049

#define STEP 256
#define STEP_2 128

#define SCORE_MAX 64

#define P31 3
#define P32 9
#define P33 27
#define P34 81
#define P35 243
#define P36 729
#define P37 2187
#define P38 6561
#define P39 19683
#define P310 59049
#define P31m 2
#define P32m 8
#define P33m 26
#define P34m 80
#define P35m 242
#define P36m 728
#define P37m 2186
#define P38m 6560
#define P39m 19682
#define P310m 59048

#define P40 1
#define P41 4
#define P42 16
#define P43 64
#define P44 256
#define P45 1024
#define P46 4096
#define P47 16384
#define P48 65536

#define N_DENSE0 16
#define N_DENSE1 8

constexpr uint_fast16_t pow3[11] = {1, P31, P32, P33, P34, P35, P36, P37, P38, P39, P310};
uint64_t stability_edge_arr[N_8BIT][N_8BIT][2];
int16_t pattern_arr[N_PHASES][N_PATTERNS][MAX_EVALUATE_IDX];
int16_t canput_pattern_arr[N_PHASES][N_CANPUT_PATTERNS][P48];

string create_line(int b, int w){
    string res = "";
    for (int i = 0; i < HW; ++i){
        if ((b >> i) & 1)
            res += "X";
        else if ((w >> i) & 1)
            res += "O";
        else
            res += ".";
    }
    return res;
}

inline void probably_move_line(int p, int o, int place, int *np, int *no){
    int i, j;
    *np = p | (1 << place);
    for (i = place - 1; i >= 0 && (1 & (o >> i)); --i);
    if (1 & (p >> i)){
        for (j = place - 1; j > i; --j)
            *np ^= 1 << j;
    }
    for (i = place + 1; i < HW && (1 & (o >> i)); ++i);
    if (1 & (p >> i)){
        for (j = place + 1; j < i; ++j)
            *np ^= 1 << j;
    }
    *no = o & ~(*np);
}

int calc_stability_line(int b, int w, int ob, int ow){
    int i, nb, nw, res = 0b11111111;
    res &= b & ob;
    res &= w & ow;
    for (i = 0; i < HW; ++i){
        if ((1 & (b >> i)) == 0 && (1 & (w >> i)) == 0){
            probably_move_line(b, w, i, &nb, &nw);
            res &= calc_stability_line(nb, nw, ob, ow);
            probably_move_line(w, b, i, &nw, &nb);
            res &= calc_stability_line(nb, nw, ob, ow);
        }
    }
    return res;
}

inline void init_evaluation_base() {
    int place, b, w, stab;
    for (b = 0; b < N_8BIT; ++b) {
        for (w = b; w < N_8BIT; ++w){
            stab = calc_stability_line(b, w, b, w);
            stability_edge_arr[b][w][0] = 0;
            stability_edge_arr[b][w][1] = 0;
            for (place = 0; place < HW; ++place){
                if (1 & (stab >> place)){
                    stability_edge_arr[b][w][0] |= 1ULL << place;
                    stability_edge_arr[b][w][1] |= 1ULL << (place * HW);
                }
            }
            stability_edge_arr[w][b][0] = stability_edge_arr[b][w][0];
            stability_edge_arr[w][b][1] = stability_edge_arr[b][w][1];
        }
    }
}

inline double leaky_relu(double x){
    return max(0.01 * x, x);
}

inline double predict(int pattern_size, double in_arr[], double dense0[N_DENSE0][20], double bias0[N_DENSE0], double dense1[N_DENSE1][N_DENSE0], double bias1[N_DENSE1], double dense2[N_DENSE1], double bias2){
    double hidden0[16], hidden1;
    int i, j;
    for (i = 0; i < N_DENSE0; ++i){
        hidden0[i] = bias0[i];
        for (j = 0; j < pattern_size * 2; ++j)
            hidden0[i] += in_arr[j] * dense0[i][j];
        hidden0[i] = leaky_relu(hidden0[i]);
    }
    double res = bias2;
    for (i = 0; i < N_DENSE1; ++i){
        hidden1 = bias1[i];
        for (j = 0; j < N_DENSE0; ++j)
            hidden1 += hidden0[j] * dense1[i][j];
        hidden1 = leaky_relu(hidden1);
        res += hidden1 * dense2[i];
    }
    res = leaky_relu(res);
    return res;
}

inline void predict_all_pattern(int phase_idx, int evaluate_idx, int pattern_size, double dense0[N_DENSE0][20], double bias0[N_DENSE0], double dense1[N_DENSE1][N_DENSE0], double bias1[N_DENSE1], double dense2[N_DENSE1], double bias2){
    int digit, idx, i;
    double arr[20], tmp_pattern_arr[MAX_EVALUATE_IDX];
    for (idx = 0; idx < pow3[pattern_size]; ++idx){
        for (i = 0; i < pattern_size; ++i){
            digit = (idx / pow3[pattern_size - 1 - i]) % 3;
            if (digit == 0){
                arr[i] = 1.0;
                arr[pattern_size + i] = 0.0;
            } else if (digit == 1){
                arr[i] = 0.0;
                arr[pattern_size + i] = 1.0;
            } else{
                arr[i] = 0.0;
                arr[pattern_size + i] = 0.0;
            }
        }
        pattern_arr[phase_idx][evaluate_idx][idx] = predict(pattern_size, arr, dense0, bias0, dense1, bias1, dense2, bias2);
    }
}

inline void predict_all_canput_pattern(int phase_idx, int evaluate_idx, double dense0[N_DENSE0][20], double bias0[N_DENSE0], double dense1[N_DENSE1][N_DENSE0], double bias1[N_DENSE1], double dense2[N_DENSE1], double bias2){
    int digit, idx, i;
    double arr[16], tmp_pattern_arr[MAX_EVALUATE_IDX];
    for (idx = 0; idx < 65536; ++idx){
        for (i = 0; i < 16; ++i)
            arr[i] = (double)((idx >> i) & 1);
        canput_pattern_arr[phase_idx][evaluate_idx][idx] = predict(8, arr, dense0, bias0, dense1, bias1, dense2, bias2);
    }
}

inline bool init_evaluation_calc(){
    ifstream fp("evaluation/param/param.txt");
    if (fp.fail()){
        cerr << "evaluation file not exist" << endl;
        exit(1);
    }
    int phase_idx, pattern_idx, i, j;
    constexpr int pattern_sizes[N_PATTERNS] = {8, 8, 8, 5, 6, 7, 8, 10, 10, 10, 10, 9, 10, 10};
    string line;
    double dense0[N_DENSE0][20];
    double bias0[N_DENSE0];
    double dense1[N_DENSE1][N_DENSE0];
    double bias1[N_DENSE1];
    double dense2[N_DENSE1];
    double bias2;
    for (phase_idx = 0; phase_idx < N_PHASES; ++phase_idx){
        for (pattern_idx = 0; pattern_idx < N_PATTERNS; ++pattern_idx){
            for (i = 0; i < N_DENSE0; ++i){
                for (j = 0; j < pattern_sizes[pattern_idx] * 2; ++j){
                    getline(fp, line);
                    dense0[i][j] = stof(line);
                }
            }
            for (i = 0; i < N_DENSE0; ++i){
                getline(fp, line);
                bias0[i] = stof(line);
            }
            for (i = 0; i < N_DENSE1; ++i){
                for (j = 0; j < N_DENSE0; ++j){
                    getline(fp, line);
                    dense1[i][j] = stof(line);
                }
            }
            for (i = 0; i < N_DENSE1; ++i){
                getline(fp, line);
                bias1[i] = stof(line);
            }
            for (i = 0; i < N_DENSE1; ++i){
                getline(fp, line);
                dense2[i] = stof(line);
            }
            getline(fp, line);
            bias2 = stof(line);
            predict_all_pattern(phase_idx, pattern_idx, pattern_sizes[pattern_idx], dense0, bias0, dense1, bias1, dense2, bias2);
        }
        for (pattern_idx = 0; pattern_idx < N_CANPUT_PATTERNS; ++pattern_idx){
            for (i = 0; i < N_DENSE0; ++i){
                for (j = 0; j < 16; ++j){
                    getline(fp, line);
                    dense0[i][j] = stof(line);
                }
            }
            for (i = 0; i < N_DENSE0; ++i){
                getline(fp, line);
                bias0[i] = stof(line);
            }
            for (i = 0; i < N_DENSE1; ++i){
                for (j = 0; j < N_DENSE0; ++j){
                    getline(fp, line);
                    dense1[i][j] = stof(line);
                }
            }
            for (i = 0; i < N_DENSE1; ++i){
                getline(fp, line);
                bias1[i] = stof(line);
            }
            for (i = 0; i < N_DENSE1; ++i){
                getline(fp, line);
                dense2[i] = stof(line);
            }
            getline(fp, line);
            bias2 = stof(line);
            predict_all_canput_pattern(phase_idx, pattern_idx, dense0, bias0, dense1, bias1, dense2, bias2);
        }
    }
    cerr << "evaluation function initialized" << endl;
    return true;
}

bool evaluate_init(){
    init_evaluation_base();
    return init_evaluation_calc();
}

inline int pick_pattern(const int phase_idx, const int pattern_idx, const uint_fast8_t b_arr[], const int p0, const int p1, const int p2, const int p3, const int p4){
    return pattern_arr[phase_idx][pattern_idx][b_arr[p0] * P34 + b_arr[p1] * P33 + b_arr[p2] * P32 + b_arr[p3] * P31 + b_arr[p4]];
}

inline int pick_pattern(const int phase_idx, const int pattern_idx, const uint_fast8_t b_arr[], const int p0, const int p1, const int p2, const int p3, const int p4, const int p5){
    return pattern_arr[phase_idx][pattern_idx][b_arr[p0] * P35 + b_arr[p1] * P34 + b_arr[p2] * P33 + b_arr[p3] * P32 + b_arr[p4] * P31 + b_arr[p5]];
}

inline int pick_pattern(const int phase_idx, const int pattern_idx, const uint_fast8_t b_arr[], const int p0, const int p1, const int p2, const int p3, const int p4, const int p5, const int p6){
    return pattern_arr[phase_idx][pattern_idx][b_arr[p0] * P36 + b_arr[p1] * P35 + b_arr[p2] * P34 + b_arr[p3] * P33 + b_arr[p4] * P32 + b_arr[p5] * P31 + b_arr[p6]];
}

inline int pick_pattern(const int phase_idx, const int pattern_idx, const uint_fast8_t b_arr[], const int p0, const int p1, const int p2, const int p3, const int p4, const int p5, const int p6, const int p7){
    return pattern_arr[phase_idx][pattern_idx][b_arr[p0] * P37 + b_arr[p1] * P36 + b_arr[p2] * P35 + b_arr[p3] * P34 + b_arr[p4] * P33 + b_arr[p5] * P32 + b_arr[p6] * P31 + b_arr[p7]];
}

inline int pick_pattern(const int phase_idx, const int pattern_idx, const uint_fast8_t b_arr[], const int p0, const int p1, const int p2, const int p3, const int p4, const int p5, const int p6, const int p7, const int p8){
    return pattern_arr[phase_idx][pattern_idx][b_arr[p0] * P38 + b_arr[p1] * P37 + b_arr[p2] * P36 + b_arr[p3] * P35 + b_arr[p4] * P34 + b_arr[p5] * P33 + b_arr[p6] * P32 + b_arr[p7] * P31 + b_arr[p8]];
}

inline int pick_pattern(const int phase_idx, const int pattern_idx, const uint_fast8_t b_arr[], const int p0, const int p1, const int p2, const int p3, const int p4, const int p5, const int p6, const int p7, const int p8, const int p9){
    return pattern_arr[phase_idx][pattern_idx][b_arr[p0] * P39 + b_arr[p1] * P38 + b_arr[p2] * P37 + b_arr[p3] * P36 + b_arr[p4] * P35 + b_arr[p5] * P34 + b_arr[p6] * P33 + b_arr[p7] * P32 + b_arr[p8] * P31 + b_arr[p9]];
}

inline int calc_pattern(const int phase_idx, Board *b){
    uint_fast8_t b_arr[HW2];
    b->translate_to_arr_player(b_arr);
    return 
        pick_pattern(phase_idx, 0, b_arr, 8, 9, 10, 11, 12, 13, 14, 15) + pick_pattern(phase_idx, 0, b_arr, 1, 9, 17, 25, 33, 41, 49, 57) + pick_pattern(phase_idx, 0, b_arr, 48, 49, 50, 51, 52, 53, 54, 55) + pick_pattern(phase_idx, 0, b_arr, 6, 14, 22, 30, 38, 46, 54, 62) + 
        pick_pattern(phase_idx, 1, b_arr, 16, 17, 18, 19, 20, 21, 22, 23) + pick_pattern(phase_idx, 1, b_arr, 2, 10, 18, 26, 34, 42, 50, 58) + pick_pattern(phase_idx, 1, b_arr, 40, 41, 42, 43, 44, 45, 46, 47) + pick_pattern(phase_idx, 1, b_arr, 5, 13, 21, 29, 37, 45, 53, 61) + 
        pick_pattern(phase_idx, 2, b_arr, 24, 25, 26, 27, 28, 29, 30, 31) + pick_pattern(phase_idx, 2, b_arr, 3, 11, 19, 27, 35, 43, 51, 59) + pick_pattern(phase_idx, 2, b_arr, 32, 33, 34, 35, 36, 37, 38, 39) + pick_pattern(phase_idx, 2, b_arr, 4, 12, 20, 28, 36, 44, 52, 60) + 
        pick_pattern(phase_idx, 3, b_arr, 3, 12, 21, 30, 39) + pick_pattern(phase_idx, 3, b_arr, 4, 11, 18, 25, 32) + pick_pattern(phase_idx, 3, b_arr, 24, 33, 42, 51, 60) + pick_pattern(phase_idx, 3, b_arr, 59, 52, 45, 38, 31) + 
        pick_pattern(phase_idx, 4, b_arr, 2, 11, 20, 29, 38, 47) + pick_pattern(phase_idx, 4, b_arr, 5, 12, 19, 26, 33, 40) + pick_pattern(phase_idx, 4, b_arr, 16, 25, 34, 43, 52, 61) + pick_pattern(phase_idx, 4, b_arr, 58, 51, 44, 37, 30, 23) + 
        pick_pattern(phase_idx, 5, b_arr, 1, 10, 19, 28, 37, 46, 55) + pick_pattern(phase_idx, 5, b_arr, 6, 13, 20, 27, 34, 41, 48) + pick_pattern(phase_idx, 5, b_arr, 8, 17, 26, 35, 44, 53, 62) + pick_pattern(phase_idx, 5, b_arr, 57, 50, 43, 36, 29, 22, 15) + 
        pick_pattern(phase_idx, 6, b_arr, 0, 9, 18, 27, 36, 45, 54, 63) + pick_pattern(phase_idx, 6, b_arr, 7, 14, 21, 28, 35, 42, 49, 56) + 
        pick_pattern(phase_idx, 7, b_arr, 9, 0, 1, 2, 3, 4, 5, 6, 7, 14) + pick_pattern(phase_idx, 7, b_arr, 9, 0, 8, 16, 24, 32, 40, 48, 56, 49) + pick_pattern(phase_idx, 7, b_arr, 49, 56, 57, 58, 59, 60, 61, 62, 63, 54) + pick_pattern(phase_idx, 7, b_arr, 54, 63, 55, 47, 39, 31, 23, 15, 7, 14) + 
        pick_pattern(phase_idx, 8, b_arr, 0, 1, 2, 3, 8, 9, 10, 16, 17, 24) + pick_pattern(phase_idx, 8, b_arr, 7, 6, 5, 4, 15, 14, 13, 23, 22, 31) + pick_pattern(phase_idx, 8, b_arr, 63, 62, 61, 60, 55, 54, 53, 47, 46, 39) + pick_pattern(phase_idx, 8, b_arr, 56, 57, 58, 59, 48, 49, 50, 40, 41, 32) + 
        pick_pattern(phase_idx, 9, b_arr, 0, 2, 3, 4, 5, 7, 10, 11, 12, 13) + pick_pattern(phase_idx, 9, b_arr, 0, 16, 24, 32, 40, 56, 17, 25, 33, 41) + pick_pattern(phase_idx, 9, b_arr, 56, 58, 59, 60, 61, 63, 50, 51, 52, 53) + pick_pattern(phase_idx, 9, b_arr, 7, 23, 31, 39, 47, 63, 22, 30, 38, 46) + 
        pick_pattern(phase_idx, 10, b_arr, 0, 9, 18, 27, 1, 10, 19, 8, 17, 26) + pick_pattern(phase_idx, 10, b_arr, 7, 14, 21, 28, 6, 13, 20, 15, 22, 29) + pick_pattern(phase_idx, 10, b_arr, 56, 49, 42, 35, 57, 50, 43, 48, 41, 34) + pick_pattern(phase_idx, 10, b_arr, 63, 54, 45, 36, 62, 53, 44, 55, 46, 37) + 
        pick_pattern(phase_idx, 11, b_arr, 0, 1, 2, 8, 9, 10, 16, 17, 18) + pick_pattern(phase_idx, 11, b_arr, 7, 6, 5, 15, 14, 13, 23, 22, 21) + pick_pattern(phase_idx, 11, b_arr, 56, 57, 58, 48, 49, 50, 40, 41, 42) + pick_pattern(phase_idx, 11, b_arr, 63, 62, 61, 55, 54, 53, 47, 46, 45) + 
        pick_pattern(phase_idx, 12, b_arr, 10, 0, 1, 2, 3, 4, 5, 6, 7, 13) + pick_pattern(phase_idx, 12, b_arr, 17, 0, 8, 16, 24, 32, 40, 48, 56, 41) + pick_pattern(phase_idx, 12, b_arr, 50, 56, 57, 58, 59, 60, 61, 62, 63, 53) + pick_pattern(phase_idx, 12, b_arr, 46, 63, 55, 47, 39, 31, 23, 15, 7, 22) + 
        pick_pattern(phase_idx, 13, b_arr, 0, 1, 2, 3, 4, 8, 9, 16, 24, 32) + pick_pattern(phase_idx, 13, b_arr, 7, 6, 5, 4, 3, 15, 14, 23, 31, 39) + pick_pattern(phase_idx, 13, b_arr, 63, 62, 61, 60, 59, 55, 54, 47, 39, 31) + pick_pattern(phase_idx, 13, b_arr, 56, 57, 58, 59, 60, 48, 49, 40, 32, 24);
}

inline int create_canput_line_h(uint64_t b, uint64_t w, int t){
    return (((w >> (HW * t)) & 0b11111111) << HW) | ((b >> (HW * t)) & 0b11111111);
}

inline int create_canput_line_v(uint64_t b, uint64_t w, int t){
    return (join_v_line(w, t) << HW) | join_v_line(b, t);
}

inline int calc_canput_pattern(const int phase_idx, Board *b, const uint64_t player_mobility, const uint64_t opponent_mobility){
    return 
        canput_pattern_arr[phase_idx][0][create_canput_line_h(player_mobility, opponent_mobility, 0)] + 
        canput_pattern_arr[phase_idx][0][create_canput_line_h(player_mobility, opponent_mobility, 7)] + 
        canput_pattern_arr[phase_idx][0][create_canput_line_v(player_mobility, opponent_mobility, 0)] + 
        canput_pattern_arr[phase_idx][0][create_canput_line_v(player_mobility, opponent_mobility, 7)] + 
        canput_pattern_arr[phase_idx][1][create_canput_line_h(player_mobility, opponent_mobility, 1)] + 
        canput_pattern_arr[phase_idx][1][create_canput_line_h(player_mobility, opponent_mobility, 6)] + 
        canput_pattern_arr[phase_idx][1][create_canput_line_v(player_mobility, opponent_mobility, 1)] + 
        canput_pattern_arr[phase_idx][1][create_canput_line_v(player_mobility, opponent_mobility, 6)] + 
        canput_pattern_arr[phase_idx][2][create_canput_line_h(player_mobility, opponent_mobility, 2)] + 
        canput_pattern_arr[phase_idx][2][create_canput_line_h(player_mobility, opponent_mobility, 5)] + 
        canput_pattern_arr[phase_idx][2][create_canput_line_v(player_mobility, opponent_mobility, 2)] + 
        canput_pattern_arr[phase_idx][2][create_canput_line_v(player_mobility, opponent_mobility, 5)] + 
        canput_pattern_arr[phase_idx][3][create_canput_line_h(player_mobility, opponent_mobility, 3)] + 
        canput_pattern_arr[phase_idx][3][create_canput_line_h(player_mobility, opponent_mobility, 4)] + 
        canput_pattern_arr[phase_idx][3][create_canput_line_v(player_mobility, opponent_mobility, 3)] + 
        canput_pattern_arr[phase_idx][3][create_canput_line_v(player_mobility, opponent_mobility, 4)];
}

inline int end_evaluate(Board *b){
    int res = -b->score_player();
    return score_to_value(res);
}

inline int mid_evaluate(Board *b){
    int phase_idx, sur0, sur1, canput0, canput1, stab0, stab1, num0, num1;
    uint64_t player_mobility, opponent_mobility, empties;
    player_mobility = calc_legal(b->player, b->opponent);
    opponent_mobility = calc_legal(b->opponent, b->player);
    if (player_mobility == 0ULL && opponent_mobility == 0ULL)
        return end_evaluate(b);
    phase_idx = b->phase();
    int res = calc_pattern(phase_idx, b) + calc_canput_pattern(phase_idx, b, player_mobility, opponent_mobility);
    if (res > 0)
        res += STEP_2;
    else if (res < 0)
        res -= STEP_2;
    res /= STEP;
    return max(-SCORE_MAX, min(SCORE_MAX, res));
}
