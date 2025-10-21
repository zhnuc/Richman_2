#include "game_state.h"
#include <stdio.h>
#include <string.h>

// 全局游戏状态定义
GameState g_game_state;

void init_game_state(void) {
    g_game_state.player_count = 0;
    g_game_state.game.started = false;
    g_game_state.game.ended = false;
    g_game_state.game.now_player_id = 0;
    g_game_state.game.last_player_id = 0;
    g_game_state.game.pending_interaction_player_id = 0;

    // 初始化房产
    for (int i = 0; i < MAP_SIZE; i++) {
        g_game_state.houses[i].id = i;
        g_game_state.houses[i].level = 0;
        g_game_state.houses[i].owner_id = -1; // 无人拥有
        
        // 根据位置设置价格
        if ((i >= 1 && i <= 13) || (i >= 15 && i <= 27)) { // 地段1
            g_game_state.houses[i].price = 200;
        } else if ((i >= 29 && i <= 34)) { // 地段2
            g_game_state.houses[i].price = 500;
        } else if ((i >= 36 && i <= 48) || (i >= 50 && i <= 62)) { // 地段3
            g_game_state.houses[i].price = 300;
        } else {
            g_game_state.houses[i].price = 0; // 特殊位置不可购买
        }
    }
    
    // 初始化道具
    memset(g_game_state.placed_prop.bomb, 0, sizeof(g_game_state.placed_prop.bomb));
    memset(g_game_state.placed_prop.barrier, 0, sizeof(g_game_state.placed_prop.barrier));
    
    // 初始化财神状态
    g_game_state.god.spawn_cooldown = 10;
    g_game_state.god.location = -1;
    g_game_state.god.duration = 0;

    // 初始化游戏信息
    g_game_state.game.now_player_id = 0;
    g_game_state.game.next_player_id = 1;
    g_game_state.game.ended = false;
    g_game_state.game.winner_id = -1;
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
    
    printf("当前玩家: %d\n", g_game_state.game.now_player_id);
    printf("游戏结束: %s\n", g_game_state.game.ended ? "是" : "否");
}

GameState* get_game_state(void) {
    return &g_game_state;
}