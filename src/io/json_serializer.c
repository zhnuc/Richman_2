#include "json_serializer.h"
#include "../game/game_state.h"
#include "../game/player.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void save_game_dump(const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("错误: 无法创建文件 %s\n", filename);
        return;
    }
    
    fprintf(file, "{\n");
    fprintf(file, "    \"players\": [\n");
    
    for (int i = 0; i < g_game_state.player_count; i++) {
        Player* p = &g_game_state.players[i];
        fprintf(file, "        {\n");
        fprintf(file, "            \"index\": %d,\n", p->index);
        fprintf(file, "            \"name\": \"%s\",\n", p->name);
        fprintf(file, "            \"fund\": %d,\n", p->fund);
        fprintf(file, "            \"credit\": %d,\n", p->credit);
        fprintf(file, "            \"location\": %d,\n", p->location);
        fprintf(file, "            \"alive\": %s,\n", p->alive ? "true" : "false");
        fprintf(file, "            \"prop\": {\n");
        fprintf(file, "                \"bomb\": %d,\n", p->prop.bomb);
        fprintf(file, "                \"barrier\": %d,\n", p->prop.barrier);
        fprintf(file, "                \"robot\": %d,\n", p->prop.robot);
        fprintf(file, "                \"total\": %d\n", p->prop.total);
        fprintf(file, "            },\n");
        fprintf(file, "            \"buff\": {\n");
        fprintf(file, "                \"god\": %d,\n", p->buff.god);
        fprintf(file, "                \"prison\": %d,\n", p->buff.prison);
        fprintf(file, "                \"hospital\": %d\n", p->buff.hospital);
        fprintf(file, "            }\n");
        fprintf(file, "        }%s\n", i < g_game_state.player_count - 1 ? "," : "");
    }
    
    fprintf(file, "    ],\n");
    fprintf(file, "    \"houses\": {\n");
    bool first_house = true;
    for (int i = 0; i < MAP_SIZE; i++) {
        if (g_game_state.houses[i].owner_id != -1) {
            if (!first_house) {
                fprintf(file, ",\n");
            }
            fprintf(file, "        \"%d\": {\n", i);
            fprintf(file, "            \"owner\": \"%s\",\n", g_game_state.players[g_game_state.houses[i].owner_id].name);
            fprintf(file, "            \"level\": %d\n", g_game_state.houses[i].level);
            fprintf(file, "        }");
            first_house = false;
        }
    }
    fprintf(file, "\n    },\n");
    
    fprintf(file, "    \"placed_prop\": {\n");
    fprintf(file, "        \"bomb\": [");
    bool first_bomb = true;
    for (int i = 0; i < MAP_SIZE; i++) {
        if (g_game_state.placed_prop.bomb[i]) {
            if (!first_bomb) {
                fprintf(file, ", ");
            }
            fprintf(file, "%d", i);
            first_bomb = false;
        }
    }
    fprintf(file, "],\n");
    fprintf(file, "        \"barrier\": [");
    bool first_barrier = true;
    for (int i = 0; i < MAP_SIZE; i++) {
        if (g_game_state.placed_prop.barrier[i]) {
            if (!first_barrier) {
                fprintf(file, ", ");
            }
            fprintf(file, "%d", i);
            first_barrier = false;
        }
    }
    fprintf(file, "]\n");
    fprintf(file, "    },\n");
    
    fprintf(file, "    \"game\": {\n");
    fprintf(file, "        \"now_player\": %d,\n", g_game_state.game.now_player_id);
    fprintf(file, "        \"next_player\": %d,\n", g_game_state.game.next_player_id);
    fprintf(file, "        \"ended\": %s,\n", g_game_state.game.ended ? "true" : "false");
    fprintf(file, "        \"winner\": %d\n", g_game_state.game.winner_id);
    fprintf(file, "    }\n");
    fprintf(file, "}\n");
    
    fclose(file);
}

// ... (rest of the file remains the same for now, will be updated in subsequent steps)
// ... existing code ...
// 辅助函数：从JSON字符串中提取字符串值
char* extract_string_value(const char* json, const char* key, const char* end_pos) {
// ... existing code ...
}

// 辅助函数：从JSON字符串中提取整数值
int extract_int_value(const char* json, const char* key, const char* end_pos) {
// ... existing code ...
}

