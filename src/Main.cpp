﻿#define PAL_STDCPP_COMPAT

#include <Siv3D.hpp> // OpenSiv3D v0.6.3
#include <vector>
#include <thread>
#include <future>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip> 
#include <time.h>
#include <queue>
#include <algorithm>
#include "ai.hpp"
#include "human_value.hpp"
#include "joseki.hpp"
#include "umigame.hpp"
#include "gui/gui_common.hpp"
#include "gui/graph.hpp"
#include "gui/graph_human_sense.hpp"
#include "gui/menu.hpp"
#include "gui/language.hpp"
#include "gui/button.hpp"

using namespace std;

#define hint_not_calculated_define 0

#define left_left 20
#define left_center 255
#define left_right 490
#define right_left 510
#define right_center 745
#define right_right 980
#define x_center 500
#define y_center 360

#define BUTTON_NOT_PUSHED 0
#define BUTTON_LONG_PRESS_THRESHOLD 500

constexpr Color font_color = Palette::White;;
constexpr int board_size = 424, board_coord_size = 20;
constexpr int board_sx = left_left + board_coord_size, board_sy = 60, board_cell_size = board_size / HW, board_cell_frame_width = 2, board_frame_width = 7;
constexpr int stone_size = 21, legal_size = 5;
constexpr int graph_sx = 40, graph_sy = 530, graph_width = 424, graph_height = 150, graph_resolution = 8, graph_font_size = 15;
constexpr int human_sense_graph_sx = 520, human_sense_graph_sy = 275, human_sense_graph_width = 405, human_sense_graph_height = 405, humnan_sense_graph_resolution = 10;
constexpr Color green = Color(36, 153, 114, 100);
constexpr int start_game_how_to_use_width = 120, start_game_how_to_use_height = 30;
constexpr int start_game_button_x = 600, start_game_button_y = 49, start_game_button_w = start_game_how_to_use_width, start_game_button_h = start_game_how_to_use_height, start_game_button_r = 5;
constexpr Color button_color = Palette::White, button_font_color = Palette::Black;
constexpr int popup_width = 500, popup_height = 300, popup_r = 20, popup_circle_r = 30;
constexpr Color popup_color = Palette::White, popup_font_color = Palette::Black, popup_frame_color = Palette::Black, textbox_active_color = Palette::Lightcyan;
constexpr int popup_output_width = 800, popup_output_height = 600;
constexpr int popup_import_width = 600, popup_import_height = 450;
constexpr int info_sx = 520, info_sy = 50;
constexpr double popup_fade_time = 500.0;

struct Cell_value {
	int value;
	int depth;
};

bool ai_init() {
	bit_init();
	flip_init();
	board_init();
	if (!evaluate_init())
		return false;
	if (!book_init())
		return false;
	if (!joseki_init())
		return false;
	return true;
}

bool lang_initialize(string file) {
	return language.init(file);
}

Menu create_menu(Texture checkbox,
	bool* dummy,
	bool* entry_mode, bool* professional_mode, bool* serious_game,
	bool* start_game_flag, bool* analyze_flag,
	bool* human_first, bool* human_second, bool* both_ai, bool* both_human,
	bool* use_hint_flag, bool* normal_hint, bool* human_hint, bool* umigame_hint,
	bool* hint_num1, bool* hint_num2, bool* hint_num4, bool* hint_num8, bool* hint_num16, bool* hint_numall,
	bool* use_value_flag,
	bool* use_book_flag, int* ai_level, int* hint_level, int* book_error, int* use_book_depth,
	bool* start_book_learn_flag, bool* stop_book_learn_flag, bool* modify_book, int* book_depth, int* book_learn_accept, bool* import_book_flag,
	bool* output_record_flag, bool* output_game_flag, bool* input_record_flag, bool* input_board_flag,
	bool* show_end_popup, bool* show_log,
	bool* thread1, bool* thread2, bool* thread4, bool* thread8, bool* thread16, bool* thread32, bool* thread64, bool* thread128,
	bool* stop_read_flag, bool* resume_read_flag, bool* vertical_convert, bool* black_line_convert, bool* white_line_convert, bool* forward_flag, bool *backward_flag,
	bool* usage_flag, bool* bug_report_flag,
	bool lang_acts[], vector<string> lang_name_vector) {
	Menu menu;
	menu_title title;
	menu_elem menu_e, side_menu, side_side_menu;
	Font menu_font(15);

	title.init(language.get("mode", "mode"));

	menu_e.init_radio(language.get("mode", "entry_mode"), entry_mode, *entry_mode);
	title.push(menu_e);
	menu_e.init_radio(language.get("mode", "professional_mode"), professional_mode, *professional_mode);
	title.push(menu_e);
	menu_e.init_radio(language.get("mode", "serious_game"), serious_game, *serious_game);
	title.push(menu_e);

	menu.push(title);

	title.init(language.get("play", "game"));

	menu_e.init_button(language.get("play", "new_game"), start_game_flag);
	title.push(menu_e);
	menu_e.init_button(language.get("play", "analyze"), analyze_flag);
	title.push(menu_e);

	menu.push(title);

	title.init(language.get("settings", "settings"));

	if (*entry_mode) {
		*ai_level = min(*ai_level, 25);
		*hint_level = min(*hint_level, 15);
		*use_book_depth = 60;
		*use_book_flag = true;
		menu_e.init_button(language.get("ai_settings", "ai_settings"), dummy);
		side_menu.init_bar(language.get("ai_settings", "ai_level"), ai_level, *ai_level, 0, 25);
		menu_e.push(side_menu);
		side_menu.init_bar(language.get("ai_settings", "hint_level"), hint_level, *hint_level, 0, 15);
		menu_e.push(side_menu);
		title.push(menu_e);
	}
	else if (*professional_mode) {
		menu_e.init_button(language.get("ai_settings", "ai_settings"), dummy);
		side_menu.init_check(language.get("ai_settings", "use_book"), use_book_flag, *use_book_flag);
		menu_e.push(side_menu);
		side_menu.init_bar(language.get("ai_settings", "ai_level"), ai_level, *ai_level, 0, 60);
		menu_e.push(side_menu);
		side_menu.init_bar(language.get("ai_settings", "hint_level"), hint_level, *hint_level, 0, 60);
		menu_e.push(side_menu);
		side_menu.init_bar(language.get("ai_settings", "book_error"), book_error, *book_error, 0, 64);
		menu_e.push(side_menu);
		side_menu.init_bar(language.get("ai_settings", "use_book_depth"), use_book_depth, *use_book_depth, 0, 60);
		menu_e.push(side_menu);
		title.push(menu_e);
	}
	else if (*serious_game) {
		*ai_level = min(*ai_level, 25);
		*use_book_depth = 60;
		*use_book_flag = true;
		menu_e.init_button(language.get("ai_settings", "ai_settings"), dummy);
		side_menu.init_bar(language.get("ai_settings", "ai_level"), ai_level, *ai_level, 0, 25);
		menu_e.push(side_menu);
		title.push(menu_e);
	}

	menu_e.init_button(language.get("settings", "play", "play"), dummy);
	side_menu.init_radio(language.get("settings", "play", "human_first"), human_first, *human_first);
	menu_e.push(side_menu);
	side_menu.init_radio(language.get("settings", "play", "human_second"), human_second, *human_second);
	menu_e.push(side_menu);
	side_menu.init_radio(language.get("settings", "play", "both_ai"), both_ai, *both_ai);
	menu_e.push(side_menu);
	side_menu.init_radio(language.get("settings", "play", "both_human"), both_human, *both_human);
	menu_e.push(side_menu);
	title.push(menu_e);

	if (!(*entry_mode)) {
		menu_e.init_button(language.get("settings", "thread", "thread"), dummy);
		side_menu.init_radio(U"1", thread1, *thread1);
		menu_e.push(side_menu);
		side_menu.init_radio(U"2", thread2, *thread2);
		menu_e.push(side_menu);
		side_menu.init_radio(U"4", thread4, *thread4);
		menu_e.push(side_menu);
		side_menu.init_radio(U"8", thread8, *thread8);
		menu_e.push(side_menu);
		side_menu.init_radio(U"16", thread16, *thread16);
		menu_e.push(side_menu);
		side_menu.init_radio(U"32", thread32, *thread32);
		menu_e.push(side_menu);
		side_menu.init_radio(U"64", thread64, *thread64);
		menu_e.push(side_menu);
		side_menu.init_radio(U"128", thread128, *thread128);
		menu_e.push(side_menu);
		title.push(menu_e);
	}

	menu.push(title);



	title.init(language.get("display", "display"));

	if (!(*serious_game)) {
		menu_e.init_check(language.get("display", "hint", "hint"), use_hint_flag, *use_hint_flag);
		if (*professional_mode) {
			side_menu.init_check(language.get("display", "hint", "stone_value"), normal_hint, *normal_hint);
			side_side_menu.init_radio(U"1" + language.get("display", "hint", "show_number"), hint_num1, *hint_num1);
			side_menu.push(side_side_menu);
			side_side_menu.init_radio(U"2" + language.get("display", "hint", "show_number"), hint_num2, *hint_num2);
			side_menu.push(side_side_menu);
			side_side_menu.init_radio(U"4" + language.get("display", "hint", "show_number"), hint_num4, *hint_num4);
			side_menu.push(side_side_menu);
			side_side_menu.init_radio(U"8" + language.get("display", "hint", "show_number"), hint_num8, *hint_num8);
			side_menu.push(side_side_menu);
			side_side_menu.init_radio(U"16" + language.get("display", "hint", "show_number"), hint_num16, *hint_num16);
			side_menu.push(side_side_menu);
			side_side_menu.init_radio(language.get("display", "hint", "show_all"), hint_numall, *hint_numall);
			side_menu.push(side_side_menu);
			menu_e.push(side_menu);

			side_menu.init_check(language.get("display", "hint", "human_value"), human_hint, *human_hint);
			menu_e.push(side_menu);
			side_menu.init_check(language.get("display", "hint", "umigame_value"), umigame_hint, *umigame_hint);
			menu_e.push(side_menu);
		}
		title.push(menu_e);
	}

	menu_e.init_check(language.get("display", "graph"), use_value_flag, *use_value_flag);
	title.push(menu_e);

	menu_e.init_check(language.get("display", "end_popup"), show_end_popup, *show_end_popup);
	title.push(menu_e);
	menu_e.init_check(language.get("display", "log"), show_log, *show_log);
	title.push(menu_e);

	menu.push(title);




	if (*professional_mode) {
		title.init(language.get("book", "book"));

		menu_e.init_button(language.get("book", "start_learn"), start_book_learn_flag);
		title.push(menu_e);
		menu_e.init_button(language.get("book", "stop_learn"), stop_book_learn_flag);
		title.push(menu_e);
		menu_e.init_button(language.get("book", "auto_modification"), modify_book);
		title.push(menu_e);
		menu_e.init_button(language.get("book", "settings"), dummy);
		side_menu.init_bar(language.get("book", "depth"), book_depth, *book_depth, 0, 60);
		menu_e.push(side_menu);
		side_menu.init_bar(language.get("book", "accept"), book_learn_accept, *book_learn_accept, 0, 64);
		menu_e.push(side_menu);
		title.push(menu_e);
		menu_e.init_button(language.get("book", "import"), import_book_flag);
		title.push(menu_e);
		menu.push(title);



		title.init(language.get("in_out", "in_out"));

		menu_e.init_button(language.get("in_out", "output_record"), output_record_flag);
		title.push(menu_e);
		menu_e.init_button(language.get("in_out", "output_game"), output_game_flag);
		title.push(menu_e);
		menu_e.init_button(language.get("in_out", "input_record"), input_record_flag);
		title.push(menu_e);
		menu_e.init_button(language.get("in_out", "input_board"), input_board_flag);
		title.push(menu_e);

		menu.push(title);
	}



	title.init(language.get("operation", "operation"));

	menu_e.init_button(language.get("operation", "stop_read"), stop_read_flag);
	title.push(menu_e);
	menu_e.init_button(language.get("operation", "resume_read"), resume_read_flag);
	title.push(menu_e);

	menu_e.init_button(language.get("operation", "forward"), forward_flag);
	title.push(menu_e);
	menu_e.init_button(language.get("operation", "backward"), backward_flag);
	title.push(menu_e);

	if (*professional_mode) {
		menu_e.init_button(language.get("operation", "convert", "convert"), dummy);
		side_menu.init_button(language.get("operation", "convert", "vertical"), vertical_convert);
		menu_e.push(side_menu);
		side_menu.init_button(language.get("operation", "convert", "black_line"), black_line_convert);
		menu_e.push(side_menu);
		side_menu.init_button(language.get("operation", "convert", "white_line"), white_line_convert);
		menu_e.push(side_menu);
		title.push(menu_e);
	}

	menu.push(title);



	title.init(language.get("help", "help"));
	menu_e.init_button(language.get("help", "how_to_use"), usage_flag);
	title.push(menu_e);
	menu_e.init_button(language.get("help", "bug_report"), bug_report_flag);
	title.push(menu_e);
	menu.push(title);


	title.init(U"Language");
	for (int i = 0; i < (int)lang_name_vector.size(); ++i) {
		menu_e.init_radio(language_name.get(lang_name_vector[i]), &lang_acts[i], lang_acts[i]);
		title.push(menu_e);
	}
	menu.push(title);

	menu.init(0, 0, menu_font, checkbox);
	return menu;
}

pair<int, Board> move_board(Board b, bool board_clicked[]) {
	Flip flip;
	for (int cell = 0; cell < HW2; ++cell) {
		if (board_clicked[cell]) {
			calc_flip(&flip, &b, cell);
			b.move(&flip);
			b.check_player();
			return make_pair(cell, b);
		}
	}
	return make_pair(-1, b);
}

Cell_value analyze_search(Board b, int level, bool use_book, int use_book_depth) {
	Cell_value res;
	if (b.get_legal() == 0) {
		res.depth = 0;
		if (b.p == BLACK) {
			res.value = b.score_player();
		}
		else {
			res.value = b.score_opponent();
		}
	}
	else {
		int depth;
		bool use_mpc, is_mid_search;
		double mpct;
		get_level(level, b.n - 4, &is_mid_search, &depth, &use_mpc, &mpct);
		Search_result search_result = ai(b, level, use_book & b.n - 3 <= use_book_depth, 0);
		res.value = (b.p ? -1 : 1) * search_result.value;
		res.depth = search_result.depth;
	}
	return res;
}

