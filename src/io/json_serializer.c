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
    // 检查是否有房产
    bool has_houses = false;
    for (int i = 0; i < 40; i++) {
        if (strlen(g_game_state.houses[i].owner) > 0) {
            has_houses = true;
            break;
        }
    }
    
    if (has_houses) {
        fprintf(file, "    \"houses\": {\n");
        bool first_house = true;
        for (int i = 0; i < 40; i++) {
            if (strlen(g_game_state.houses[i].owner) > 0) {
                if (!first_house) fprintf(file, ",\n");
                fprintf(file, "        \"%d\": {\n", i);
                fprintf(file, "            \"owner\": \"%s\",\n", g_game_state.houses[i].owner);
                fprintf(file, "            \"level\": %d\n", g_game_state.houses[i].level);
                fprintf(file, "        }");
                first_house = false;
            }
        }
        fprintf(file, "\n    },\n");
    } else {
        fprintf(file, "    \"houses\": {},\n");
    }
    
    fprintf(file, "    \"placed_prop\": {\n");
    fprintf(file, "        \"bomb\": [");
    for (int i = 0; i < g_game_state.placed_prop.bomb_count; i++) {
        fprintf(file, "%d%s", g_game_state.placed_prop.bomb_positions[i], 
                i < g_game_state.placed_prop.bomb_count - 1 ? ", " : "");
    }
    fprintf(file, "],\n");
    fprintf(file, "        \"barrier\": [");
    for (int i = 0; i < g_game_state.placed_prop.barrier_count; i++) {
        fprintf(file, "%d%s", g_game_state.placed_prop.barrier_positions[i], 
                i < g_game_state.placed_prop.barrier_count - 1 ? ", " : "");
    }
    fprintf(file, "]\n");
    fprintf(file, "    },\n");
    
    fprintf(file, "    \"game\": {\n");
    fprintf(file, "        \"now_player\": %d,\n", g_game_state.game.now_player);
    fprintf(file, "        \"next_player\": %d,\n", g_game_state.game.next_player);
    fprintf(file, "        \"started\": %s,\n", g_game_state.game.started ? "true" : "false");
    fprintf(file, "        \"ended\": %s,\n", g_game_state.game.ended ? "true" : "false");
    fprintf(file, "        \"winner\": %d\n", g_game_state.game.winner);
    fprintf(file, "    }\n");
    fprintf(file, "}\n");
    
    fclose(file);
}

// 辅助函数：从JSON字符串中提取字符串值
char* extract_string_value(const char* json, const char* key, const char* end_pos) {
    char search_key[64];
    snprintf(search_key, sizeof(search_key), "\"%s\":", key);
    
    char* key_pos = strstr(json, search_key);
    if (!key_pos || key_pos > end_pos) return NULL;
    
    char* value_start = strchr(key_pos + strlen(search_key), '"');
    if (!value_start) return NULL;
    value_start++; // 跳过开始的引号
    
    char* value_end = strchr(value_start, '"');
    if (!value_end || value_end > end_pos) return NULL;
    
    int len = value_end - value_start;
    char* result = malloc(len + 1);
    strncpy(result, value_start, len);
    result[len] = '\0';
    return result;
}

// 辅助函数：从JSON字符串中提取整数值
int extract_int_value(const char* json, const char* key, const char* end_pos) {
    char search_key[64];
    snprintf(search_key, sizeof(search_key), "\"%s\":", key);
    
    char* key_pos = strstr(json, search_key);
    if (!key_pos || key_pos > end_pos) return 0;
    
    char* value_start = key_pos + strlen(search_key);
    while (*value_start == ' ' || *value_start == '\t' || *value_start == '\n') {
        value_start++;
    }
    
    return atoi(value_start);
}

// 辅助函数：提取布尔值
bool extract_bool_value(const char* json, const char* key, const char* end_pos) {
    char search_key[64];
    snprintf(search_key, sizeof(search_key), "\"%s\":", key);
    
    char* key_pos = strstr(json, search_key);
    if (!key_pos || key_pos > end_pos) return false;
    
    char* value_start = key_pos + strlen(search_key);
    while (*value_start == ' ' || *value_start == '\t' || *value_start == '\n') {
        value_start++;
    }
    
    return (strncmp(value_start, "true", 4) == 0);
}

// 辅助函数：找到匹配的大括号
char* find_matching_brace(const char* start) {
    const char* current = start + 1; // 跳过开始的{
    int brace_count = 1;
    
    while (*current && brace_count > 0) {
        if (*current == '{') brace_count++;
        else if (*current == '}') brace_count--;
        current++;
    }
    
    return brace_count == 0 ? (char*)current : NULL;
}

// 辅助函数：解析player的prop对象
void parse_player_prop(const char* player_obj, const char* player_end, PlayerProp* prop) {
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
}