// 辅助函数：提取布尔值
bool extract_bool_value(const char* json, const char* key, const char* end_pos) {
// ... existing code ...
}

// 辅助函数：找到匹配的大括号
char* find_matching_brace(const char* start) {
// ... existing code ...
}

// 辅助函数：解析player的prop对象
void parse_player_prop(const char* player_obj, const char* player_end, Prop* prop) {
    char* prop_start = strstr(player_obj, "\"prop\":");
    if (!prop_start || prop_start > player_end) return;
    
    char* prop_obj_start = strchr(prop_start, '{');
    if (!prop_obj_start) return;
    
    char* prop_obj_end = find_matching_brace(prop_obj_start);
    if (!prop_obj_end || prop_obj_end > player_end) return;
    
    prop->bomb = extract_int_value(prop_obj_start, "bomb", prop_obj_end);
    prop->barrier = extract_int_value(prop_obj_start, "barrier", prop_obj_end);
    prop->robot = extract_int_value(prop_obj_start, "robot", prop_obj_end);
    prop->total = extract_int_value(prop_obj_start, "total", prop_obj_end);
    // 本回合购买标记在JSON中不保存，加载时重置为0
    prop->barrier_bought_this_turn = 0;
    prop->robot_bought_this_turn = 0;
    prop->bomb_bought_this_turn = 0;
}

// 辅助函数：解析player的buff对象
void parse_player_buff(const char* player_obj, const char* player_end, Buff* buff) {
    char* buff_start = strstr(player_obj, "\"buff\":");
    if (!buff_start || buff_start > player_end) return;
    
    char* buff_obj_start = strchr(buff_start, '{');
    if (!buff_obj_start) return;
    
    char* buff_obj_end = find_matching_brace(buff_obj_start);
    if (!buff_obj_end || buff_obj_end > player_end) return;
    
    buff->god = extract_int_value(buff_obj_start, "god", buff_obj_end);
    buff->prison = extract_int_value(buff_obj_start, "prison", buff_obj_end);
    buff->hospital = extract_int_value(buff_obj_start, "hospital", buff_obj_end);
}

// 解析players数组
void parse_and_load_players(const char* content) {
// ... existing code ...
}

// 解析houses对象
void parse_and_load_houses(const char* content) {
    char* houses_start = strstr(content, "\"houses\":");
    if (!houses_start) return;
    
    char* obj_start = strchr(houses_start, '{');
    if (!obj_start) return;
    
    char* obj_end = find_matching_brace(obj_start);
    if (!obj_end) return;
    
    // 初始化所有房产为空
    for (int i = 0; i < MAP_SIZE; i++) {
        g_game_state.houses[i].owner_id = -1;
        g_game_state.houses[i].level = 0;
    }
    
    // 解析每个房产条目
    char* current = obj_start + 1; // 跳过开始的{
    while (current < obj_end) {
        // ... (logic to parse houses, needs to be updated)
        current++; // Placeholder
    }
}

// 解析placed_prop对象
void parse_and_load_placed_prop(const char* content) {
    char* placed_prop_start = strstr(content, "\"placed_prop\":");
    if (!placed_prop_start) return;
    
    char* obj_start = strchr(placed_prop_start, '{');
    if (!obj_start) return;
    
    char* obj_end = find_matching_brace(obj_start);
    if (!obj_end) return;
    
    // 初始化
    memset(g_game_state.placed_prop.bomb, 0, sizeof(g_game_state.placed_prop.bomb));
    memset(g_game_state.placed_prop.barrier, 0, sizeof(g_game_state.placed_prop.barrier));
    
    // ... (logic to parse placed props, needs to be updated)
}

// 解析game信息
void parse_and_load_game_info(const char* content) {
    char* game_start = strstr(content, "\"game\":");
    if (!game_start) return;
    
    char* obj_start = strchr(game_start, '{');
    if (!obj_start) return;
    
    char* obj_end = find_matching_brace(obj_start);
    if (!obj_end) return;
    
    g_game_state.game.now_player_id = extract_int_value(obj_start, "now_player", obj_end);
    g_game_state.game.next_player_id = extract_int_value(obj_start, "next_player", obj_end);
    g_game_state.game.ended = extract_bool_value(obj_start, "ended", obj_end);
    g_game_state.game.winner_id = extract_int_value(obj_start, "winner", obj_end);
}

int load_game_preset(const char* filename) {
// ... existing code ...
}