int find_history_idx(vector<History_elem> history, int history_place) {
	for (int i = 0; i < (int)history.size(); ++i) {
		if (history[i].b.n - 4 == history_place) {
			return i;
		}
	}
	return 0;
}

void initialize_draw(future<bool>* f, bool* initializing, bool* initialize_failed, Font font, Font small_font, Texture icon, Texture logo, bool texture_loaded, String tips) {
	icon.scaled((double)(left_right - left_left) / icon.width()).draw(left_left, y_center - (left_right - left_left) / 2);
	logo.scaled((double)(left_right - left_left) * 0.8 / logo.width()).draw(right_left, y_center - 30);
	if (!(*initialize_failed) && texture_loaded) {
		font(language.get("loading", "loading")).draw(right_left, y_center + font.fontSize(), font_color);
		small_font(language.get("tips", "do_you_know")).draw(right_left, y_center + font.fontSize() + 70, font_color);
		small_font(tips).draw(right_left, y_center + font.fontSize() + 100, font_color);
		if (f->wait_for(chrono::seconds(0)) == future_status::ready) {
			if (f->get()) {
				*initializing = false;
			}
			else {
				*initialize_failed = true;
			}
		}
	}
	else {
		small_font(language.get("loading", "load_failed")).draw(right_left, y_center + font.fontSize(), font_color);
	}
}

void lang_initialize_failed_draw(Font font, Font small_font, Texture icon, Texture logo) {
	icon.scaled((double)(left_right - left_left) / icon.width()).draw(left_left, y_center - (left_right - left_left) / 2);
	logo.scaled((double)(left_right - left_left) * 0.8 / logo.width()).draw(right_left, y_center - 30);
	small_font(U"言語パックを読み込めませんでした\nresourcesフォルダを確認してください").draw(right_left, y_center + font.fontSize() * 3 / 2, font_color);
	small_font(U"Failed to load language pack\nPlease check the resources directory").draw(right_left, y_center + font.fontSize() * 3, font_color);
}

void closing_draw(Font font, Font small_font, Texture icon, Texture logo, bool texture_loaded) {
	icon.scaled((double)(left_right - left_left) / icon.width()).draw(left_left, y_center - (left_right - left_left) / 2);
	logo.scaled((double)(left_right - left_left) * 0.8 / logo.width()).draw(right_left, y_center - 30);
	font(language.get("closing")).draw(right_left, y_center + font.fontSize(), font_color);
}

void board_draw(Rect board_cells[], History_elem b, int next_policy, int int_mode, bool use_hint_flag, bool normal_hint, bool human_hint, bool umigame_hint,
	const int hint_state, const uint64_t hint_legal, const int hint_value[], const int hint_depth[], const bool hint_best_moves[], const int hint_show_num, Font normal_font, Font small_font, Font big_font, Font mini_font, Font coord_font,
	bool before_start_game,
	const int umigame_state[], const umigame_result umigame_value[],
	const int human_value_state, const Human_value human_value[],
	bool book_start_learn) {
	String coord_x = U"abcdefgh";
	for (int i = 0; i < HW; ++i) {
		coord_font(i + 1).draw(Arg::center(board_sx - board_coord_size, board_sy + board_cell_size * i + board_cell_size / 2), Color(51, 51, 51));
		coord_font(coord_x[i]).draw(Arg::center(board_sx + board_cell_size * i + board_cell_size / 2, board_sy - board_coord_size - 2), Color(51, 51, 51));
	}
	for (int i = 0; i < HW_M1; ++i) {
		Line(board_sx + board_cell_size * (i + 1), board_sy, board_sx + board_cell_size * (i + 1), board_sy + board_cell_size * HW).draw(board_cell_frame_width, Color(51, 51, 51));
		Line(board_sx, board_sy + board_cell_size * (i + 1), board_sx + board_cell_size * HW, board_sy + board_cell_size * (i + 1)).draw(board_cell_frame_width, Color(51, 51, 51));
	}
	Circle(board_sx + 2 * board_cell_size, board_sy + 2 * board_cell_size, 5).draw(Color(51, 51, 51));
	Circle(board_sx + 2 * board_cell_size, board_sy + 6 * board_cell_size, 5).draw(Color(51, 51, 51));
	Circle(board_sx + 6 * board_cell_size, board_sy + 2 * board_cell_size, 5).draw(Color(51, 51, 51));
	Circle(board_sx + 6 * board_cell_size, board_sy + 6 * board_cell_size, 5).draw(Color(51, 51, 51));
	RoundRect(board_sx, board_sy, board_cell_size * HW, board_cell_size * HW, 20).draw(ColorF(0, 0, 0, 0)).drawFrame(0, board_frame_width, Palette::White);
	int board_arr[HW2];
	Flip mob;
	uint64_t legal = b.b.get_legal();
	b.b.translate_to_arr(board_arr);
	for (int cell = 0; cell < HW2; ++cell) {
		int x = board_sx + (cell % HW) * board_cell_size + board_cell_size / 2;
		int y = board_sy + (cell / HW) * board_cell_size + board_cell_size / 2;
		if (board_arr[cell] == BLACK) {
			Circle(x, y, stone_size).draw(Palette::Black);
		}
		else if (board_arr[cell] == WHITE) {
			Circle(x, y, stone_size).draw(Palette::White);
		}
		if (1 & (legal >> cell)) {
			if (cell == next_policy) {
				int xx = board_sx + (HW_M1 - cell % HW) * board_cell_size + board_cell_size / 2;
				int yy = board_sy + (HW_M1 - cell / HW) * board_cell_size + board_cell_size / 2;
				if (b.b.p == WHITE) {
					Circle(xx, yy, stone_size).draw(ColorF(Palette::White, 0.2));
				}
				else {
					Circle(xx, yy, stone_size).draw(ColorF(Palette::Black, 0.2));
				}
			}
			if (int_mode != 2 && !before_start_game && !book_start_learn && (!use_hint_flag || (!normal_hint && !human_hint && !umigame_hint))) {
				int xx = board_sx + (HW_M1 - cell % HW) * board_cell_size + board_cell_size / 2;
				int yy = board_sy + (HW_M1 - cell / HW) * board_cell_size + board_cell_size / 2;
				Circle(xx, yy, legal_size).draw(Palette::Cyan);
			}
		}
	}
	if (b.policy != -1) {
		Circle(board_sx + (HW_M1 - b.policy % HW) * board_cell_size + board_cell_size / 2, board_sy + (HW_M1 - b.policy / HW) * board_cell_size + board_cell_size / 2, legal_size).draw(Palette::Red);
	}
	if (int_mode != 2 && use_hint_flag && legal != 0 && !before_start_game && !book_start_learn && (legal | hint_legal) == legal) {
		bool hint_shown[HW2];
		for (int i = 0; i < HW2; ++i) {
			hint_shown[i] = false;
		}
		if (normal_hint) {
			if (hint_state >= 2) {
				vector<pair<int, int>> show_cells;
				uint64_t all_legal = b.b.get_legal();
				for (int cell = 0; cell < HW2; ++cell) {
					if (1 & (all_legal >> cell)) {
						if ((1 & (hint_legal >> cell)) && (hint_best_moves[cell] || hint_depth[cell] == SEARCH_BOOK)) {
							show_cells.emplace_back(make_pair(-INF, cell));
						}
						else {
							show_cells.emplace_back(make_pair(-hint_value[cell], cell));
						}
					}
				}
				sort(show_cells.begin(), show_cells.end());
				int n_shown = 0, last_value = -INF;
				for (pair<int, int> elem : show_cells) {
					if (last_value > hint_value[elem.second] && n_shown >= hint_show_num) {
						int x = board_sx + (HW_M1 - elem.second % HW) * board_cell_size + board_cell_size / 2;
						int y = board_sy + (HW_M1 - elem.second / HW) * board_cell_size + board_cell_size / 2;
						Circle(x, y, legal_size).draw(Palette::Cyan);
					}
					else {
						Color color = Palette::White;
						if (hint_best_moves[elem.second]) {
							color = Palette::Cyan;
						}
						if (int_mode == 0) {
							int x = board_sx + (HW_M1 - elem.second % HW) * board_cell_size + board_cell_size / 2;
							int y = board_sy + (HW_M1 - elem.second / HW) * board_cell_size + board_cell_size / 2;
							big_font(hint_value[elem.second]).draw(Arg::center = Vec2{ x, y }, color);
						}
						else if (int_mode == 1) {
							int x = board_sx + (HW_M1 - elem.second % HW) * board_cell_size + 3;
							int y = board_sy + (HW_M1 - elem.second / HW) * board_cell_size + 3;
							normal_font(hint_value[elem.second]).draw(x, y, color);
							y += 19;
							if (hint_depth[elem.second] == SEARCH_BOOK) {
								small_font(U"book").draw(x, y, color);
							}
							else if (hint_depth[elem.second] == SEARCH_FINAL) {
								small_font(U"100%").draw(x, y, color);
							}
							else {
								small_font(U"Lv.", hint_depth[elem.second]).draw(x, y, color);
							}
						}
						hint_shown[elem.second] = true;
						last_value = hint_value[elem.second];
						++n_shown;
					}
				}
			}
		}
		if (umigame_hint && int_mode == 1) {
			for (int cell = 0; cell < HW2; ++cell) {
				if (1 & (legal >> cell)) {
					if (umigame_state[cell] == 2) {
						int umigame_sx = board_sx + (HW_M1 - cell % HW) * board_cell_size + 3;
						int umigame_sy = board_sy + (HW_M1 - cell / HW) * board_cell_size + 35;
						RectF black_rect = mini_font(umigame_value[cell].b).region(umigame_sx, umigame_sy);
						mini_font(umigame_value[cell].b).draw(umigame_sx, umigame_sy, Palette::Black);
						umigame_sx += black_rect.size.x;
						mini_font(umigame_value[cell].w).draw(umigame_sx, umigame_sy, Palette::White);
					}
				}
			}
		}
		if (human_hint && int_mode == 1) {
			if (human_value_state == 2) {
				int max_human_value = -INF;
				for (int cell = 0; cell < HW2; ++cell) {
					if (1 & (legal >> cell)) {
						int x = board_sx + (HW_M1 - cell % HW + 1) * board_cell_size - 3;
						int y = board_sy + (HW_M1 - cell / HW) * board_cell_size + 2;
						mini_font((int)round(human_value[cell].stability_black)).draw(Arg::topRight(x, y), Palette::Black);
						mini_font((int)round(human_value[cell].stability_white)).draw(Arg::topRight(x, y + mini_font.fontSize() + 1), Palette::White);
					}
				}
			}
		}
	}
}

bool show_popup(Board b, bool use_ai_flag, bool human_first, bool human_second, bool both_ai, int ai_level, Font big_font, Font small_font, long long strt) {
	double transparency = min(1.0, (double)(tim() - strt) / popup_fade_time);
	RoundRect(x_center - popup_width / 2, y_center - popup_height / 2, popup_width, popup_height, popup_r).draw(ColorF(popup_color, transparency));
	int black_stones = pop_count_ull(b.player);
	int white_stones = pop_count_ull(b.opponent);
	if (b.p == WHITE) {
		swap(black_stones, white_stones);
	}
	String result_str;
	if (use_ai_flag && human_first) {
		if (black_stones > white_stones) {
			big_font(language.get("result", "you_win")).draw(Arg::bottomCenter(x_center, y_center - 60), ColorF(popup_font_color, transparency));
		}
		else if (black_stones < white_stones) {
			big_font(language.get("result", "AI_win")).draw(Arg::bottomCenter(x_center, y_center - 60), ColorF(popup_font_color, transparency));
		}
		else {
			big_font(language.get("result", "draw")).draw(Arg::bottomCenter(x_center, y_center - 60), ColorF(popup_font_color, transparency));
		}
	}
	else if (use_ai_flag && human_second) {
		if (black_stones < white_stones) {
			big_font(language.get("result", "you_win")).draw(Arg::bottomCenter(x_center, y_center - 60), ColorF(popup_font_color, transparency));
		}
		else if (black_stones > white_stones) {
			big_font(language.get("result", "AI_win")).draw(Arg::bottomCenter(x_center, y_center - 60), ColorF(popup_font_color, transparency));
		}
		else {
			big_font(language.get("result", "draw")).draw(Arg::bottomCenter(x_center, y_center - 60), ColorF(popup_font_color, transparency));
		}
	}
	else {
		if (black_stones > white_stones) {
			big_font(language.get("result", "black_win")).draw(Arg::bottomCenter(x_center, y_center - 60), ColorF(popup_font_color, transparency));
		}
		else if (black_stones < white_stones) {
			big_font(language.get("result", "white_win")).draw(Arg::bottomCenter(x_center, y_center - 60), ColorF(popup_font_color, transparency));
		}
		else {
			big_font(language.get("result", "draw")).draw(Arg::bottomCenter(x_center, y_center - 60), ColorF(popup_font_color, transparency));
		}
	}
	Circle(x_center - popup_width / 3, y_center, popup_circle_r).draw(ColorF(Palette::Black, transparency));
	small_font(black_stones).draw(Arg::rightCenter(x_center - popup_width / 3 + popup_circle_r * 2 + 20, y_center), ColorF(popup_font_color, transparency));
	Circle(x_center + popup_width / 3, y_center, popup_circle_r).draw(ColorF(Palette::White, transparency)).drawFrame(2, ColorF(Palette::Black, transparency));
	small_font(white_stones).draw(Arg::leftCenter(x_center + popup_width / 3 - popup_circle_r * 2 - 20, y_center), ColorF(popup_font_color, transparency));
	FrameButton close_button;
	close_button.init(x_center - 225, y_center + 60, 200, 50, 10, 2, language.get("button", "close"), small_font, button_color, button_font_color, button_font_color);
	close_button.draw(transparency);
	FrameButton tweet_button;
	tweet_button.init(x_center + 25, y_center + 60, 200, 50, 10, 2, language.get("button", "tweet"), small_font, button_color, button_font_color, button_font_color);
	tweet_button.draw(transparency);
	if (transparency == 1.0 && tweet_button.clicked()) {
		String tweet_result;
		if (use_ai_flag && human_first) {
			if (black_stones > white_stones) {
				tweet_result = language.get("result", "tweet_you_win_0") + language.get("common", "othello_ai") + U" Egaroucid " + language.get("common", "level") + U" " + Format(ai_level) + language.get("result", "tweet_you_win_1");
			}
			else if (black_stones < white_stones) {
				tweet_result = language.get("result", "tweet_AI_win_0") + language.get("common", "othello_ai") + U" Egaroucid " + language.get("common", "level") + U" " + Format(ai_level) + language.get("result", "tweet_AI_win_1");
			}
			else {
				tweet_result = language.get("result", "tweet_draw_0") + language.get("common", "othello_ai") + U" Egaroucid " + language.get("common", "level") + U" " + Format(ai_level) + language.get("result", "tweet_draw_1");
			}
		}
		else if (use_ai_flag && human_second) {
			if (black_stones < white_stones) {
				tweet_result = language.get("result", "tweet_you_win_0") + language.get("common", "othello_ai") + U" Egaroucid " + language.get("common", "level") + U" " + Format(ai_level) + language.get("result", "tweet_you_win_1");
			}
			else if (black_stones > white_stones) {
				tweet_result = language.get("result", "tweet_AI_win_0") + language.get("common", "othello_ai") + U" Egaroucid " + language.get("common", "level") + U" " + Format(ai_level) + language.get("result", "tweet_AI_win_1");
			}
			else {
				tweet_result = language.get("result", "tweet_draw_0") + language.get("common", "othello_ai") + U" Egaroucid " + language.get("common", "level") + U" " + Format(ai_level) + language.get("result", "tweet_draw_1");
			}
		}
		else {
			tweet_result = language.get("common", "othello_ai") + U" Egaroucid";
		}
		tweet_result += U" #egaroucid https://www.egaroucid-app.nyanyan.dev/";
		Twitter::OpenTweetWindow(tweet_result);
	}
	bool close_flag = close_button.clicked() || KeyEnter.pressed() || KeyEscape.pressed();
	return (transparency == 1.0) && close_flag;
}

