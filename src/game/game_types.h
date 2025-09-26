#ifndef GAME_TYPES_H
#define GAME_TYPES_H

#include <stdbool.h>

#define MAX_PLAYERS 4
#define MAP_SIZE 70
#define MAX_PROPS 10
#define MAX_NAME_LENGTH 32

// 角色信息结构
typedef struct {
    int id;           // 角色ID (1-4)
    char name[MAX_NAME_LENGTH];    // 角色名称
    char display_name[MAX_NAME_LENGTH]; // 显示名称
    const char* color_code; // 颜色代码
} Character;

// 道具结构
typedef struct {
    int bomb;       // 玩家拥有炸弹个数
    int barrier;    // 玩家拥有路障个数
    int robot;      // 玩家拥有机器人个数
    int total;      // 玩家拥有道具总计个数
} Prop;

// 状态效果结构 (buff)
typedef struct {
    int god;      // 财神剩余轮数
    int prison;   // 监狱剩余轮数
    int hospital; // 医院剩余轮数
} Buff;

// 玩家结构
typedef struct {
    int index;
    char name[MAX_NAME_LENGTH];
    const char* color; // 用于存储ANSI颜色代码
    int fund;
    int credit;
    int location;
    bool alive;
    Prop prop;
    Buff buff;
} Player;

// 房屋结构
typedef struct {
    int id;
    int price;
    int level;
    int owner_id; // -1 for no owner
} House;

// 放置的道具结构
typedef struct {
    int bomb[MAP_SIZE];    // 炸弹位置列表, 1表示有
    int barrier[MAP_SIZE]; // 路障位置列表, 1表示有
} PlacedProp;

// 游戏核心状态结构
typedef struct {
    int now_player_id; // 当前操作玩家
    int next_player_id; // 下一操作玩家
    bool ended;        // 游戏是否结束
    int winner_id;     // 胜者 ID，未结束为 -1
} Game;

// 顶层游戏状态结构
typedef struct {
    Player players[MAX_PLAYERS];
    int player_count;
    House houses[MAP_SIZE];
    PlacedProp placed_prop;
    Game game;
} GameState;

#endif // GAME_TYPES_H