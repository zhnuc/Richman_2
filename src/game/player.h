#ifndef PLAYER_H
#define PLAYER_H

#include "game_types.h"

// 玩家管理函数声明
Player* create_player_by_character(int character_id, int fund);
Player* create_player(int index, const char* name, int fund);
void free_player(Player* player);
bool is_player_valid(const Player* player);
void print_player_info(const Player* player);

#endif // PLAYER_H