#ifndef CHARACTER_H
#define CHARACTER_H

#include "game_types.h"

// 角色管理函数声明
void init_characters(void);
void show_character_selection(void);
Character* get_character_by_id(int id);
Character* get_character_by_index(int index);
bool is_valid_character_id(int id);

// 全局角色数组
extern Character g_characters[4];

#endif // CHARACTER_H