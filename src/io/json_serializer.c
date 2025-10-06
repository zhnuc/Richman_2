#include "json_serializer.h"
#include "../game/game_state.h"
#include "../game/player.h"
#include "colors.h"
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

    bool first_player = true;
    for (int i = 0; i < g_game_state.player_count; i++) {
        Player* p = &g_game_state.players[i];
        if (p->index < 0) continue;

        if (!first_player) {
            fprintf(file, ",\n");
        }

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
        fprintf(file, "        }");
        first_player = false;
    }
    fprintf(file, "\n    ],\n");
    fprintf(file, "    \"houses\": {\n");
    bool first_house = true;
    for (int i = 0; i < MAP_SIZE; i++) {
        if (g_game_state.houses[i].owner_id != -1) {
            if (!first_house) {
                fprintf(file, ",\n");
            }
            fprintf(file, "        \"%d\": {\n", i);
            fprintf(file, "            \"owner\": %d,\n", g_game_state.houses[i].owner_id);
            fprintf(file, "            \"level\": %d\n", g_game_state.houses[i].level);
            fprintf(file, "        }");
            first_house = false;
        }
    }
    fprintf(file, "\n    },\n");

    fprintf(file, "    \"god\": {\n");
    fprintf(file, "        \"spawn_cooldown\": %d,\n", g_game_state.god.spawn_cooldown);
    fprintf(file, "        \"location\": %d,\n", g_game_state.god.location);
    fprintf(file, "        \"duration\": %d\n", g_game_state.god.duration);
    fprintf(file, "    },\n");

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
    fprintf(file, "        \"started\": %s,\n", g_game_state.game.started ? "true" : "false");
    fprintf(file, "        \"ended\": %s,\n", g_game_state.game.ended ? "true" : "false");
    fprintf(file, "        \"winner\": %d\n", g_game_state.game.winner_id);
    fprintf(file, "    }\n");
    fprintf(file, "}\n");

    fclose(file);
}

// 辅助函数：从JSON字符串中提取字符串值
char* extract_string_value(const char* json, const char* key, const char* end_pos) {
    char search_key[100];
    sprintf(search_key, "\"%s\": \"", key);

    char* key_pos = strstr(json, search_key);
    if (!key_pos || (end_pos && key_pos > end_pos)) {
        return NULL;
    }

    char* value_start = key_pos + strlen(search_key);
    char* value_end = strchr(value_start, '"');
    if (!value_end) {
        return NULL;
    }

    int len = value_end - value_start;
    char* value = (char*)malloc(len + 1);
    if (!value) return NULL;
    strncpy(value, value_start, len);
    value[len] = '\0';
    return value;
}

// 辅助函数：从JSON字符串中提取整数值
int extract_int_value(const char* json, const char* key, const char* end_pos) {
    char search_key[100];
    sprintf(search_key, "\"%s\": ", key);

    char* key_pos = strstr(json, search_key);
    if (!key_pos || (end_pos && key_pos > end_pos)) {
        return -1; // Or some other error indicator
    }

    char* value_start = key_pos + strlen(search_key);
    return atoi(value_start);
}

// 辅助函数：提取布尔值
bool extract_bool_value(const char* json, const char* key, const char* end_pos) {
    char search_key[100];
    sprintf(search_key, "\"%s\": ", key);

    char* key_pos = strstr(json, search_key);
    if (!key_pos || (end_pos && key_pos > end_pos)) {
        return false;
    }

    char* value_start = key_pos + strlen(search_key);
    return strncmp(value_start, "true", 4) == 0;
}