int output_game_popup(Font big_font, Font mid_font, Font small_font, String* black_player, String* white_player, String* game_memo, bool active_cells[]) {
	constexpr int sx = x_center - popup_output_width / 2;
	constexpr int sy = y_center - popup_output_height / 2;
	constexpr int player_area_width = popup_output_width / 2 - 60;
	constexpr int memo_area_width = popup_output_width - 80;
	constexpr int textbox_height = 40;
	RoundRect(sx, sy, popup_output_width, popup_output_height, popup_r).draw(popup_color);

	big_font(language.get("save_game", "save_game")).draw(Arg::center(x_center, sy + 40), popup_font_color);
	mid_font(language.get("save_game", "player_name")).draw(Arg::center(x_center, sy + 90), popup_font_color);
	Circle(x_center - player_area_width - 25, sy + 120 + 20, 16).draw(Palette::Black);
	Circle(x_center + player_area_width + 25, sy + 120 + 20, 16).draw(Palette::White).drawFrame(2, Palette::Black);
	Rect black_area{ x_center - player_area_width, sy + 120, player_area_width, textbox_height };
	Rect white_area{ x_center, sy + 120, player_area_width, textbox_height };
	mid_font(language.get("save_game", "memo")).draw(Arg::center(x_center, sy + 190), popup_font_color);
	Rect memo_area{ x_center - memo_area_width / 2, sy + 220, memo_area_width, textbox_height * 6 };
	const String editingText = TextInput::GetEditingText();
	bool tab_inputted = false;
	if (active_cells[0]) {
		tab_inputted = (*black_player).narrow().find('\t') != string::npos;
	}
	else if (active_cells[1]) {
		tab_inputted = (*white_player).narrow().find('\t') != string::npos;
	}
	else if (active_cells[2]) {
		tab_inputted = (*game_memo).narrow().find('\t') != string::npos;
	}
	if (tab_inputted) {
		for (int i = 0; i < 3; ++i) {
			if (active_cells[i]) {
				active_cells[i] = false;
				active_cells[(i + 1) % 3] = true;
				break;
			}
		}
		*black_player = (*black_player).replaced(U"\t", U"");
		*white_player = (*white_player).replaced(U"\t", U"");
		*game_memo = (*game_memo).replaced(U"\t", U"");
	}
	if (black_area.leftClicked() || active_cells[0]) {
		black_area.draw(textbox_active_color).drawFrame(2, popup_frame_color);
		TextInput::UpdateText(*black_player);
		if (KeyControl.pressed() && KeyV.down()) {
			String clip_text;
			Clipboard::GetText(clip_text);
			*black_player += clip_text;
		}
		small_font(*black_player + U'|' + editingText).draw(black_area.stretched(-4), popup_font_color);
		active_cells[0] = true;
		active_cells[1] = false;
		active_cells[2] = false;
	}
	else {
		black_area.draw(popup_color).drawFrame(2, popup_frame_color);
		small_font(*black_player).draw(black_area.stretched(-4), popup_font_color);
	}
	if (white_area.leftClicked() || active_cells[1]) {
		white_area.draw(textbox_active_color).drawFrame(2, popup_frame_color);
		TextInput::UpdateText(*white_player);
		if (KeyControl.pressed() && KeyV.down()) {
			String clip_text;
			Clipboard::GetText(clip_text);
			*white_player += clip_text;
		}
		small_font(*white_player + U'|' + editingText).draw(white_area.stretched(-4), popup_font_color);
		active_cells[0] = false;
		active_cells[1] = true;
		active_cells[2] = false;
	}
	else {
		white_area.draw(popup_color).drawFrame(2, popup_frame_color);
		small_font(*white_player).draw(white_area.stretched(-4), popup_font_color);
	}
	if (memo_area.leftClicked() || active_cells[2]) {
		memo_area.draw(textbox_active_color).drawFrame(2, popup_frame_color);
		TextInput::UpdateText(*game_memo);
		small_font(*game_memo + U'|' + editingText).draw(memo_area.stretched(-4), popup_font_color);
		active_cells[0] = false;
		active_cells[1] = false;
		active_cells[2] = true;
		if (KeyControl.pressed() && KeyV.down()) {
			String clip_text;
			Clipboard::GetText(clip_text);
			*game_memo += clip_text;
		}
	}
	else {
		memo_area.draw(popup_color).drawFrame(2, popup_frame_color);
		small_font(*game_memo).draw(memo_area.stretched(-4), popup_font_color);
	}
	FrameButton close_button;
	close_button.init(x_center - 375, sy + 500, 350, 50, 10, 2, language.get("button", "not_save_game"), mid_font, button_color, button_font_color, button_font_color);
	close_button.draw();
	FrameButton save_button;
	save_button.init(x_center + 25, sy + 500, 350, 50, 10, 2, language.get("button", "save_game"), mid_font, button_color, button_font_color, button_font_color);
	save_button.draw();
	if (save_button.clicked()) {
		return 1;
	}
	else if (close_button.clicked() || KeyEscape.pressed()) {
		return 2;
	}
	return 0;
}

int import_record_popup(Font big_font, Font mid_font, Font small_font, String* record) {
	constexpr int sx = x_center - popup_import_width / 2;
	constexpr int sy = y_center - popup_import_height / 2;
	constexpr int textbox_height = 40;
	RoundRect(sx, sy, popup_import_width, popup_import_height, popup_r).draw(popup_color);
	big_font(language.get("in_out", "input_record")).draw(Arg::center(x_center, sy + 40), popup_font_color);
	Rect text_area{ sx + 25, sy + 100, popup_import_width - 50, 200};
	text_area.draw(textbox_active_color).drawFrame(2, popup_frame_color);
	TextInput::UpdateText(*record);
	bool return_pressed = false;
	if (record->size()) {
		if ((*record)[record->size() - 1] == '\n') {
			return_pressed = true;
		}
	}
	if (KeyControl.pressed() && KeyV.down()) {
		String clip_text;
		Clipboard::GetText(clip_text);
		*record += clip_text;
	}
	small_font(*record + U'|').draw(text_area.stretched(-4), popup_font_color);
	FrameButton close_button;
	close_button.init(x_center - 225, sy + 350, 200, 50, 10, 2, language.get("button", "close"), mid_font, button_color, button_font_color, button_font_color);
	close_button.draw();
	FrameButton import_button;
	import_button.init(x_center + 25, sy + 350, 200, 50, 10, 2, language.get("button", "import"), mid_font, button_color, button_font_color, button_font_color);
	import_button.draw();
	if (import_button.clicked() || return_pressed) {
		return 1;
	}
	else if (close_button.clicked() || KeyEscape.pressed()) {
		return 2;
	}
	return 0;
}

int import_board_popup(Font big_font, Font mid_font, Font small_font, String* text) {
	constexpr int sx = x_center - popup_import_width / 2;
	constexpr int sy = y_center - popup_import_height / 2;
	constexpr int textbox_height = 40;
	RoundRect(sx, sy, popup_import_width, popup_import_height, popup_r).draw(popup_color);
	big_font(language.get("in_out", "input_board")).draw(Arg::center(x_center, sy + 40), popup_font_color);
	Rect text_area{ sx + 25, sy + 100, popup_import_width - 50, 200 };
	text_area.draw(textbox_active_color).drawFrame(2, popup_frame_color);
	TextInput::UpdateText(*text);
	bool return_pressed = false;
	if (text->size()) {
		if ((*text)[text->size() - 1] == '\n') {
			return_pressed = true;
		}
	}
	if (KeyControl.pressed() && KeyV.down()) {
		String clip_text;
		Clipboard::GetText(clip_text);
		*text += clip_text;
	}
	small_font(*text + U'|').draw(text_area.stretched(-4), popup_font_color);
	FrameButton close_button;
	close_button.init(x_center - 225, sy + 350, 200, 50, 10, 2, language.get("button", "close"), mid_font, button_color, button_font_color, button_font_color);
	close_button.draw();
	FrameButton import_button;
	import_button.init(x_center + 25, sy + 350, 200, 50, 10, 2, language.get("button", "import"), mid_font, button_color, button_font_color, button_font_color);
	import_button.draw();
	if (import_button.clicked() || return_pressed) {
		return 1;
	}
	else if (close_button.clicked() || KeyEscape.pressed()) {
		return 2;
	}
	return 0;
}

void reset_hint(int* hint_state, future<bool>* hint_future) {
	global_searching = false;
	for (int i = 0; i < HW2; ++i) {
		if (*hint_state % 2 == 1) {
			hint_future->get();
		}
		*hint_state = hint_not_calculated_define;
	}
	global_searching = true;
}

void reset_umigame(int umigame_state[], future<umigame_result> umigame_future[]) {
	global_searching = false;
	for (int i = 0; i < HW2; ++i) {
		if (umigame_state[i] == 1) {
			umigame_future[i].get();
		}
		umigame_state[i] = hint_not_calculated_define;
	}
	global_searching = true;
}

void reset_human_value(int* human_value_state, future<void>* human_value_future) {
	global_searching = false;
	if (*human_value_state == 1) {
		human_value_future->get();
	}
	*human_value_state = 0;
	global_searching = true;
}

void reset_ai(bool* ai_thinking, future<Search_result>* ai_future) {
	if (*ai_thinking) {
		global_searching = false;
		ai_future->get();
		global_searching = true;
		*ai_thinking = false;
	}
}

void reset_analyze(bool* analyzing, int *analyze_state, future<Cell_value>* analyze_future, future<Human_value>* analyze_stab_future) {
	if (*analyzing) {
		global_searching = false;
		if (*analyze_state == 1) {
			analyze_future->get();
		}
		else if (*analyze_state == 2) {
			analyze_stab_future->get();
		}
		global_searching = true;
		*analyzing = false;
		*analyze_state = 0;
	}
}

bool not_finished(Board bd) {
	return calc_legal(bd.player, bd.opponent) != 0ULL || calc_legal(bd.opponent, bd.player) != 0ULL;
}

umigame_result get_umigame_p(Board b) {
	return umigame.get(&b);
}

future<umigame_result> get_umigame(Board b) {
	return async(launch::async, get_umigame_p, b);
}

future<void> get_human_value(Board b, int depth, Human_value res[], int search_depth) {
	return async(launch::async, calc_all_human_value, b, depth, res, search_depth);
}

int import_int(ifstream* ifs) {
	string line;
	if (!getline(*ifs, line)) {
		cerr << "setting NOT imported" << endl;
		return -INF;
	}
	try {
		return stoi(line);
	}
	catch (const invalid_argument& ex) {
		cerr << "setting NOT imported" << endl;
		return -INF;
	}
}

string import_str(ifstream* ifs) {
	string line;
	if (!getline(*ifs, line)) {
		cerr << "setting NOT imported" << endl;
		return "undefined";
	}
	return line;
}

bool import_setting(int* int_mode, bool* use_book, int* ai_level, int* ai_book_accept, int* hint_level, int* use_book_depth,
	int* use_ai_mode,
	bool* use_hint_flag, bool* normal_hint, bool* human_hint, bool* umigame_hint,
	bool* show_end_popup,
	int* n_thread_idx,
	int* hint_num,
	int* book_depth, int* book_learn_accept,
	bool* show_log,
	string* lang_name) {
	ifstream ifs("resources/settings.txt");
	if (ifs.fail()) {
		return false;
	}
	*int_mode = import_int(&ifs);
	if (*int_mode == -INF) {
		return false;
	}
	*use_book = import_int(&ifs);
	if (*use_book == -INF) {
		return false;
	}
	*ai_level = import_int(&ifs);
	if (*ai_level == -INF) {
		return false;
	}
	*ai_book_accept = import_int(&ifs);
	if (*ai_book_accept == -INF) {
		return false;
	}
	*hint_level = import_int(&ifs);
	if (*hint_level == -INF) {
		return false;
	}
	*use_book_depth = import_int(&ifs);
	if (*use_book_depth == -INF) {
		return false;
	}
	*use_ai_mode = import_int(&ifs);
	if (*use_ai_mode == -INF) {
		return false;
	}
	*use_hint_flag = import_int(&ifs);
	if (*use_hint_flag == -INF) {
		return false;
	}
	*normal_hint = import_int(&ifs);
	if (*normal_hint == -INF) {
		return false;
	}
	*human_hint = import_int(&ifs);
	if (*human_hint == -INF) {
		return false;
	}
	*umigame_hint = import_int(&ifs);
	if (*umigame_hint == -INF) {
		return false;
	}
	*show_end_popup = import_int(&ifs);
	if (*show_end_popup == -INF) {
		return false;
	}
	*n_thread_idx = import_int(&ifs);
	if (*n_thread_idx == -INF) {
		return false;
	}
	*hint_num = import_int(&ifs);
	if (*hint_num == -INF) {
		return false;
	}
	*book_depth = import_int(&ifs);
	if (*book_depth == -INF) {
		return false;
	}
	*book_learn_accept = import_int(&ifs);
	if (*book_learn_accept == -INF) {
		return false;
	}
	*show_log = import_int(&ifs);
	if (*show_log == -INF) {
		return false;
	}
	*lang_name = import_str(&ifs);
	while (lang_name->back() == '\n' || lang_name->back() == '\r') {
		lang_name->pop_back();
	}
	if (*lang_name == "undefined") {
		return false;
	}
	return true;
}

