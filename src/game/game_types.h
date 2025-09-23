#ifndef GAME_TYPES_H
#define GAME_TYPES_H

#include <stdbool.h>

// 玩家道具结构
typedef struct {
    int bomb;
    int barrier;
    int robot;
    int total;
} PlayerProp;

// 玩家Buff结构
typedef struct {
    int god;        // 财神附体，没有为 0
    int prison;     // 蹲监狱，没有为 0
    int hospital;   // 住院，没有为 0
} PlayerBuff;

// 角色信息结构
typedef struct {
    int id;           // 角色ID (1-4)
    char name[32];    // 角色名称
    char display_name[32]; // 显示名称
} Character;

// 玩家结构
typedef struct {
    int index;        // 从 0 开始（0：Q， 1：A， 2：S，3：J）
    char name[32];    // 名称（钱夫人，阿土伯，孙小美，金贝贝）
    int fund;         // 资金
    int credit;       // 点数
    int location;     // 当前位置，从起始点开始顺时针，从 0 开始
    bool alive;       // 玩家是否存活
    PlayerProp prop;  // 道具信息
    PlayerBuff buff;  // Buff信息
} Player;

// 房产结构
typedef struct {
    char owner[32];
    int level;      // 等级（0-3）
} House;

// 已放置道具结构
typedef struct {
    int bomb_positions[20];
    int bomb_count;
    int barrier_positions[20];
    int barrier_count;
} PlacedProp;

// 游戏信息结构
typedef struct {
    int now_player;
    int next_player;
    bool started;
    bool ended;
    int winner;
} GameInfo;

// 游戏状态结构
typedef struct {
    Player players[4];
    int player_count;
    House houses[40];  // 假设40个位置的棋盘
    PlacedProp placed_prop;
    GameInfo game;
} GameState;

#endif // GAME_TYPES_H