// 辅助函数：找到匹配的大括号
char* find_matching_brace(const char* start) {
    if (!start || *start != '{') return NULL;
    int brace_level = 0;
    const char* p = start;
    do {
        if (*p == '{') {
            brace_level++;
        } else if (*p == '}') {
            brace_level--;
        }
        p++;
    } while (brace_level > 0 && *p != '\0');

    return (brace_level == 0) ? (char*)p - 1 : NULL;
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

// 解析god对象
void parse_and_load_god(const char* content) {
    // 查找顶级的 god 对象，而不是 buff 中的 god 字段
    // 我们需要查找 "god": { 的模式，而不仅仅是 "god":
    const char* search_pos = content;
    const char* god_start = NULL;
    
    // 循环查找所有的 "god": 直到找到后面跟着 { 的那个
    while ((search_pos = strstr(search_pos, "\"god\":")) != NULL) {
        const char* check_pos = search_pos + strlen("\"god\":");
        // 跳过空白字符
        while (*check_pos == ' ' || *check_pos == '\t' || *check_pos == '\n' || *check_pos == '\r') {
            check_pos++;
        }
        // 检查是否是对象开始
        if (*check_pos == '{') {
            god_start = search_pos;
            break;
        }
        search_pos++; // 继续查找下一个
    }
    
    if (!god_start) {
        return;
    }

    char* obj_start = strchr(god_start, '{');
    if (!obj_start) {
        return;
    }

    char* obj_end = find_matching_brace(obj_start);
    if (!obj_end) {
        return;
    }

    g_game_state.god.spawn_cooldown = extract_int_value(obj_start, "spawn_cooldown", obj_end);
    g_game_state.god.location = extract_int_value(obj_start, "location", obj_end);
    g_game_state.god.duration = extract_int_value(obj_start, "duration", obj_end);
}

// 解析players数组
void parse_and_load_players(const char* content) {
    char* players_start = strstr(content, "\"players\":");
    if (!players_start) return;

    char* arr_start = strchr(players_start, '[');
    if (!arr_start) return;

    char* arr_end = strchr(arr_start, ']'); // Simple assumption
    if (!arr_end) return;

    char* current = arr_start + 1;
    int i = 0;
    while (current < arr_end && i < MAX_PLAYERS) {
        char* player_start = strchr(current, '{');
        if (!player_start || player_start >= arr_end) break;

        char* player_end = find_matching_brace(player_start);
        if (!player_end || player_end >= arr_end) break;

        Player* p = &g_game_state.players[i];
        p->index = extract_int_value(player_start, "index", player_end);

        char* name = extract_string_value(player_start, "name", player_end);
        if (name) {
            strncpy(p->name, name, MAX_NAME_LENGTH - 1);
            p->name[MAX_NAME_LENGTH - 1] = '\0';
            free(name);
        }

        p->fund = extract_int_value(player_start, "fund", player_end);
        p->credit = extract_int_value(player_start, "credit", player_end);
        p->location = extract_int_value(player_start, "location", player_end);
        p->alive = extract_bool_value(player_start, "alive", player_end);

        parse_player_prop(player_start, player_end, &p->prop);
        parse_player_buff(player_start, player_end, &p->buff);

        if (strcmp(p->name, "钱夫人") == 0) p->color = COLOR_RED;
        else if (strcmp(p->name, "阿土伯") == 0) p->color = COLOR_GREEN;
        else if (strcmp(p->name, "孙小美") == 0) p->color = COLOR_BLUE;
        else if (strcmp(p->name, "金贝贝") == 0) p->color = COLOR_YELLOW;
        else p->color = COLOR_RESET;

        current = player_end + 1;
        i++;
    }
    g_game_state.player_count = i;
}

// 解析houses对象
void parse_and_load_houses(const char* content) {
    char* houses_start = strstr(content, "\"houses\":");
    if (!houses_start) return;

    char* obj_start = strchr(houses_start, '{');
    if (!obj_start) return;

    char* obj_end = find_matching_brace(obj_start);
    if (!obj_end) return;

    for (int i = 0; i < MAP_SIZE; i++) {
        g_game_state.houses[i].owner_id = -1;
        g_game_state.houses[i].level = 0;
    }

    char* current = obj_start + 1;
    while (current < obj_end) {
        char* key_start = strchr(current, '"');
        if (!key_start || key_start > obj_end) break;
        char* key_end = strchr(key_start + 1, '"');
        if (!key_end || key_end > obj_end) break;

        int loc_len = key_end - (key_start + 1);
        char loc_str[10] = {0};
        strncpy(loc_str, key_start + 1, loc_len);
        int loc = atoi(loc_str);

        char* house_obj_start = strchr(key_end, '{');
        if (!house_obj_start || house_obj_start > obj_end) break;
        char* house_obj_end = find_matching_brace(house_obj_start);
        if (!house_obj_end || house_obj_end > obj_end) break;

        if (loc >= 0 && loc < MAP_SIZE) {
            g_game_state.houses[loc].level = extract_int_value(house_obj_start, "level", house_obj_end);

            // 尝试作为整数解析owner（玩家索引）
            int owner_id = extract_int_value(house_obj_start, "owner", house_obj_end);
            if (owner_id >= 0 && owner_id < g_game_state.player_count) {
                g_game_state.houses[loc].owner_id = owner_id;
            } else {
                // 如果不是整数，尝试作为字符串解析（玩家名称）
                char* owner_name = extract_string_value(house_obj_start, "owner", house_obj_end);
                if (owner_name) {
                    for (int i = 0; i < g_game_state.player_count; i++) {
                        if (strcmp(g_game_state.players[i].name, owner_name) == 0) {
                            g_game_state.houses[loc].owner_id = g_game_state.players[i].index;
                            break;
                        }
                    }
                    free(owner_name);
                }
            }
        }
        current = house_obj_end + 1;
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

    memset(g_game_state.placed_prop.bomb, 0, sizeof(g_game_state.placed_prop.bomb));
    memset(g_game_state.placed_prop.barrier, 0, sizeof(g_game_state.placed_prop.barrier));

    char* bomb_start = strstr(obj_start, "\"bomb\":");
    if (bomb_start && bomb_start < obj_end) {
        char* arr_start = strchr(bomb_start, '[');
        char* arr_end = strchr(bomb_start, ']');
        if (arr_start && arr_end) {
            char* p = arr_start + 1;
            while (p < arr_end) {
                int loc = atoi(p);
                if (loc >= 0 && loc < MAP_SIZE) g_game_state.placed_prop.bomb[loc] = 1;
                while (*p && *p != ',' && p < arr_end) p++;
                if (*p == ',') p++;
            }
        }
    }

    char* barrier_start = strstr(obj_start, "\"barrier\":");
    if (barrier_start && barrier_start < obj_end) {
        char* arr_start = strchr(barrier_start, '[');
        char* arr_end = strchr(barrier_start, ']');
        if (arr_start && arr_end) {
            char* p = arr_start + 1;
            while (p < arr_end) {
                int loc = atoi(p);
                if (loc >= 0 && loc < MAP_SIZE) g_game_state.placed_prop.barrier[loc] = 1;
                while (*p && *p != ',' && p < arr_end) p++;
                if (*p == ',') p++;
            }
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

    g_game_state.game.now_player_id = extract_int_value(obj_start, "now_player", obj_end);
    g_game_state.game.next_player_id = extract_int_value(obj_start, "next_player", obj_end);
    g_game_state.game.started = extract_bool_value(obj_start, "started", obj_end);
    g_game_state.game.ended = extract_bool_value(obj_start, "ended", obj_end);
    g_game_state.game.winner_id = extract_int_value(obj_start, "winner", obj_end);
}

int load_game_preset(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        return -1;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* content = (char*)malloc(length + 1);
    if (!content) { fclose(file); return -1; }
    fread(content, 1, length, file);
    content[length] = '\0';

    fclose(file);

    parse_and_load_players(content);
    parse_and_load_houses(content);
    parse_and_load_god(content);
    parse_and_load_placed_prop(content);
    parse_and_load_game_info(content);

    free(content);

    if (g_game_state.player_count > 0) {
        g_game_state.game.started = true;
    }

    return 0;
}