#ifndef BLOCK_SYSTEM_H
#define BLOCK_SYSTEM_H

#include "game_types.h"

// 路障系统常量
#define BLOCK_RANGE 10          // 路障放置最大距离
#define BLOCK_SYMBOL '#'        // 路障在地图上的显示符号

// 路障命令处理函数
bool handle_block_command(Player* player, int relative_distance);

// 路障放置相关函数
bool place_block(int player_index, int target_location);
bool is_valid_block_position(int current_pos, int relative_distance);
int calculate_block_position(int current_pos, int relative_distance);

// 路障拦截相关函数
bool check_block_interception(int location);
void trigger_block_interception(Player* player, int location);
void remove_block(int location);

// 路障查询函数
bool has_block_at_location(int location);
void display_all_blocks(void);

// 位置检查辅助函数
bool is_special_building(int location);
bool has_player_at_location(int location);
bool has_bomb_at_location(int location);

#endif // BLOCK_SYSTEM_H