void export_setting(int int_mode, bool use_book, int ai_level, int ai_book_accept, int hint_level, int use_book_depth, 
	int use_ai_mode,
	bool use_hint_flag, bool normal_hint, bool human_hint, bool umigame_hint,
	bool show_end_popup,
	int n_thread_idx,
	int hint_num,
	int book_depth, int book_learn_accept,
	bool show_log,
	string lang_name) {
	ofstream ofs("resources/settings.txt");
	if (!ofs.fail()) {
		ofs << int_mode << endl;
		ofs << use_book << endl;
		ofs << ai_level << endl;
		ofs << ai_book_accept << endl;
		ofs << hint_level << endl;
		ofs << use_book_depth << endl;
		ofs << use_ai_mode << endl;
		ofs << use_hint_flag << endl;
		ofs << normal_hint << endl;
		ofs << human_hint << endl;
		ofs << umigame_hint << endl;
		ofs << show_end_popup << endl;
		ofs << n_thread_idx << endl;
		ofs << hint_num << endl;
		ofs << book_depth << endl;
		ofs << book_learn_accept << endl;
		ofs << show_log << endl;
		ofs << lang_name << endl;
	}
}

String str_record(int policy) {
	return Unicode::Widen(string({ (char)('a' + HW_M1 - policy % HW), (char)('1' + HW_M1 - policy / HW) }));
}

bool import_record(String record, vector<History_elem>* n_history) {
	Board h_bd;
	bool flag = true;
	String record_tmp = U"";
	record = record.replace(U"\r", U"").replace(U"\n", U"");
	if (record.size() % 2 != 0 && record.size() <= 120) {
		flag = false;
	}
	else {
		int y, x;
		uint64_t legal;
		Flip flip;
		h_bd.reset();
		History_elem hist_tmp = { h_bd, -INF, -1, U"" };
		n_history->emplace_back(hist_tmp);
		for (int i = 0; i < (int)record.size(); i += 2) {
			x = (int)record[i] - (int)'a';
			if (x < 0 || HW <= x) {
				x = (int)record[i] - (int)'A';
				if (x < 0 || HW <= x) {
					flag = false;
					break;
				}
			}
			y = (int)record[i + 1] - (int)'1';
			if (y < 0 || HW <= y) {
				flag = false;
				break;
			}
			y = HW_M1 - y;
			x = HW_M1 - x;
			legal = h_bd.get_legal();
			if (1 & (legal >> (y * HW + x))) {
				calc_flip(&flip, &h_bd, y * HW + x);
				h_bd.move(&flip);
				if (h_bd.check_player()) {
					if (i != record.size() - 2) {
						flag = false;
						break;
					}
				}
			}
			else {
				flag = false;
				break;
			}
			hist_tmp = { h_bd, -INF, y * HW + x, n_history->at(n_history->size() - 1).record + str_record(y * HW + x) };
			n_history->emplace_back(hist_tmp);
		}
	}
	return flag;
}

pair<bool, Board> import_board(String board_str) {
	board_str = board_str.replace(U"\r", U"").replace(U"\n", U"").replace(U" ", U"");
	bool flag = true;
	int player = -1;
	int bd_arr[HW2];
	Board bd;
	if (board_str.size() != HW2 + 1) {
		flag = false;
	}
	else {
		for (int i = 0; i < HW2; ++i) {
			if (board_str[i] == '0' || board_str[i] == 'B' || board_str[i] == 'b' || board_str[i] == 'X' || board_str[i] == 'x' || board_str[i] == '*')
				bd_arr[i] = BLACK;
			else if (board_str[i] == '1' || board_str[i] == 'W' || board_str[i] == 'w' || board_str[i] == 'O' || board_str[i] == 'o')
				bd_arr[i] = WHITE;
			else if (board_str[i] == '.' || board_str[i] == '-')
				bd_arr[i] = VACANT;
			else {
				flag = false;
				break;
			}
		}
		if (board_str[HW2] == '0' || board_str[HW2] == 'B' || board_str[HW2] == 'b' || board_str[HW2] == 'X' || board_str[HW2] == 'x' || board_str[HW2] == '*')
			player = 0;
		else if (board_str[HW2] == '1' || board_str[HW2] == 'W' || board_str[HW2] == 'w' || board_str[HW2] == 'O' || board_str[HW2] == 'o')
			player = 1;
		else
			flag = false;
	}
	if (flag) {
		bd.translate_from_arr(bd_arr, player);
	}
	return make_pair(flag, bd);
}

#ifdef _WIN64
int get_localtime(tm *a, time_t *b) {
	return localtime_s(a, b);
}
#else
int get_localtime(tm* a, time_t* b) {
	a = localtime(b);
	return 0;
}
#endif

bool output_game(History_elem hist, int ai_level, int use_ai_mode, String black_player, String white_player, String game_memo) {
	time_t now;
	tm newtime;
	time(&now);
	int err = get_localtime(&newtime, &now);
	ostringstream sout;
	string year = to_string(newtime.tm_year + 1900);
	sout << setfill('0') << setw(2) << newtime.tm_mon + 1;
	string month = sout.str();
	sout.str("");
	sout.clear(stringstream::goodbit);
	sout << setfill('0') << setw(2) << newtime.tm_mday;
	string day = sout.str();
	sout.str("");
	sout.clear(stringstream::goodbit);
	sout << setfill('0') << setw(2) << newtime.tm_hour;
	string hour = sout.str();
	sout.str("");
	sout.clear(stringstream::goodbit);
	sout << setfill('0') << setw(2) << newtime.tm_min;
	string minute = sout.str();
	sout.str("");
	sout.clear(stringstream::goodbit);
	sout << setfill('0') << setw(2) << newtime.tm_sec;
	string second = sout.str();
	string info = year + "_" + month + "_" + day + "_" + hour + "_" + minute + "_" + second;
	ofstream ofs("records/" + info + ".txt");
	if (ofs.fail()) {
		return false;
	}
	string result = "?";
	if (hist.b.get_legal() == 0) {
		if (hist.b.p == BLACK) {
			result = to_string(hist.b.score_player());
		}
		else {
			result = to_string(hist.b.score_opponent());
		}
	}
	ofs << hist.record.narrow() << endl;
	ofs << result << endl;
	ofs << info << endl;
	ofs << ai_level << endl;
	ofs << use_ai_mode << endl;
	ofs << black_player.narrow() << endl;
	ofs << white_player.narrow() << endl;
	ofs << game_memo.narrow() << endl;
	return true;
}

bool close_app(int* hint_state, future<bool>* hint_future,
	int umigame_state[], future<umigame_result> umigame_future[],
	int* human_value_state, future<void>* human_value_future,
	bool* ai_thinking, future<Search_result>* ai_future,
	bool* analyzing, int *analyze_state, future<Cell_value>* analyze_future, future<Human_value>* analyze_human_future,
	int int_mode, bool use_book, int ai_level, int ai_book_accept, int hint_level, int use_book_depth, 
	int use_ai_mode,
	bool use_hint_flag, bool normal_hint, bool human_hint, bool umigame_hint,
	bool show_end_popup,
	int n_thread_idx,
	int hint_num,
	int book_depth, int book_learn_accept,
	bool show_log,
	bool* book_learning, future<void>* book_learn_future, bool book_changed,
	string lang_name) {
	reset_hint(hint_state, hint_future);
	reset_umigame(umigame_state, umigame_future);
	reset_human_value(human_value_state, human_value_future);
	reset_ai(ai_thinking, ai_future);
	reset_analyze(analyzing, analyze_state, analyze_future, analyze_human_future);
	export_setting(int_mode, use_book, ai_level, ai_book_accept, hint_level, use_book_depth, 
		use_ai_mode,
		use_hint_flag, normal_hint, human_hint, umigame_hint,
		show_end_popup,
		n_thread_idx,
		hint_num,
		book_depth, book_learn_accept,
		show_log,
		lang_name);
	if (*book_learning) {
		*book_learning = false;
		global_searching = false;
		book_learn_future->get();
		global_searching = true;
	}
	if (book_changed) {
		book.save_bin();
	}
	return true;
}

void info_draw(Board bd, string joseki_name, int ai_level, int hint_level, Font mid_font, Font small_font) {
	if (bd.get_legal() != 0) {
		mid_font(Format(pop_count_ull(bd.player) + pop_count_ull(bd.opponent) - 3) + language.get("info", "moves")).draw(info_sx, info_sy);
	}
	if (bd.get_legal() == 0) {
		mid_font(language.get("info", "game_end")).draw(info_sx, info_sy + 40);
	}
	else if (bd.p == BLACK) {
		mid_font(language.get("info", "black")).draw(info_sx, info_sy + 40);
	}
	else if (bd.p == WHITE) {
		mid_font(language.get("info", "white")).draw(info_sx, info_sy + 40);
	}
	mid_font(language.get("info", "joseki_name") + U": " + Unicode::FromUTF8(joseki_name)).draw(info_sx, info_sy + 80);
	Circle(info_sx + 12, info_sy + 140, 12).draw(Palette::Black);
	Circle(info_sx + 12, info_sy + 180, 12).draw(Palette::White);
	if (bd.p == BLACK) {
		mid_font(pop_count_ull(bd.player)).draw(Arg::leftCenter(info_sx + 40, info_sy + 140));
		mid_font(pop_count_ull(bd.opponent)).draw(Arg::leftCenter(info_sx + 40, info_sy + 180));
	}
	else {
		mid_font(pop_count_ull(bd.opponent)).draw(Arg::leftCenter(info_sx + 40, info_sy + 140));
		mid_font(pop_count_ull(bd.player)).draw(Arg::leftCenter(info_sx + 40, info_sy + 180));
	}
	int mid_depth, end_depth;
	get_level_depth(ai_level, &mid_depth, &end_depth);
	small_font(language.get("info", "ai") + U": " + language.get("common", "level") + Format(ai_level)).draw(info_sx + 120, info_sy + 125);
	small_font(language.get("info", "lookahead_0") + Format(mid_depth) + language.get("info", "lookahead_1")).draw(info_sx + 120, info_sy + 150);
	small_font(language.get("info", "complete_0") + Format(end_depth) + language.get("info", "complete_1")).draw(info_sx + 120, info_sy + 175);
	get_level_depth(hint_level, &mid_depth, &end_depth);
	small_font(language.get("info", "hint") + U": " + language.get("common", "level") + Format(hint_level)).draw(info_sx + 280, info_sy + 125);
	small_font(language.get("info", "lookahead_0") + Format(mid_depth) + language.get("info", "lookahead_1")).draw(info_sx + 280, info_sy + 150);
	small_font(language.get("info", "complete_0") + Format(end_depth) + language.get("info", "complete_1")).draw(info_sx + 280, info_sy + 175);
}

bool operator< (const pair<int, Board> &a, const pair<int, Board> &b){
	return a.first < b.first;
};

void learn_book(Board bd, int level, int depth, int book_learn_accept, Board* bd_ptr, int* value_ptr, bool* book_learning) {
	cerr << "start learning book" << endl;
	priority_queue<pair<int, Board>> que;
	int value = book.get(&bd);
	if (value == -INF) {
		value = -ai_value(bd, level);
		if (value == INF) {
			*book_learning = false;
			return;
		}
	}
	bd.copy(bd_ptr);
	*value_ptr = (bd.p ? -1 : 1) * value;
	book.reg(bd, value);
	que.push(make_pair(-abs(value), bd));
	pair<int, Board> popped;
	vector<pair<int, Board>> children;
	int weight, i, j;
	uint64_t legal;
	Board b, nb;
	Flip mob;
	bool reg_value;
	while (!que.empty()) {
		popped = que.top();
		que.pop();
		b = popped.second;
		if (b.n - 4 < depth) {
			children.clear();
			legal = b.get_legal();
			for (i = 0; i < HW2; ++i) {
				if (1 & (legal >> i)) {
					if (!(*book_learning)) {
						return;
					}
					calc_flip(&mob, &b, i);
					b.move_copy(&mob, &nb);
					value = -ai_value(nb, level);
					if (abs(value) <= HW2) {
						nb.copy(bd_ptr);
						*value_ptr = value;
						book.reg(nb, value);
						if (-value <= book_learn_accept && global_searching && nb.n - 4 < depth) {
							children.emplace_back(make_pair(-value + popped.first, nb));
						}
					}
				}
			}
			sort(children.begin(), children.end());
			for (i = 0; i < (int)children.size(); ++i) {
				que.push(make_pair((children[i].first - children[children.size() - 1].first) * (children[i].first - children[children.size() - 1].first) - b.n, children[i].second));
			}
		}
	}
	*book_learning = false;
	cerr << "book learning finished" << endl;
}

void show_change_book(int change_book_cell, String changed_book_value_str, Font font) {
	font(language.get("book", "changed_value") + U"(" + str_record(change_book_cell) + U"): " + changed_book_value_str).draw(600, 90, font_color);
}