// 辅助函数：解析player的buff对象
void parse_player_buff(const char* player_obj, const char* player_end, PlayerBuff* buff) {
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
    char* players_start = strstr(content, "\"players\": [");
    if (!players_start) return;
    
    char* array_start = strchr(players_start, '[') + 1;
    char* array_end = strchr(array_start, ']');
    if (!array_end) return;
    
    int player_index = 0;
    char* current = array_start;
    
    while (current < array_end && (current = strstr(current, "{")) != NULL && current < array_end) {
        char* obj_end = find_matching_brace(current);
        if (!obj_end || obj_end > array_end) break;
        
        // 直接设置player结构体
        Player* player = &g_game_state.players[player_index];
        player->index = extract_int_value(current, "index", obj_end);
        
        char* name = extract_string_value(current, "name", obj_end);
        if (name) {
            strncpy(player->name, name, sizeof(player->name) - 1);
            player->name[sizeof(player->name) - 1] = '\0';
            free(name);
        }
        
        player->fund = extract_int_value(current, "fund", obj_end);
        player->credit = extract_int_value(current, "credit", obj_end);
        player->location = extract_int_value(current, "location", obj_end);
        player->alive = extract_bool_value(current, "alive", obj_end);
        
        // 解析prop和buff子对象
        parse_player_prop(current, obj_end, &player->prop);
        parse_player_buff(current, obj_end, &player->buff);
        
        player_index++;
        current = obj_end;
    }
    
    g_game_state.player_count = player_index;
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
    for (int i = 0; i < 40; i++) {
        strcpy(g_game_state.houses[i].owner, "");
        g_game_state.houses[i].level = 0;
    }
    
    // 解析每个房产条目
    char* current = obj_start + 1; // 跳过开始的{
    while (current < obj_end) {
        // 跳过空白字符
        while (*current == ' ' || *current == '\t' || *current == '\n' || *current == ',') {
            current++;
        }
        
        if (*current == '}' || current >= obj_end) break;
        
        // 查找位置key（如"5"）
        if (*current == '"') {
            char* key_start = current + 1;
            char* key_end = strchr(key_start, '"');
            if (!key_end) break;
            
            int position = atoi(key_start);
            if (position < 0 || position >= 40) {
                current = key_end + 1;
                continue;
            }
            
            // 找到对应的value对象
            current = strchr(key_end, ':');
            if (!current) break;
            current++;
            
            // 跳过空白找到{
            while (*current == ' ' || *current == '\t' || *current == '\n') {
                current++;
            }
            
            if (*current == '{') {
                char* value_end = find_matching_brace(current);
                if (value_end) {
                    // 提取owner和level
                    char* owner = extract_string_value(current, "owner", value_end);
                    if (owner) {
                        strncpy(g_game_state.houses[position].owner, owner, sizeof(g_game_state.houses[position].owner) - 1);
                        g_game_state.houses[position].owner[sizeof(g_game_state.houses[position].owner) - 1] = '\0';
                        free(owner);
                    }
                    g_game_state.houses[position].level = extract_int_value(current, "level", value_end);
                    current = value_end;
                }
            }
        } else {
            current++;
        }
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
    g_game_state.placed_prop.bomb_count = 0;
    g_game_state.placed_prop.barrier_count = 0;
    
    // 解析bomb数组
    char* bomb_start = strstr(obj_start, "\"bomb\":");
    if (bomb_start && bomb_start < obj_end) {
        char* bomb_array_start = strchr(bomb_start, '[');
        char* bomb_array_end = strchr(bomb_array_start, ']');
        if (bomb_array_start && bomb_array_end && bomb_array_end < obj_end) {
            char* current = bomb_array_start + 1;
            int bomb_count = 0;
            while (current < bomb_array_end && bomb_count < 20) {
                while (*current == ' ' || *current == '\t' || *current == '\n' || *current == ',') {
                    current++;
                }
                if (*current >= '0' && *current <= '9') {
                    g_game_state.placed_prop.bomb_positions[bomb_count++] = atoi(current);
                    while (*current >= '0' && *current <= '9') {
                        current++;
                    }
                } else {
                    current++;
                }
            }
            g_game_state.placed_prop.bomb_count = bomb_count;
        }
    }
    
    // 解析barrier数组
    char* barrier_start = strstr(obj_start, "\"barrier\":");
    if (barrier_start && barrier_start < obj_end) {
        char* barrier_array_start = strchr(barrier_start, '[');
        char* barrier_array_end = strchr(barrier_array_start, ']');
        if (barrier_array_start && barrier_array_end && barrier_array_end < obj_end) {
            char* current = barrier_array_start + 1;
            int barrier_count = 0;
            while (current < barrier_array_end && barrier_count < 20) {
                while (*current == ' ' || *current == '\t' || *current == '\n' || *current == ',') {
                    current++;
                }
                if (*current >= '0' && *current <= '9') {
                    g_game_state.placed_prop.barrier_positions[barrier_count++] = atoi(current);
                    while (*current >= '0' && *current <= '9') {
                        current++;
                    }
                } else {
                    current++;
                }
            }
            g_game_state.placed_prop.barrier_count = barrier_count;
        }
    }
}

// 解析game信息
void parse_and_load_game_info(const char* content) {
    char* game_start = strstr(content, "\"game\":");
    if (!game_start) return;
    
    char* obj_start = strchr(game_start, '{');
    if (!obj_start) return;
    
    char* obj_end = find_matching_brace(obj_start);
    if (!obj_end) return;
    
    g_game_state.game.now_player = extract_int_value(obj_start, "now_player", obj_end);
    g_game_state.game.next_player = extract_int_value(obj_start, "next_player", obj_end);
    g_game_state.game.started = extract_bool_value(obj_start, "started", obj_end);
    g_game_state.game.ended = extract_bool_value(obj_start, "ended", obj_end);
    g_game_state.game.winner = extract_int_value(obj_start, "winner", obj_end);
}

int load_game_preset(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        return -1; // 文件不存在或无法打开
    }
    
    // 获取文件大小并读取内容
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);
    
    char* content = malloc(size + 1);
    if (!content) {
        fclose(file);
        return -1;
    }
    
    fread(content, 1, size, file);
    content[size] = '\0';
    fclose(file);
    
    // 初始化游戏状态
    init_game_state();
    
    // 解析并加载所有游戏状态数据
    parse_and_load_players(content);      // 完整加载players数组
    parse_and_load_houses(content);       // 加载houses数据
    parse_and_load_placed_prop(content);  // 加载已放置道具
    parse_and_load_game_info(content);    // 加载游戏信息
    
    free(content);
    return 0;
}