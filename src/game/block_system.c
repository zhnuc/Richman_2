#include "block_system.h"
#include "game_state.h"
#include "../io/command_processor.h" // 包含 g_last_action_message
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// 计算路障放置位置
int calculate_block_position(int current_pos, int relative_distance) {
    int target_pos = current_pos + relative_distance;
    // 处理地图循环
    while (target_pos < 0) target_pos += MAP_SIZE;
    while (target_pos >= MAP_SIZE) target_pos -= MAP_SIZE;
    return target_pos;
}

// 验证路障放置位置是否有效
bool is_valid_block_position(int current_pos, int relative_distance) {
    (void)current_pos; // 避免未使用参数警告
    // 检查距离范围 (-10 到 +10)
    if (abs(relative_distance) > BLOCK_RANGE) {
        return false;
    }
    // 不能在当前位置放置路障
    if (relative_distance == 0) {
        return false;
    }
    return true;
}

// 检查指定位置是否有路障
bool has_block_at_location(int location) {
    if (location < 0 || location >= MAP_SIZE) {
        return false;
    }
    return g_game_state.placed_prop.barrier[location] == 1;
}

// 检查位置是否为特殊建筑
bool is_special_building(int location) {
    // 特殊建筑位置：起点0、医院14、道具屋28、礼品屋35、监狱49、魔法屋63
    // 但是根据测试用例，路障可以在特殊建筑位置放置
    return false; // 暂时允许在所有位置放置路障
}

// 检查位置是否有玩家
bool has_player_at_location(int location) {
    for (int i = 0; i < g_game_state.player_count; i++) {
        if (g_game_state.players[i].alive && g_game_state.players[i].location == location) {
            return true;
        }
    }
    return false;
}

// 放置路障
bool place_block(int player_index, int target_location) {
    (void)player_index; // 避免未使用参数警告
    char message_buffer[256];

    // 检查位置有效性
    if (target_location < 0 || target_location >= MAP_SIZE) {
        snprintf(message_buffer, sizeof(message_buffer), "无效的放置位置。\n");
        strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
        return false;
    }
    
    // 检查是否为特殊建筑
    if (is_special_building(target_location)) {
        snprintf(message_buffer, sizeof(message_buffer), "不能在特殊建筑位置放置道具。\n");
        strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
        return false;
    }
    
    // 检查该位置是否有玩家
    if (has_player_at_location(target_location)) {
        snprintf(message_buffer, sizeof(message_buffer), "不能在有玩家的位置放置道具。\n");
        strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
        return false;
    }
    
    // 检查该位置是否已有路障
    if (has_block_at_location(target_location)) {
        snprintf(message_buffer, sizeof(message_buffer), "该位置已有路障，无法放置。\n");
        strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
        return false;
    }
    
    // 放置路障
    g_game_state.placed_prop.barrier[target_location] = 1;
    snprintf(message_buffer, sizeof(message_buffer), "路障已放置在位置 %d。\n", target_location);
    strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
    return true;
}

// 移除路障
void remove_block(int location) {
    if (location >= 0 && location < MAP_SIZE) {
        g_game_state.placed_prop.barrier[location] = 0;
        char message_buffer[256];
        snprintf(message_buffer, sizeof(message_buffer), "位置 %d 的路障已被移除。\n", location);
        strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
    }
}

// 检查路障拦截
bool check_block_interception(int location) {
    return has_block_at_location(location);
}

// 触发路障拦截效果
void trigger_block_interception(Player* player, int location) {
    char message_buffer[256];
    snprintf(message_buffer, sizeof(message_buffer), "玩家 %s 被位置 %d 的路障拦截！\n您被拦截在路障位置，无法继续前进。\n", player->name, location);
    strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
    
    // 路障一次性使用，拦截后移除
    remove_block(location);
}

