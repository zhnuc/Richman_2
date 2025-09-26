#ifndef BLOCK_SYSTEM_H
#define BLOCK_SYSTEM_H

#include "game_types.h"

// 路障系统常量
#define BLOCK_RANGE 10          // 路障放置最大距离
#define BLOCK_SYMBOL '#'        // 路障在地图上的显示符号

// 炸弹系统常量
#define BOMB_RANGE 10           // 炸弹放置最大距离
#define BOMB_SYMBOL '@'         // 炸弹在地图上的显示符号
#define HOSPITAL_DAYS 3         // 炸弹爆炸后住院天数

// 路障命令处理函数
bool handle_block_command(Player* player, int relative_distance);

// 炸弹命令处理函数
bool handle_bomb_command(Player* player, int relative_distance);

// 路障放置相关函数
bool place_block(int player_index, int target_location);
bool is_valid_block_position(int current_pos, int relative_distance);
int calculate_block_position(int current_pos, int relative_distance);

// 炸弹放置相关函数
bool place_bomb(int player_index, int target_location);
bool is_valid_bomb_position(int current_pos, int relative_distance);
int calculate_bomb_position(int current_pos, int relative_distance);

// 路障拦截相关函数
bool check_block_interception(int location);
void trigger_block_interception(Player* player, int location);
void remove_block(int location);

// 炸弹爆炸相关函数
bool check_bomb_explosion(int location);
void trigger_bomb_explosion(Player* player, int location);
void remove_bomb(int location);

// 路障查询函数
bool has_block_at_location(int location);
void display_all_blocks(void);

// 炸弹查询函数
void display_all_bombs(void);

// 位置检查辅助函数
bool is_special_building(int location);
bool has_player_at_location(int location);
bool has_bomb_at_location(int location);

#endif // BLOCK_SYSTEM_H
