#ifndef PROP_SHOP_H
#define PROP_SHOP_H

#include "game_types.h"

// 道具屋位置常量
#define PROP_SHOP_LOCATION 28

// 道具价格常量
#define PROP_BARRIER_PRICE 50    // 路障价格
#define PROP_ROBOT_PRICE   30    // 机器娃娃价格
#define PROP_BOMB_PRICE    50    // 炸弹价格

// 道具屋核心功能函数
void enter_prop_shop(Player* player);
void show_prop_shop_menu(void);
bool buy_prop(Player* player, int prop_id);
bool can_afford_prop(Player* player, int prop_id);
bool has_prop_space(Player* player);
int get_prop_price(int prop_id);
const char* get_prop_name(int prop_id);

#endif // PROP_SHOP_H
