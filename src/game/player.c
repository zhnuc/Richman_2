#include "player.h"
#include "character.h"
#include "../io/colors.h" // 包含颜色定义
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 外部游戏状态声明
extern GameState g_game_state;

Player* create_player_by_character(int character_id, int fund) {
    if (!is_valid_character_id(character_id) || g_game_state.player_count >= MAX_PLAYERS) {
        return NULL;
    }
    
    Character* character = get_character_by_id(character_id);
    if (!character) {
        return NULL;
    }
    
    Player* player = &g_game_state.players[g_game_state.player_count];
    player->index = g_game_state.player_count;  // 从0开始
    strncpy(player->name, character->name, sizeof(player->name) - 1);
    player->name[sizeof(player->name) - 1] = '\0';
    player->fund = fund;
    player->credit = 0;
    player->location = 0;
    player->alive = true;
    
    // 根据角色ID分配颜色
    switch (character_id) {
        case 1: player->color = COLOR_RED; break;    // 钱夫人
        case 2: player->color = COLOR_GREEN; break;  // 阿土伯
        case 3: player->color = COLOR_BLUE; break;   // 孙小美
        case 4: player->color = COLOR_YELLOW; break; // 金贝贝
        default: player->color = COLOR_RESET; break;
    }
    
    // 初始化道具
    player->prop.bomb = 0;
    player->prop.barrier = 0;
    player->prop.robot = 0;
    player->prop.total = 0;
    
    // 初始化Buff
    player->buff.god = 0;
    player->buff.prison = 0;
    player->buff.hospital = 0;
    
    g_game_state.player_count++;
    return player;
}

Player* create_player(int index, const char* name, int fund) {
    if (index < 0 || index >= MAX_PLAYERS || g_game_state.player_count >= MAX_PLAYERS) {
        return NULL;
    }
    
    Player* player = &g_game_state.players[g_game_state.player_count];
    player->index = index;
    strncpy(player->name, name, sizeof(player->name) - 1);
    player->name[sizeof(player->name) - 1] = '\0';
    player->fund = fund;
    player->credit = 0;
    player->location = 0;
    player->alive = true;
    
    // 初始化道具
    player->prop.bomb = 0;
    player->prop.barrier = 0;
    player->prop.robot = 0;
    player->prop.total = 0;
    
    // 初始化Buff
    player->buff.god = 0;
    player->buff.prison = 0;
    player->buff.hospital = 0;
    
    g_game_state.player_count++;
    return player;
}

void free_player(Player* player) {
    // 目前玩家存储在全局状态中，不需要单独释放
    // 如果将来改为动态分配，可以在这里释放
    (void)player; // 避免未使用参数警告
}

bool is_player_valid(const Player* player) {
    return player != NULL && player->alive;
}

void print_player_info(const Player* player) {
    if (!player) return;
    
    printf("玩家%d: %s, 资金:%d, 位置:%d, 存活:%s\n", 
           player->index, player->name, player->fund, player->location, 
           player->alive ? "是" : "否");
}