bool import_book_p(string file) {
	cerr << "book import" << endl;
	bool result = false;
	vector<string> lst;
	auto offset = string::size_type(0);
	while (1) {
		auto pos = file.find(".", offset);
		if (pos == string::npos) {
			lst.push_back(file.substr(offset));
			break;
		}
		lst.push_back(file.substr(offset, pos - offset));
		offset = pos + 1;
	}
	if (lst[lst.size() - 1] == "egbk") {
		cerr << "importing Egaroucid book" << endl;
		result = book.import_file_bin(file);
	}
	else if (lst[lst.size() - 1] == "dat") {
		cerr << "importing Edax book" << endl;
		result = book.import_edax_book(file);
	}
	else {
		cerr << "this is not a book" << endl;
	}
	return result;
}

int import_book(future<bool>* import_book_future, Font font, Texture icon, Texture logo) {
	icon.scaled((double)(left_right - left_left) / icon.width()).draw(left_left, y_center - (left_right - left_left) / 2);
	logo.scaled((double)(left_right - left_left) * 0.8 / logo.width()).draw(right_left, y_center - 30);
	font(language.get("book", "import_explanation")).draw(right_left, y_center + font.fontSize() * 2, font_color);
	Button break_button;
	break_button.init(x_center - 125, 600, 250, 50, 10, language.get("book", "back"), font, button_color, button_font_color);
	break_button.draw();
	if (break_button.clicked()) {
		return 0;
	}
	if (DragDrop::HasNewFilePaths()) {
		for (const auto& dropped : DragDrop::GetDroppedFilePaths()) {
			*import_book_future = async(launch::async, import_book_p, dropped.path.narrow());
			return 2;
		}
	}
	return 1;
}

int import_book_failed(future<bool>* import_book_future, Font font, Texture icon, Texture logo) {
	icon.scaled((double)(left_right - left_left) / icon.width()).draw(left_left, y_center - (left_right - left_left) / 2);
	logo.scaled((double)(left_right - left_left) * 0.8 / logo.width()).draw(right_left, y_center - 30);
	font(language.get("book", "import_failed")).draw(right_left, y_center + font.fontSize() * 2, font_color);
	Button break_button;
	break_button.init(x_center - 125, 600, 250, 50, 10, language.get("book", "back"), font, button_color, button_font_color);
	break_button.draw();
	if (break_button.clicked()) {
		return 0;
	}
	if (DragDrop::HasNewFilePaths()) {
		for (const auto& dropped : DragDrop::GetDroppedFilePaths()) {
			*import_book_future = async(launch::async, import_book_p, dropped.path.narrow());
			return 2;
		}
	}
	return 3;
}

void import_loading_book(Font font, Texture icon, Texture logo) {
	icon.scaled((double)(left_right - left_left) / icon.width()).draw(left_left, y_center - (left_right - left_left) / 2);
	logo.scaled((double)(left_right - left_left) * 0.8 / logo.width()).draw(right_left, y_center - 30);
	font(language.get("book", "loading")).draw(right_left, y_center + font.fontSize(), font_color);
}

