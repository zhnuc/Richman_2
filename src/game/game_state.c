#include "game_state.h"
#include <stdio.h>
#include <string.h>

// 全局游戏状态定义
GameState g_game_state;

void init_game_state(void) {
    g_game_state.player_count = 0;
    
    // 初始化房产
    for (int i = 0; i < 40; i++) {
        strcpy(g_game_state.houses[i].owner, "");
        g_game_state.houses[i].level = 0;
    }
    
    // 初始化道具
    g_game_state.placed_prop.bomb_count = 0;
    g_game_state.placed_prop.barrier_count = 0;
    
    // 初始化游戏信息
    g_game_state.game.now_player = 0;
    g_game_state.game.next_player = 1;
    g_game_state.game.started = false;
    g_game_state.game.ended = false;
    g_game_state.game.winner = -1;
}

void print_game_state(void) {
    printf("=== 游戏状态 ===\n");
    printf("玩家数量: %d\n", g_game_state.player_count);
    
    for (int i = 0; i < g_game_state.player_count; i++) {
        Player* p = &g_game_state.players[i];
        printf("玩家%d: %s, 资金:%d, 位置:%d, 存活:%s\n", 
               p->index, p->name, p->fund, p->location, 
               p->alive ? "是" : "否");
    }
    
    printf("当前玩家: %d\n", g_game_state.game.now_player);
    printf("游戏状态: %s\n", g_game_state.game.started ? "已开始" : "未开始");
}

GameState* get_game_state(void) {
    return &g_game_state;
}