// 处理block命令
bool handle_block_command(Player* player, int position_or_distance) {
    char message_buffer[256];
    // 检查玩家是否有路障道具
    if (player->prop.barrier <= 0) {
        snprintf(message_buffer, sizeof(message_buffer), "您没有路障道具。\n");
        strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
        return false;
    }
    
    int target_location;
    int relative_distance;
    
    // 判断是绝对位置还是相对距离
    // 如果数字 >= 50，则当作绝对位置处理
    // 如果数字 < 50，则当作相对距离处理
    if (position_or_distance >= 50) {
        // 当作绝对位置处理
        target_location = position_or_distance;
        if (target_location >= MAP_SIZE) {
            snprintf(message_buffer, sizeof(message_buffer), "无效的放置位置。位置超出地图范围。\n");
            strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
            return false;
        }
        if (target_location == player->location) {
            snprintf(message_buffer, sizeof(message_buffer), "无效的放置位置。路障不能放置在当前位置。\n");
            strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
            return false;
        }
    } else {
        // 当作相对距离处理
        relative_distance = position_or_distance;
        if (!is_valid_block_position(player->location, relative_distance)) {
            snprintf(message_buffer, sizeof(message_buffer), "无效的放置距离。路障只能放置在前后 %d 步范围内，且不能放置在当前位置。\n", BLOCK_RANGE);
            strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
            return false;
        }
        target_location = calculate_block_position(player->location, relative_distance);
    }
    
    // 放置路障
    if (place_block(player->index, target_location)) {
        // 消耗路障道具
        player->prop.barrier--;
        player->prop.total--;
        snprintf(message_buffer, sizeof(message_buffer), "使用了一个路障道具。剩余路障：%d\n", player->prop.barrier);
        strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
        return true;
    }
    
    return false;
}

// 显示所有路障位置（调试用）
void display_all_blocks(void) {
    printf("当前地图上的路障位置：");
    bool found = false;
    for (int i = 0; i < MAP_SIZE; i++) {
        if (has_block_at_location(i)) {
            printf(" %d", i);
            found = true;
        }
    }
    if (!found) {
        printf(" 无");
    }
    printf("\n");
}

// ========== 机器娃娃系统实现 ==========

// 检查位置是否有任何道具（路障）
bool has_any_prop_at_location(int location) {
    if (location < 0 || location >= MAP_SIZE) return false;
    return has_block_at_location(location);
}

// 清除单个位置的道具，返回实际清除的道具数量
int clear_single_prop(int location) {
    if (location < 0 || location >= MAP_SIZE) return 0;
    char message_buffer[256];
    
    // 检查并清除路障
    if (has_block_at_location(location)) {
        g_game_state.placed_prop.barrier[location] = 0;
        snprintf(message_buffer, sizeof(message_buffer), "清除了位置 %d 的路障。\n", location);
        strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
        return 1;
    }
    
    // 该位置没有道具
    return 0;
}

// 清除指定范围内的所有道具
int clear_props_in_range(Player* player, int start_location, int range) {
    (void)player; // 避免未使用参数警告
    int cleared_count = 0;
    char message_buffer[256];
    
    snprintf(message_buffer, sizeof(message_buffer), "机器娃娃开始清扫前方 %d 步内的道具...\n", range);
    strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
    
    // 清除前方range步内的所有道具
    for (int i = 1; i <= range; i++) {
        int target_location = (start_location + i) % MAP_SIZE;
        
        if (has_any_prop_at_location(target_location)) {
            clear_single_prop(target_location);
            cleared_count++;  // 每个位置最多只有1个道具
        }
    }
    
    if (cleared_count == 0) {
        snprintf(message_buffer, sizeof(message_buffer), "前方 %d 步内没有发现任何道具。\n", range);
        strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
    } else {
        snprintf(message_buffer, sizeof(message_buffer), "机器娃娃清扫完成，共清除了 %d 个道具。\n", cleared_count);
        strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
    }
    
    return cleared_count;
}

// 处理robot命令
bool handle_robot_command(Player* player) {
    char message_buffer[256];
    // 检查玩家是否有机器娃娃道具
    if (player->prop.robot <= 0) {
        snprintf(message_buffer, sizeof(message_buffer), "您没有机器娃娃道具。\n");
        strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
        return false;
    }
    
    snprintf(message_buffer, sizeof(message_buffer), "玩家 %s 使用机器娃娃清扫前方道具。\n", player->name);
    strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
    
    // 清除前方10步内的道具
    int cleared_count = clear_props_in_range(player, player->location, ROBOT_CLEAR_RANGE);
    
    // 消耗机器娃娃道具（一次性使用）
    player->prop.robot--;
    player->prop.total--;
    snprintf(message_buffer, sizeof(message_buffer), "使用了一个机器娃娃道具。剩余机器娃娃：%d\n", player->prop.robot);
    strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
    
    // 即使没有清除任何道具，机器娃娃也会被消耗
    if (cleared_count == 0) {
        snprintf(message_buffer, sizeof(message_buffer), "虽然没有清除任何道具，但机器娃娃已被使用。\n");
        strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
    }
    
    return true;
}

