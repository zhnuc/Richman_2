#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "game_types.h"

// 全局游戏状态声明
extern GameState g_game_state;

// 游戏状态管理函数声明
void init_game_state(void);
void print_game_state(void);
GameState* get_game_state(void);

#endif // GAME_STATE_H