void Main() {
	//#ifndef _WIN64
	//SIV3D_SET(EngineOption::Renderer::OpenGLES);
	//#endif
	Size window_size = Size(1000, 720);
	Window::Resize(window_size);
	Window::SetStyle(WindowStyle::Sizable);
	Scene::SetResizeMode(ResizeMode::Keep);
	Window::SetTitle(U"Egaroucid 5.6.0");
	System::SetTerminationTriggers(UserAction::NoAction);
	Scene::SetBackground(green);
	//Console.open();
	stringstream logger_stream;
	cerr.rdbuf(logger_stream.rdbuf());
	string logger;
	String logger_String;

	bool dummy;
	constexpr int mode_size = 3;
	bool show_mode[mode_size] = { true, false, false };
	int int_mode = 0;
	bool start_game_flag = false, analyze_flag = false;
	bool human_first = true, human_second = false, both_ai = false, both_human = false;
	bool use_hint_flag = true, normal_hint = true, human_hint = true, umigame_hint = true;
	bool use_value_flag = true;
	bool start_book_learn_flag = false, stop_book_learn_flag = false, book_modify = false;
	bool output_record_flag = false, output_game_flag = false, input_record_flag = false, input_board_flag = false;
	bool texture_loaded = true;
	bool n_threads[8] = { false, false, true, false, false, false, false, false };
	int n_threads_num[8] = { 1, 2, 4, 8, 16, 32, 64, 128 };
	int n_thread_idx = 2;
	bool stop_read_flag = false, resume_read_flag = false, vertical_convert = false, white_line_convert = false, black_line_convert = false, forward_flag = false, backward_flag = false;
	bool usage_flag = false, bug_report_flag = false;
	bool language_acts[100];
	language_acts[0] = true;
	for (int i = 1; i < 100; ++i) {
		language_acts[i] = false;
	}
	vector<string> language_names;
	ifstream ifs("resources/languages/languages.txt");
	string lang_line;
	while (getline(ifs, lang_line)) {
		while (lang_line.back() == '\n' || lang_line.back() == '\r') {
			lang_line.pop_back();
		}
		language_names.emplace_back(lang_line);
	}
	Texture icon(U"resources/img/icon.png", TextureDesc::Mipped);
	Texture logo(U"resources/img/logo.png", TextureDesc::Mipped);
	Texture checkbox(U"resources/img/checked.png", TextureDesc::Mipped);
	if (icon.isEmpty() || logo.isEmpty() || checkbox.isEmpty()) {
		texture_loaded = false;
	}
	Rect board_cells[HW2];
	for (int cell = 0; cell < HW2; ++cell) {
		board_cells[cell] = Rect(board_sx + (HW_M1 - cell % HW) * board_cell_size, board_sy + (HW_M1 - cell / HW) * board_cell_size, board_cell_size, board_cell_size);
	}
	Font graph_font(graph_font_size);
	Graph graph;
	graph.sx = graph_sx;
	graph.sy = graph_sy;
	graph.size_x = graph_width;
	graph.size_y = graph_height;
	graph.resolution = graph_resolution;
	graph.font = graph_font;
	graph.font_size = graph_font_size;
	Human_sense_graph human_sense_graph;
	human_sense_graph.sx = human_sense_graph_sx;
	human_sense_graph.sy = human_sense_graph_sy;
	human_sense_graph.size_x = human_sense_graph_width;
	human_sense_graph.size_y = human_sense_graph_height;
	human_sense_graph.resolution = humnan_sense_graph_resolution;
	human_sense_graph.font = graph_font;
	human_sense_graph.font_size = graph_font_size;
	Font board_coord_font(board_coord_size);
	Font font50(50);
	Font font40(40);
	Font font30(30);
	Font font20(20);
	Font font15(15);
	Font normal_hint_font(17, Typeface::Bold);
	Font mini_hint_font(13, Typeface::Heavy);
	Font small_hint_font(9, Typeface::Bold);

	Board bd;
	bool board_clicked[HW2];
	vector<History_elem> history, fork_history;
	int history_place = 0;
	bool fork_mode = false;
	int bd_value = 0;

	int hint_value[HW2], hint_depth[HW2];
	int hint_calc_value[HW2], hint_calc_depth[HW2];
	bool hint_best_moves[HW2];
	uint64_t hint_legal = 0;
	future<bool>  hint_future;
	int hint_state = 0;
	bool hint_nums[6] = { false, false, false, false, false, true };
	constexpr int hint_actual_nums[6] = { 1, 2, 4, 8, 16, HW2 };
	int hint_num = 5;

	int umigame_state[HW2];
	umigame_result umigame_value[HW2];
	future<umigame_result> umigame_future[HW2];
	for (int i = 0; i < HW2; ++i) {
		umigame_state[i] = 0;
	}

	int human_value_state = 0;
	vector<Human_value> human_value_hist, fork_human_value_hist;
	Human_value human_value[HW2];
	int human_value_depth = 5;
	int human_value_search_depth = 2;
	future<void> human_value_future;

	future<bool> initialize_future = async(launch::async, ai_init);
	bool initializing = true, initialize_failed = false;

	Menu menu;

	future<Search_result> ai_future;
	bool ai_thinking = false;
	int ai_value = 0;
	int ai_level = 21, ai_book_accept = 4, hint_level = 9, use_book_depth = 60;
	bool use_book = true;

	bool before_start_game = true;
	Button start_game_button;

	bool show_popup_flag = true;
	bool show_end_popup = true;
	bool show_end_popup_change = true;
	long long popup_start_time = 0;

	bool analyzing = false;
	int analyze_idx = 0;
	future<Cell_value> analyze_future;
	future<Human_value> analyze_human_future;
	int analyze_state = 0;

	bool closing = false;
	future<bool> closing_future;

	bool outputting_game = false;
	String black_player = U"", white_player = U"", game_memo = U"";
	bool output_active[3] = { false, false, false };

	future<void> book_learn_future;
	bool book_learning = false, book_start_learn = false;
	int book_learn_accept = 4, book_depth = 30;
	bool book_changed = false;

	future<void> modify_book_future;
	bool book_modifying = false;

	bool changing_book = false;
	int change_book_cell = -1;
	String changed_book_value_str;

	bool import_book_flag = false;
	int importing_book = 0;
	future<bool> import_book_future;

	bool show_log = true;

	bool main_window_active = true;

	string joseki_name = "";

	String tips;

	bool inputting_record = false;
	String imported_record;

	bool inputting_board = false;
	String imported_board;

	uint64_t left_pushed = BUTTON_NOT_PUSHED;
	uint64_t right_pushed = BUTTON_NOT_PUSHED;

	int use_ai_mode;
	string lang_name;
	if (!import_setting(&int_mode, &use_book, &ai_level, &ai_book_accept, &hint_level, &use_book_depth,
		&use_ai_mode,
		&use_hint_flag, &normal_hint, &human_hint, &umigame_hint,
		&show_end_popup,
		&n_thread_idx,
		&hint_num,
		&book_depth, &book_learn_accept,
		&show_log,
		&lang_name)) {
		cerr << "use default setting" << endl;
		int_mode = 0;
		use_book = true;
		ai_level = 15;
		ai_book_accept = 2;
		hint_level = 7;
		use_book_depth = 60;
		use_ai_mode = 0;
		use_hint_flag = true;
		normal_hint = true;
		human_hint = false;
		umigame_hint = false;
		show_end_popup = true;
		n_thread_idx = 2;
		hint_num = 5;
		book_depth = 30;
		book_learn_accept = 6;
		show_log = true;
		lang_name = language_names[0];
	}
	for (int i = 0; i < mode_size; ++i) {
		show_mode[i] = i == int_mode;
	}
	for (int i = 0; i < 8; ++i) {
		n_threads[i] = i == n_thread_idx;
	}
	for (int i = 0; i < (int)language_names.size(); ++i) {
		language_acts[i] = lang_name == language_names[i];
	}
	if (use_ai_mode == 0) {
		human_first = true;
		human_second = false;
		both_ai = false;
		both_human = false;
	}
	else if (use_ai_mode == 1) {
		human_first = false;
		human_second = true;
		both_ai = false;
		both_human = false;
	}
	else if (use_ai_mode == 2) {
		human_first = false;
		human_second = false;
		both_ai = true;
		both_human = false;
	}
	else if (use_ai_mode == 3) {
		human_first = false;
		human_second = false;
		both_ai = false;
		both_human = true;
	}
	for (int i = 0; i < 6; ++i) {
		hint_nums[i] = i == hint_num;
	}

	show_end_popup_change = show_end_popup;

	int lang_initialized = 0;
	string lang_file = "resources/languages/" + lang_name + ".json";
	future<bool> lang_initialize_future = async(launch::async, lang_initialize, lang_file);
	language_name.init();


	while (System::Update()) {
		/*** log ***/
		while (getline(logger_stream, logger))
			logger_String = Unicode::Widen(logger);
		logger_stream.clear();
		if (show_log)
			font15(logger_String).draw(Arg::bottomLeft(5, 720), font_color);
		/*** log ***/
		/*** terminate ***/
		if (System::GetUserActions() & UserAction::CloseButtonClicked) {
			closing = true;
			use_ai_mode = 0;
			if (human_first) {
				use_ai_mode = 0;
			}
			else if (human_second) {
				use_ai_mode = 1;
			}
			else if (both_ai) {
				use_ai_mode = 2;
			}
			else if (both_human) {
				use_ai_mode = 3;
			}
			closing_future = async(launch::async, close_app, &hint_state, &hint_future,
				umigame_state, umigame_future,
				&human_value_state, &human_value_future,
				&ai_thinking, &ai_future,
				&analyzing, &analyze_state, &analyze_future, &analyze_human_future,
				int_mode, use_book, ai_level, ai_book_accept, hint_level, use_book_depth, 
				use_ai_mode,
				use_hint_flag, normal_hint, human_hint, umigame_hint,
				show_end_popup,
				n_thread_idx,
				hint_num,
				book_depth, book_learn_accept,
				show_log,
				&book_learning, &book_learn_future, book_changed,
				lang_name);
		}
		if (closing) {
			closing_draw(font50, font20, icon, logo, texture_loaded);
			if (closing_future.wait_for(chrono::seconds(0)) == future_status::ready) {
				closing_future.get();
				System::Exit();
			}
			continue;
		}
		/*** terminate ***/

		/*** initialize ***/
		if (initializing) {
			if (lang_initialized == 0) {
				if (lang_initialize_future.wait_for(chrono::seconds(0)) == future_status::ready) {
					if (lang_initialize_future.get()) {
						lang_initialized = 1;
					}
					else {
						lang_initialized = 3;
					}
				}
			}
			else if (lang_initialized == 1) {
				menu = create_menu(checkbox, &dummy,
					&show_mode[0], &show_mode[1], &show_mode[2],
					&start_game_flag, &analyze_flag,
					&human_first, &human_second, &both_ai, &both_human,
					&use_hint_flag, &normal_hint, &human_hint, &umigame_hint,
					&hint_nums[0], &hint_nums[1], &hint_nums[2], &hint_nums[3], &hint_nums[4], &hint_nums[5],
					&use_value_flag,
					&use_book, &ai_level, &hint_level, &ai_book_accept, &use_book_depth,
					&start_book_learn_flag, &stop_book_learn_flag, &book_modify, &book_depth, &book_learn_accept, &import_book_flag,
					&output_record_flag, &output_game_flag, &input_record_flag, &input_board_flag,
					&show_end_popup_change, &show_log,
					&n_threads[0], &n_threads[1], &n_threads[2], &n_threads[3], &n_threads[4], &n_threads[5], &n_threads[6], &n_threads[7],
					&stop_read_flag, &resume_read_flag, &vertical_convert, &black_line_convert, &white_line_convert, &forward_flag, &backward_flag,
					&usage_flag, &bug_report_flag,
					language_acts, language_names);
				start_game_button.init(start_game_button_x, start_game_button_y, start_game_button_w, start_game_button_h, start_game_button_r, language.get("button", "start_game"), font15, button_color, button_font_color);
				tips = language.get_random("tips", "tips");
				lang_initialized = 2;
			}
			else if (lang_initialized == 2) {
				if (!show_mode[int_mode]) {
					for (int i = 0; i < mode_size; ++i) {
						if (show_mode[i]) {
							int_mode = i;
						}
					}
					menu = create_menu(checkbox, &dummy,
						&show_mode[0], &show_mode[1], &show_mode[2],
						&start_game_flag, &analyze_flag,
						&human_first, &human_second, &both_ai, &both_human,
						&use_hint_flag, &normal_hint, &human_hint, &umigame_hint,
						&hint_nums[0], &hint_nums[1], &hint_nums[2], &hint_nums[3], &hint_nums[4], &hint_nums[5],
						&use_value_flag,
						&use_book, &ai_level, &hint_level, &ai_book_accept, &use_book_depth,
						&start_book_learn_flag, &stop_book_learn_flag, &book_modify, &book_depth, &book_learn_accept, &import_book_flag,
						&output_record_flag, &output_game_flag, &input_record_flag, &input_board_flag,
						&show_end_popup_change, &show_log,
						&n_threads[0], &n_threads[1], &n_threads[2], &n_threads[3], &n_threads[4], &n_threads[5], &n_threads[6], &n_threads[7],
						&stop_read_flag, &resume_read_flag, &vertical_convert, &black_line_convert, &white_line_convert, &forward_flag, &backward_flag,
						&usage_flag, &bug_report_flag,
						language_acts, language_names);
				}
				initialize_draw(&initialize_future, &initializing, &initialize_failed, font50, font20, icon, logo, texture_loaded, tips);
				if (!initializing) {
					bd.reset();
					History_elem hist_tmp = { bd, -INF, -1, U"" };
					history.emplace_back(hist_tmp);
					history_place = 0;
					fork_mode = false;
					hint_state = hint_not_calculated_define;
					for (int i = 0; i < HW2; ++i) {
						umigame_state[i] = hint_not_calculated_define;
					}
					human_value_state = hint_not_calculated_define;
					thread_pool.resize(n_threads_num[n_thread_idx]);
					main_window_active = false;
				}
			}
			else {
				lang_initialize_failed_draw(font50, font20, icon, logo);
			}
		}
		/*** initialize ***/

		/*** book importing ***/
		else if (importing_book == 1) {
			importing_book = import_book(&import_book_future, font30, icon, logo);
		}
		else if (importing_book == 2) {
			import_loading_book(font50, icon, logo);
			if (import_book_future.wait_for(chrono::seconds(0)) == future_status::ready) {
				if (import_book_future.get()) {
					importing_book = 0;
					book_changed = true;
				}
				else {
					importing_book = 3;
				}
			}
		}
		else if (importing_book == 3) {
			importing_book = import_book_failed(&import_book_future, font30, icon, logo);
		}
		/*** book importing ***/

		/*** initialized ***/
		else {
			/**** when mode changed **/
			if (!show_mode[int_mode]) {
				for (int i = 0; i < mode_size; ++i) {
					if (show_mode[i]) {
						int_mode = i;
					}
				}
				if (int_mode != 1 && fork_mode) {
					fork_mode = false;
					fork_history.clear();
					history_place = min(history_place, (int)history.size() - 1);
					bd = history[find_history_idx(history, history_place)].b;
				}
				menu = create_menu(checkbox, &dummy,
					&show_mode[0], &show_mode[1], &show_mode[2],
					&start_game_flag, &analyze_flag,
					&human_first, &human_second, &both_ai, &both_human,
					&use_hint_flag, &normal_hint, &human_hint, &umigame_hint,
					&hint_nums[0], &hint_nums[1], &hint_nums[2], &hint_nums[3], &hint_nums[4], &hint_nums[5],
					&use_value_flag,
					&use_book, &ai_level, &hint_level, &ai_book_accept, &use_book_depth,
					&start_book_learn_flag, &stop_book_learn_flag, &book_modify, &book_depth, &book_learn_accept, &import_book_flag,
					&output_record_flag, &output_game_flag, &input_record_flag, &input_board_flag,
					&show_end_popup_change, &show_log,
					&n_threads[0], &n_threads[1], &n_threads[2], &n_threads[3], &n_threads[4], &n_threads[5], &n_threads[6], &n_threads[7],
					&stop_read_flag, &resume_read_flag, &vertical_convert, &black_line_convert, &white_line_convert, &forward_flag, &backward_flag,
					&usage_flag, &bug_report_flag,
					language_acts, language_names);
			}
			/**** when mode changed **/

			/*** thread ***/
			if (!n_threads[n_thread_idx]) {
				for (int i = 0; i < 8; ++i) {
					if (n_threads[i]) {
						n_thread_idx = i;
					}
				}
				thread_pool.resize(n_threads_num[n_thread_idx]);
				cerr << "threads resized to " << n_threads_num[n_thread_idx] << " completed " << thread_pool.size() << endl;
			}
			/*** thread ***/

			/*** language ***/
			for (int i = 0; i < (int)language_names.size(); ++i) {
				if (language_acts[i] && language_names[i] != lang_name) {
					lang_name = language_names[i];
					lang_file = "resources/languages/" + lang_name + ".json";
					lang_initialize(lang_file);
					cerr << "lang " << language.get("lang_name").narrow() << endl;
					menu = create_menu(checkbox, &dummy,
						&show_mode[0], &show_mode[1], &show_mode[2],
						&start_game_flag, &analyze_flag,
						&human_first, &human_second, &both_ai, &both_human,
						&use_hint_flag, &normal_hint, &human_hint, &umigame_hint,
						&hint_nums[0], &hint_nums[1], &hint_nums[2], &hint_nums[3], &hint_nums[4], &hint_nums[5],
						&use_value_flag,
						&use_book, &ai_level, &hint_level, &ai_book_accept, &use_book_depth,
						&start_book_learn_flag, &stop_book_learn_flag, &book_modify, &book_depth, &book_learn_accept, &import_book_flag,
						&output_record_flag, &output_game_flag, &input_record_flag, &input_board_flag,
						&show_end_popup_change, &show_log,
						&n_threads[0], &n_threads[1], &n_threads[2], &n_threads[3], &n_threads[4], &n_threads[5], &n_threads[6], &n_threads[7],
						&stop_read_flag, &resume_read_flag, &vertical_convert, &black_line_convert, &white_line_convert, &forward_flag, &backward_flag,
						&usage_flag, &bug_report_flag,
						language_acts, language_names);
					start_game_button.init(start_game_button_x, start_game_button_y, start_game_button_w, start_game_button_h, start_game_button_r, language.get("button", "start_game"), font15, button_color, button_font_color);
				}

			}
			/*** language ***/

			/*** hint show num ***/
			if (!hint_nums[hint_num]) {
				for (int i = 0; i < 6; ++i) {
					if (hint_nums[i]) {
						hint_num = i;
					}
				}
			}
			/*** hint show num ***/

			/*** analyzing ***/
			if (analyzing) {
				if (fork_mode) {
					if (analyze_idx == (int)fork_history.size()) {
						analyzing = false;
						main_window_active = true;
					}
					else {
						bd = fork_history[analyze_idx].b;
						history_place = fork_history[analyze_idx].b.n - 4;
						if (analyze_state == 0) {
							fork_history[analyze_idx].v = -INF;
							analyze_future = async(launch::async, analyze_search, fork_history[analyze_idx].b, ai_level, use_book, use_book_depth);
							analyze_state = 1;
						}
						else if (analyze_state == 1 && analyze_future.wait_for(chrono::seconds(0)) == future_status::ready) {
							fork_history[analyze_idx].v = analyze_future.get().value;
							if (human_hint) {
								analyze_human_future = async(launch::async, calc_human_value, fork_history[analyze_idx].b, human_value_depth, human_value_search_depth);
								analyze_state = 2;
							}
							else {
								analyze_state = 0;
								++analyze_idx;
							}
						}
						else if (analyze_state == 2 && analyze_human_future.wait_for(chrono::seconds(0)) == future_status::ready) {
							fork_human_value_hist.emplace_back(analyze_human_future.get());
							analyze_state = 0;
							++analyze_idx;
						}
					}
				}
				else {
					if (analyze_idx == (int)history.size()) {
						analyzing = false;
						main_window_active = true;
					}
					else {
						bd = history[analyze_idx].b;
						history_place = history[analyze_idx].b.n - 4;
						if (analyze_state == 0) {
							history[analyze_idx].v = -INF;
							analyze_future = async(launch::async, analyze_search, history[analyze_idx].b, ai_level, use_book, use_book_depth);
							analyze_state = 1;
						}
						else if (analyze_state == 1 && analyze_future.wait_for(chrono::seconds(0)) == future_status::ready) {
							history[analyze_idx].v = analyze_future.get().value;
							if (human_hint) {
								analyze_human_future = async(launch::async, calc_human_value, history[analyze_idx].b, human_value_depth, human_value_search_depth);
								analyze_state = 2;
							}
							else {
								analyze_state = 0;
								++analyze_idx;
							}
						}
						else if (analyze_state == 2 && analyze_human_future.wait_for(chrono::seconds(0)) == future_status::ready) {
							human_value_hist.emplace_back(analyze_human_future.get());
							analyze_state = 0;
							++analyze_idx;
						}
					}
				}
			}
			/*** analyzing ***/

			if (!before_start_game) {
				uint64_t legal = bd.get_legal();
				for (int cell = 0; cell < HW2; ++cell) {
					board_clicked[cell] = false;
				}
				if (!menu.active() && !changing_book && !book_learning && !book_modifying && main_window_active && ((both_human || (human_first && bd.p == BLACK) || (human_second && bd.p == WHITE)) || (history_place != history[history.size() - 1].b.n - 4 && show_mode[1]))) {
					for (int cell = 0; cell < HW2; ++cell) {
						board_clicked[cell] = board_cells[cell].leftClicked() && (1 & (legal >> cell));
					}
				}
				if (not_finished(bd) && (both_human || (human_first && bd.p == BLACK) || (human_second && bd.p == WHITE) || history_place != history[history.size() - 1].b.n - 4)) {
					/*** human plays ***/
					pair<int, Board> moved_board = move_board(bd, board_clicked);
					if (moved_board.first != -1) {
						reset_hint(&hint_state, &hint_future);
						reset_umigame(umigame_state, umigame_future);
						reset_human_value(&human_value_state, &human_value_future);
						reset_analyze(&analyzing, &analyze_state, &analyze_future, &analyze_human_future);
						moved_board.second.check_player();
						bool next_fork_mode = (!fork_mode && history_place != history[history.size() - 1].b.n - 4);
						if (next_fork_mode && history[find_history_idx(history, moved_board.second.n - 4)].b == moved_board.second) {
							bd = moved_board.second;
							history_place = bd.n - 4;
						}
						else if (fork_mode && fork_history[find_history_idx(fork_history, moved_board.second.n - 4)].b == moved_board.second) {
							bd = moved_board.second;
							history_place = bd.n - 4;
						}
						else {
							bd = moved_board.second;
							if (fork_mode || next_fork_mode) {
								while (fork_history.size()) {
									if (fork_history[fork_history.size() - 1].b.n >= bd.n) {
										fork_history.pop_back();
									}
									else {
										break;
									}
								}
								if (!next_fork_mode && fork_history.size()) {
									History_elem hist_tmp = { bd, -INF, moved_board.first, fork_history[fork_history.size() - 1].record + str_record(moved_board.first) };
									fork_history.emplace_back(hist_tmp);
								}
								else {
									History_elem hist_tmp = { bd, -INF, moved_board.first, history[find_history_idx(history, history_place)].record + str_record(moved_board.first) };
									fork_history.emplace_back(hist_tmp);
									fork_mode = true;
								}
							}
							else {
								History_elem hist_tmp = { bd, -INF, moved_board.first, history[history.size() - 1].record + str_record(moved_board.first) };
								history.emplace_back(hist_tmp);
							}
							history_place = bd.n - 4;
						}
					}
					/*** human plays ***/

					/*** hints ***/
					if (not_finished(bd) && use_hint_flag && !analyzing) {
						if (normal_hint && hint_state <= hint_level * 2 + 1) {
							if (hint_state % 2 == 0) {
								if (global_searching) {
									if (hint_state == 0) {
										hint_legal = bd.get_legal();
										for (int cell = 0; cell < HW2; ++cell) {
											hint_value[cell] = 0;
										}
										child_transpose_table.init();
									}
									else if (show_mode[1] && hint_state == hint_level * 2 / 3 * 2) {
										uint64_t n_hint_legal = 0;
										vector<pair<int, int>> legals;
										for (int cell = 0; cell < HW2; ++cell) {
											if (1 & (hint_legal >> cell)) {
												legals.emplace_back(make_pair(-hint_value[cell], cell));
											}
										}
										sort(legals.begin(), legals.end());
										for (int i = 0; i < min((int)legals.size(), max(hint_actual_nums[hint_num] + 4, hint_actual_nums[hint_num] * 3 / 2 + 1)); ++i) {
											n_hint_legal |= 1ULL << legals[i].second;
										}
										for (int i = 0; i < (int)legals.size(); ++i) {
											if (hint_depth[legals[i].second] == SEARCH_BOOK) {
												n_hint_legal |= 1ULL << legals[i].second;
											}
										}
										hint_legal = n_hint_legal;
									}
									hint_future = async(launch::async, ai_hint, bd, hint_state / 2, hint_level, hint_calc_value, hint_calc_depth, hint_best_moves, hint_value, hint_legal);
									++hint_state;
								}
							}
							else if (hint_future.wait_for(chrono::seconds(0)) == future_status::ready) {
								bool hint_calclated = hint_future.get();
								if (hint_calclated) {
									bool is_mid_search = get_level_midsearch(hint_state / 2, bd.n - 4);
									if (global_searching) {
										uint64_t all_legal = bd.get_legal();
										bool all_complete_searched = true;
										for (int cell = 0; cell < HW2; ++cell) {
											if (1 & (hint_legal >> cell)) {
												hint_depth[cell] = hint_calc_depth[cell];
												if (hint_depth[cell] != SEARCH_FINAL && hint_depth[cell] != SEARCH_BOOK) {
													all_complete_searched = false;
												}
												if (hint_state > 2 && hint_depth[cell] != SEARCH_FINAL && is_mid_search) {
													hint_value[cell] += hint_calc_value[cell];
													hint_value[cell] /= 2;
												}
												else {
													hint_value[cell] = hint_calc_value[cell];
												}
											}
											else if (1 & (all_legal >> cell)) {
												hint_value[cell] = -INF;
											}
										}
										if (all_complete_searched) {
											hint_state = hint_level * 2 + 2;
										}
										else {
											++hint_state;
										}
									}
								}
								else {
									++hint_state;
								}
							}
						}
						if (show_mode[1] && umigame_hint) {
							for (int cell = 0; cell < HW2; ++cell) {
								if ((1 & (legal >> cell))) {
									if (umigame_state[cell] == 0) {
										Flip m;
										calc_flip(&m, &bd, cell);
										Board moved_b = bd.move_copy(&m);
										if (book.get(&moved_b) != -INF) {
											umigame_future[cell] = get_umigame(moved_b);
											umigame_state[cell] = 1;
										}
										else {
											umigame_state[cell] = 3;
										}
									}
									else if (umigame_state[cell] == 1) {
										if (umigame_future[cell].wait_for(chrono::seconds(0)) == future_status::ready) {
											umigame_value[cell] = umigame_future[cell].get();
											umigame_state[cell] = 2;
										}
									}
								}
							}
						}
						if (show_mode[1] && human_hint) {
							if (human_value_state == 0) {
								human_value_future = get_human_value(bd, human_value_depth, human_value, human_value_search_depth);
								human_value_state = 1;
							}
							else if (human_value_state == 1) {
								if (human_value_future.wait_for(chrono::seconds(0)) == future_status::ready) {
									human_value_future.get();
									human_value_state = 2;
								}
							}
						}
					}
					/*** hints ***/

					/*** change book ***/
					if (show_mode[1]) {
						uint64_t legal = bd.get_legal();
						for (int cell = 0; cell < HW2; ++cell) {
							if (1 & (legal >> cell)) {
								if (board_cells[cell].rightClicked()) {
									if (changing_book && cell == change_book_cell) {
										if (changed_book_value_str.size() == 0) {
											changing_book = false;
										}
										else {
											int changed_book_value = ParseOr<int>(changed_book_value_str, -1000);
											if (changed_book_value != -1000) {
												reset_hint(&hint_state, &hint_future);
												Flip m;
												calc_flip(&m, &bd, cell);
												book.change(bd.move_copy(&m), changed_book_value);
												changed_book_value_str.clear();
												book_changed = true;
												changing_book = false;
												hint_state = 0;
											}
										}
									}
									else {
										changing_book = true;
										change_book_cell = cell;
									}
								}
							}
						}
						if (changing_book && KeyEnter.down()) {
							if (changed_book_value_str.size() == 0) {
								changing_book = false;
							}
							else {
								int changed_book_value = ParseOr<int>(changed_book_value_str, -1000);
								if (changed_book_value != -1000) {
									reset_hint(&hint_state, &hint_future);
									Flip m;
									calc_flip(&m, &bd, change_book_cell);
									book.change(bd.move_copy(&m), changed_book_value);
									changed_book_value_str.clear();
									book_changed = true;
									changing_book = false;
									hint_state = 0;
								}
							}
						}
						if (changing_book && KeyEscape.down()) {
							changing_book = false;
							changed_book_value_str.clear();
						}
						if (changing_book) {
							if (Key0.down() || KeyNum0.down()) {
								if (ParseOr<int>(changed_book_value_str + U"0", -1000) != -1000) {
									changed_book_value_str += U"0";
								}
							}
							else if (Key1.down() || KeyNum1.down()) {
								if (ParseOr<int>(changed_book_value_str + U"1", -1000) != -1000) {
									changed_book_value_str += U"1";
								}
							}
							else if (Key2.down() || KeyNum2.down()) {
								if (ParseOr<int>(changed_book_value_str + U"2", -1000) != -1000) {
									changed_book_value_str += U"2";
								}
							}
							else if (Key3.down() || KeyNum3.down()) {
								if (ParseOr<int>(changed_book_value_str + U"3", -1000) != -1000) {
									changed_book_value_str += U"3";
								}
							}
							else if (Key4.down() || KeyNum4.down()) {
								if (ParseOr<int>(changed_book_value_str + U"4", -1000) != -1000) {
									changed_book_value_str += U"4";
								}
							}
							else if (Key5.down() || KeyNum5.down()) {
								if (ParseOr<int>(changed_book_value_str + U"5", -1000) != -1000) {
									changed_book_value_str += U"5";
								}
							}
							else if (Key6.down() || KeyNum6.down()) {
								if (ParseOr<int>(changed_book_value_str + U"6", -1000) != -1000) {
									changed_book_value_str += U"6";
								}
							}
							else if (Key7.down() || KeyNum7.down()) {
								if (ParseOr<int>(changed_book_value_str + U"7", -1000) != -1000) {
									changed_book_value_str += U"7";
								}
							}
							else if (Key8.down() || KeyNum8.down()) {
								if (ParseOr<int>(changed_book_value_str + U"8", -1000) != -1000) {
									changed_book_value_str += U"8";
								}
							}
							else if (Key9.down() || KeyNum9.down()) {
								if (ParseOr<int>(changed_book_value_str + U"9", -1000) != -1000) {
									changed_book_value_str += U"9";
								}
							}
							else if (KeyMinus.down()) {
								if (changed_book_value_str.size() == 0) {
									changed_book_value_str += U"-";
								}
							}
							else if (KeyBackspace.down()) {
								if (changed_book_value_str.size())
									changed_book_value_str.pop_back();
							}
							show_change_book(change_book_cell, changed_book_value_str, font20);
						}
					}
					/*** change book ***/
				}
				/*** ai plays ***/
				else if (not_finished(bd) && history[history.size() - 1].b.n - 4 == history_place && !book_learning && !book_modifying) {
					if (ai_thinking) {
						if (ai_future.wait_for(chrono::seconds(0)) == future_status::ready) {
							Search_result ai_result = ai_future.get();
							int sgn = (bd.p ? -1 : 1);
							Flip flip;
							calc_flip(&flip, &bd, ai_result.policy);
							bd.move(&flip);
							if (bd.check_player()) {
								popup_start_time = tim();
							}
							int v = sgn * ai_result.value;
							if (history.size()) {
								history[history.size() - 1].v = v;
							}
							History_elem hist_tmp = { bd, -INF, flip.pos, history[history.size() - 1].record + str_record(flip.pos) };
							history.emplace_back(hist_tmp);
							history_place = bd.n - 4;
							ai_value = ai_result.value;
							ai_thinking = false;
							reset_hint(&hint_state, &hint_future);
							reset_umigame(umigame_state, umigame_future);
							reset_human_value(&human_value_state, &human_value_future);
							reset_analyze(&analyzing, &analyze_state, &analyze_future, &analyze_human_future);
						}
					}
					else if (global_searching) {
						ai_future = async(launch::async, ai, bd, ai_level, use_book && bd.n - 3 <= use_book_depth, show_mode[2] ? 0 : ai_book_accept);
						ai_thinking = true;
					}
				}
				/*** ai plays ***/
			}

			/*** graph interaction ***/
			if (main_window_active && !ai_thinking && !book_learning && !book_modifying && !changing_book) {
				int former_history_place = history_place;
				if (!KeyLeft.pressed() && !KeyA.pressed()) {
					left_pushed = BUTTON_NOT_PUSHED;
				}
				if (!KeyRight.pressed() && !KeyD.pressed()) {
					right_pushed = BUTTON_NOT_PUSHED;
				}
				if (KeyLeft.down() || KeyA.down() || (left_pushed != BUTTON_NOT_PUSHED && tim() - left_pushed >= BUTTON_LONG_PRESS_THRESHOLD) || backward_flag) {
					history_place = max(0, history_place - 1);
					if (KeyLeft.down() || KeyA.down()) {
						left_pushed = tim();
					}
				}
				else if (KeyRight.down() || KeyD.down() || (right_pushed != BUTTON_NOT_PUSHED && tim() - right_pushed >= BUTTON_LONG_PRESS_THRESHOLD) || forward_flag) {
					if (fork_mode) {
						history_place = min(fork_history[fork_history.size() - 1].b.n - 4, history_place + 1);
					}
					else {
						history_place = min(history[history.size() - 1].b.n - 4, history_place + 1);
					}
					if (KeyRight.down() || KeyD.down()) {
						right_pushed = tim();
					}
				}
				else if (use_value_flag) {
					history_place = graph.update_place(history, fork_history, history_place);
				}
				if (history_place != former_history_place) {
					if (ai_thinking) {
						reset_ai(&ai_thinking, &ai_future);
					}
					if (fork_history.size()) {
						if (history_place > fork_history[fork_history.size() - 1].b.n - 4) {
							history_place = fork_history[fork_history.size() - 1].b.n - 4;
						}
						if (history_place < fork_history[0].b.n - 4) {
							fork_mode = false;
						}
					}
					if (!fork_mode) {
						bd = history[find_history_idx(history, history_place)].b;
						fork_history.clear();
						reset_hint(&hint_state, &hint_future);
						reset_umigame(umigame_state, umigame_future);
						reset_human_value(&human_value_state, &human_value_future);
						reset_analyze(&analyzing, &analyze_state, &analyze_future, &analyze_human_future);
					}
					else {
						bd = fork_history[find_history_idx(fork_history, history_place)].b;
						reset_hint(&hint_state, &hint_future);
						reset_umigame(umigame_state, umigame_future);
						reset_human_value(&human_value_state, &human_value_future);
						reset_analyze(&analyzing, &analyze_state, &analyze_future, &analyze_human_future);
					}
				}
			}
			/*** graph interaction ***/

			/*** Board draw ***/
			if (book_learning || book_modifying) {
				History_elem history_elem;
				history_elem.b = bd;
				history_elem.policy = -1;
				history_elem.record = U"";
				history_elem.v = -INF;
				board_draw(board_cells, history_elem, -1, int_mode, use_hint_flag, normal_hint, human_hint, umigame_hint,
						hint_state, hint_legal, hint_value, hint_depth, hint_best_moves, hint_actual_nums[hint_num], normal_hint_font, small_hint_font, font30, mini_hint_font, board_coord_font,
						before_start_game,
						umigame_state, umigame_value,
						human_value_state, human_value,
						book_start_learn);
			}
			else if (!fork_mode) {
				if (analyzing && analyze_idx < (int)history.size()) {
					int next_policy = -1;
					if ((int)history.size() > analyze_idx + 1) {
						next_policy = history[analyze_idx + 1].policy;
					}
					board_draw(board_cells, history[analyze_idx], next_policy, int_mode, use_hint_flag, normal_hint, human_hint, umigame_hint,
						hint_state, hint_legal, hint_value, hint_depth, hint_best_moves, hint_actual_nums[hint_num], normal_hint_font, small_hint_font, font30, mini_hint_font, board_coord_font,
						before_start_game,
						umigame_state, umigame_value,
						human_value_state, human_value,
						book_start_learn);
				}
				else {
					int history_idx = find_history_idx(history, history_place);
					int next_policy = -1;
					if ((int)history.size() > history_idx + 1) {
						next_policy = history[history_idx + 1].policy;
					}
					board_draw(board_cells, history[history_idx], next_policy, int_mode, use_hint_flag, normal_hint, human_hint, umigame_hint,
						hint_state, hint_legal, hint_value, hint_depth, hint_best_moves, hint_actual_nums[hint_num], normal_hint_font, small_hint_font, font30, mini_hint_font, board_coord_font,
						before_start_game,
						umigame_state, umigame_value,
						human_value_state, human_value,
						book_start_learn);
				}
			}
			else {
				if (analyzing && analyze_idx < (int)fork_history.size()) {
					int next_policy = -1;
					if ((int)fork_history.size() > analyze_idx + 1) {
						next_policy = fork_history[analyze_idx + 1].policy;
					}
					board_draw(board_cells, fork_history[analyze_idx], next_policy, int_mode, use_hint_flag, normal_hint, human_hint, umigame_hint,
						hint_state, hint_legal, hint_value, hint_depth, hint_best_moves, hint_actual_nums[hint_num], normal_hint_font, small_hint_font, font30, mini_hint_font, board_coord_font,
						before_start_game,
						umigame_state, umigame_value,
						human_value_state, human_value,
						book_start_learn);
				}
				else {
					int history_idx = find_history_idx(fork_history, history_place);
					int next_policy = -1;
					if ((int)fork_history.size() > history_idx + 1) {
						next_policy = fork_history[history_idx + 1].policy;
					}
					board_draw(board_cells, fork_history[history_idx], next_policy, int_mode, use_hint_flag, normal_hint, human_hint, umigame_hint,
						hint_state, hint_legal, hint_value, hint_depth, hint_best_moves, hint_actual_nums[hint_num], normal_hint_font, small_hint_font, font30, mini_hint_font, board_coord_font,
						before_start_game,
						umigame_state, umigame_value,
						human_value_state, human_value,
						book_start_learn);
				}
			}
			/*** Board draw ***/

			/*** joseki ***/
			string new_joseki;
			joseki_name.clear();
			int history_idx = find_history_idx(history, history_place);
			for (int i = 0; i <= history_idx; ++i) {
				new_joseki = joseki.get(history[i].b);
				if (new_joseki != "") {
					joseki_name = new_joseki;
				}
			}
			if (fork_mode) {
				int history_idx = find_history_idx(fork_history, history_place);
				for (int i = 0; i <= history_idx; ++i) {
					new_joseki = joseki.get(fork_history[i].b);
					if (new_joseki != "") {
						joseki_name = new_joseki;
					}
				}
			}
			/*** joseki ***/

			/*** info draw ***/
			info_draw(bd, joseki_name, ai_level, hint_level, font20, font15);
			/*** info draw ***/

			/*** human sense value draw ***/
			if (use_value_flag) {
				human_sense_graph.draw(human_value_hist, bd);
			}
			/*** human sense value draw ***/

			/*** graph draw ***/
			if (use_value_flag) {
				graph.draw(history, fork_history, history_place);
			}
			/*** graph draw ***/

			/*** before and after game ***/
			if (!before_start_game) {
				if (bd.get_legal() == 0ULL && !fork_mode && show_popup_flag && show_end_popup) {
					show_popup_flag = !show_popup(bd, !both_human, human_first, human_second, both_ai, ai_level, font50, font30, popup_start_time);
					main_window_active = !show_popup_flag;
					if (main_window_active) {
						global_searching = true;
						System::Sleep(100);
					}
				}
			}
			else if (human_second || both_ai) {
				start_game_button.draw();
				main_window_active = false;
				if (!menu.active() && start_game_button.clicked()) {
					cerr << "start game!" << endl;
					before_start_game = false;
					main_window_active = true;
					if (history.size()) {
						history_place = history[history.size() - 1].b.n - 4;
					}
					else {
						history_place = 0;
					}
					if (main_window_active) {
						global_searching = true;
						System::Sleep(100);
					}
				}
			}
			else {
				cerr << "start game!" << endl;
				before_start_game = false;
				main_window_active = true;
				if (history.size()) {
					history_place = history[history.size() - 1].b.n - 4;
				}
				else {
					history_place = 0;
				}
				if (main_window_active) {
					global_searching = true;
					System::Sleep(100);
				}
			}
			/*** before and after game ***/

			/*** output game ***/
			if (outputting_game) {
				int output_state = output_game_popup(font40, font30, font20, &black_player, &white_player, &game_memo, output_active);
				if (output_state == 1) {
					if (fork_mode) {
						if (output_game(fork_history[find_history_idx(fork_history, history_place)], ai_level, use_ai_mode, black_player, white_player, game_memo)) {
							cerr << "game saved" << endl;
						}
						else {
							cerr << "game save FAILED" << endl;
						}
					}
					else {
						if (output_game(history[find_history_idx(history, history_place)], ai_level, use_ai_mode, black_player, white_player, game_memo)) {
							cerr << "game saved" << endl;
						}
						else {
							cerr << "game save FAILED" << endl;
						}
					}
					outputting_game = false;
					main_window_active = true;
					System::Sleep(100);
				}
				else if (output_state == 2) {
					outputting_game = false;
					main_window_active = true;
				}
			}
			/*** output game ***/

			/*** importing record ***/
			if (inputting_record) {
				int state = import_record_popup(font40, font30, font20, &imported_record);
				if (state == 1) {
					vector<History_elem> n_history;
					if (import_record(imported_record, &n_history)) {
						history.clear();
						fork_history.clear();
						for (History_elem elem : n_history) {
							history.emplace_back(elem);
						}
						bd = history[history.size() - 1].b;
						history_place = bd.n - 4;
						human_value_hist.clear();
						fork_human_value_hist.clear();
						fork_mode = false;
						before_start_game = true;
						show_popup_flag = true;
						joseki_name.clear();
						reset_hint(&hint_state, &hint_future);
						reset_umigame(umigame_state, umigame_future);
						reset_human_value(&human_value_state, &human_value_future);
						reset_ai(&ai_thinking, &ai_future);
						reset_analyze(&analyzing, &analyze_state, &analyze_future, &analyze_human_future);
						before_start_game = false;
						main_window_active = true;
						inputting_record = false;
						System::Sleep(100);
					}
				}
				else if (state == 2) {
					main_window_active = true;
					inputting_record = false;
				}
			}
			/*** importing record ***/

			/*** importing board ***/
			if (inputting_board) {
				int state = import_board_popup(font40, font30, font20, &imported_board);
				if (state == 1) {
					pair<bool, Board> imported = import_board(imported_board);
					if (imported.first) {
						bd = imported.second;
						history.clear();
						fork_history.clear();
						History_elem hist_tmp = { bd, -INF, -1, U"" };
						history.emplace_back(hist_tmp);
						history_place = bd.n - 4;
						human_value_hist.clear();
						fork_human_value_hist.clear();
						fork_mode = false;
						before_start_game = true;
						show_popup_flag = true;
						joseki_name.clear();
						reset_hint(&hint_state, &hint_future);
						reset_umigame(umigame_state, umigame_future);
						reset_human_value(&human_value_state, &human_value_future);
						reset_ai(&ai_thinking, &ai_future);
						reset_analyze(&analyzing, &analyze_state, &analyze_future, &analyze_human_future);
						before_start_game = false;
						main_window_active = true;
						inputting_board = false;
						System::Sleep(100);
					}
				}
				else if (state == 2) {
					main_window_active = true;
					inputting_board = false;
				}
			}
			/*** importing board ***/

			/*** book learn ***/
			if (book_start_learn && !book_learning && !book_modifying) {
				book_start_learn = false;
				book_learn_future.get();
			}
			/*** book learn ***/

			/*** book modification ***/
			if (book_modifying) {
				if (modify_book_future.wait_for(chrono::seconds(0)) == future_status::ready) {
					modify_book_future.get();
					book_modifying = false;
				}
			}
			/*** book modification ***/

			/*** ai stop calculating ***/
			if (ai_thinking && (both_human || (human_first && bd.p == BLACK) || (human_second && bd.p == WHITE))) {
				reset_hint(&hint_state, &hint_future);
				reset_umigame(umigame_state, umigame_future);
				reset_human_value(&human_value_state, &human_value_future);
				reset_ai(&ai_thinking, &ai_future);
				reset_analyze(&analyzing, &analyze_state, &analyze_future, &analyze_human_future);
			}
			/*** ai stop calculating ***/

			/*** menu buttons ***/
			if (start_game_flag && !book_learning && !book_modifying) {
				cerr << "reset" << endl;
				bd.reset();
				history.clear();
				History_elem hist_tmp = { bd, -INF, -1, U"" };
				history.emplace_back(hist_tmp);
				fork_history.clear();
				human_value_hist.clear();
				fork_human_value_hist.clear();
				history_place = 0;
				fork_mode = false;
				before_start_game = true;
				show_popup_flag = true;
				joseki_name.clear();
				reset_hint(&hint_state, &hint_future);
				reset_umigame(umigame_state, umigame_future);
				reset_human_value(&human_value_state, &human_value_future);
				reset_ai(&ai_thinking, &ai_future);
				reset_analyze(&analyzing, &analyze_state, &analyze_future, &analyze_human_future);
			}
			else if (analyze_flag && !book_learning && !book_modifying) {
				reset_hint(&hint_state, &hint_future);
				reset_umigame(umigame_state, umigame_future);
				reset_human_value(&human_value_state, &human_value_future);
				reset_ai(&ai_thinking, &ai_future);
				reset_analyze(&analyzing, &analyze_state, &analyze_future, &analyze_human_future);
				human_value_hist.clear();
				fork_human_value_hist.clear();
				analyzing = true;
				main_window_active = false;
				analyze_state = false;
				analyze_idx = 0;
				before_start_game = false;
			}
			else if (output_record_flag && !book_learning) {
				if (fork_mode) {
					Clipboard::SetText(fork_history[find_history_idx(fork_history, history_place)].record);
				}
				else {
					Clipboard::SetText(history[find_history_idx(history, history_place)].record);
				}
				cerr << "record copied" << endl;
			}
			else if (output_game_flag && !before_start_game && main_window_active && !book_learning && !book_modifying && !inputting_record && !inputting_board) {
				if (!both_human) {
					if (both_ai) {
						black_player = U"Egaroucid";
						white_player = U"Egaroucid";
					}
					else if (human_first) {
						if (black_player == U"Egaroucid") {
							black_player = U"";
						}
						white_player = U"Egaroucid";
					}
					else if (human_second) {
						if (white_player == U"Egaroucid") {
							white_player = U"";
						}
						black_player = U"Egaroucid";
					}
				}
				else {
					if (black_player == U"Egaroucid") {
						black_player = U"";
					}
					if (white_player == U"Egaroucid") {
						white_player = U"";
					}
				}
				outputting_game = true;
				main_window_active = false;
			}
			else if (input_record_flag && !book_learning && !book_modifying && !outputting_game && !inputting_board) {
				inputting_record = true;
				main_window_active = false;
				imported_record.clear();
			}
			else if (input_board_flag && !book_learning && !book_modifying && !outputting_game && !inputting_record) {
				inputting_board = true;
				main_window_active = false;
				imported_board.clear();
			}
			else if (stop_read_flag) {
				cerr << "stop calculating" << endl;
				reset_hint(&hint_state, &hint_future);
				reset_umigame(umigame_state, umigame_future);
				reset_human_value(&human_value_state, &human_value_future);
				reset_ai(&ai_thinking, &ai_future);
				reset_analyze(&analyzing, &analyze_state, &analyze_future, &analyze_human_future);
				hint_state = INF;
				for (int i = 0; i < HW2; ++i) {
					umigame_state[i] = INF;
				}
				human_value_state = INF;
				global_searching = false;
			}
			else if (resume_read_flag) {
				cerr << "resume calculating" << endl;
				global_searching = true;
			}
			else if (vertical_convert && !analyzing && !book_learning && !book_modifying && !ai_thinking) {
				bd.board_rotate_180();
				String record = U"";
				for (History_elem& elem : history) {
					elem.b.board_rotate_180();
					if (elem.policy != -1) {
						record += str_record(HW2_M1 - elem.policy);
						elem.policy = HW2_M1 - elem.policy;
						elem.record = record;
					}
				}
				if (fork_history.size()) {
					record = history[find_history_idx(history, fork_history[0].b.n - 5)].record;
					for (History_elem& elem : fork_history) {
						elem.b.board_rotate_180();
						if (elem.policy != -1) {
							record += str_record(HW2_M1 - elem.policy);
							elem.policy = HW2_M1 - elem.policy;
							elem.record = record;
						}
					}
				}
				reset_hint(&hint_state, &hint_future);
				reset_umigame(umigame_state, umigame_future);
				reset_human_value(&human_value_state, &human_value_future);
				reset_ai(&ai_thinking, &ai_future);
			}
			else if (black_line_convert && !analyzing && !book_learning && !book_modifying && !ai_thinking) {
				bd.board_black_line_mirror();
				String record = U"";
				for (History_elem& elem : history) {
					elem.b.board_black_line_mirror();
					if (elem.policy != -1) {
						int y = HW_M1 - elem.policy % HW;
						int x = HW_M1 - elem.policy / HW;
						record += str_record(y * HW + x);
						elem.policy = y * HW + x;
						elem.record = record;
					}
				}
				if (fork_history.size()) {
					record = history[find_history_idx(history, fork_history[0].b.n - 5)].record;
					for (History_elem& elem : fork_history) {
						elem.b.board_black_line_mirror();
						if (elem.policy != -1) {
							int y = HW_M1 - elem.policy % HW;
							int x = HW_M1 - elem.policy / HW;
							record += str_record(y * HW + x);
							elem.policy = y * HW + x;
							elem.record = record;
						}
					}
				}
				reset_hint(&hint_state, &hint_future);
				reset_umigame(umigame_state, umigame_future);
				reset_human_value(&human_value_state, &human_value_future);
				reset_ai(&ai_thinking, &ai_future);
			}
			else if (white_line_convert && !analyzing && !book_learning && !book_modifying && !ai_thinking) {
				bd.board_white_line_mirror();
				String record = U"";
				for (History_elem& elem : history) {
					elem.b.board_white_line_mirror();
					if (elem.policy != -1) {
						int y = elem.policy % HW;
						int x = elem.policy / HW;
						record += str_record(y * HW + x);
						elem.policy = y * HW + x;
						elem.record = record;
					}
				}
				if (fork_history.size()) {
					record = history[find_history_idx(history, fork_history[0].b.n - 5)].record;
					for (History_elem& elem : fork_history) {
						elem.b.board_white_line_mirror();
						if (elem.policy != -1) {
							int y = elem.policy % HW;
							int x = elem.policy / HW;
							record += str_record(y * HW + x);
							elem.policy = y * HW + x;
							elem.record = record;
						}
					}
				}
				reset_hint(&hint_state, &hint_future);
				reset_umigame(umigame_state, umigame_future);
				reset_human_value(&human_value_state, &human_value_future);
				reset_ai(&ai_thinking, &ai_future);
			}
			else if (start_book_learn_flag && !before_start_game && !book_learning && !book_modifying) {
				book_learning = true;
				book_start_learn = true;
				book_changed = true;
				book_learn_future = async(launch::async, learn_book, bd, ai_level, book_depth, book_learn_accept, &bd, &bd_value, &book_learning);
			}
			else if (stop_book_learn_flag || (book_learning && !show_mode[1])) {
				if (book_learning) {
					book_learning = false;
					global_searching = false;
					book_learn_future.get();
					global_searching = true;
				}
				book_start_learn = false;
			}
			else if (import_book_flag && !book_learning && !book_modifying) {
				importing_book = 1;
			}
			else if (book_modify && !book_learning && !book_modifying) {
				if (book.get(&bd) != -INF) {
					cerr << "modifying book..." << endl;
					book_modifying = true;
					book_changed = true;
					modify_book_future = async(launch::async, modify_book_parent, &bd);
				}
			}
			else if (usage_flag) {
				System::LaunchBrowser(U"https://www.egaroucid-app.nyanyan.dev/usage/");
			}
			else if (bug_report_flag) {
				System::LaunchBrowser(U"https://docs.google.com/forms/d/e/1FAIpQLSd6ML1T1fc707luPEefBXuImMnlM9cQP8j-YHKiSyFoS-8rmQ/viewform?usp=sf_link");
			}
			else if (show_end_popup_change != show_end_popup) {
				if (!main_window_active && show_end_popup) {
					main_window_active = true;
				}
				show_end_popup = show_end_popup_change;
			}
			/*** menu buttons ***/

			menu.draw();
		}
		/*** initialized ***/
	}

}
