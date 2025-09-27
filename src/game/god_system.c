#include "god_system.h"
#include "game_state.h"
#include "map.h"
#include <stdlib.h>
#include <stdio.h>

extern GameState g_game_state;

// 检查位置是否可放置财神
static bool is_valid_god_spawn_location(int location) {
    // 不能是礼品屋(G)或道具屋(T)
    char symbol = get_map_symbol(location);
    if (symbol == 'G' || symbol == 'T') {
        return false;
    }
    // 不能有玩家
    for (int i = 0; i < g_game_state.player_count; i++) {
        if (g_game_state.players[i].location == location) {
            return false;
        }
    }
    // 不能有其他道具
    if (g_game_state.placed_prop.bomb[location] || g_game_state.placed_prop.barrier[location]) {
        return false;
    }
    return true;
}

void update_god_status() {
    if (g_game_state.god.location != -1) { // 财神已出现
        g_game_state.god.duration--;
        if (g_game_state.god.duration <= 0) {
            printf("财神在位置 %d 停留时间结束，消失了。\n", g_game_state.god.location);
            g_game_state.god.location = -1;
            g_game_state.god.spawn_cooldown = 10; // 重置冷却
        }
    } else { // 财神未出现
        g_game_state.god.spawn_cooldown--;
        if (g_game_state.god.spawn_cooldown <= 0) {
            // 尝试生成财神
            int attempts = 100; // 避免死循环
            while (attempts-- > 0) {
                int new_location = rand() % MAP_SIZE;
                if (is_valid_god_spawn_location(new_location)) {
                    g_game_state.god.location = new_location;
                    g_game_state.god.duration = 5;
                    printf("财神出现在地图位置 %d！\n", new_location);
                    break;
                }
            }
        }
    }
}

// 检查指定位置是否有财神
bool check_god_encounter(int location) {
    return g_game_state.god.location != -1 && g_game_state.god.location == location;
}

// 触发财神效果
void trigger_god_encounter(Player* player, int location) {
    printf("玩家 %s 在位置 %d 遇到了财神！获得财神附身效果。\n", player->name, location);
    player->buff.god = 5; // 获得5回合财神附身
    g_game_state.god.location = -1; // 财神被领取后消失
    g_game_state.god.spawn_cooldown = 10; // 重置冷却
}
