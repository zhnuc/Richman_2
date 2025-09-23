#include "json_serializer.h"
#include "../game/game_state.h"
#include <stdio.h>
#include <string.h>

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
    
    // 输出房产信息
    bool has_houses = false;
    for (int i = 0; i < 40; i++) {
        if (strlen(g_game_state.houses[i].owner) > 0) {
            if (has_houses) fprintf(file, ",\n");
            fprintf(file, "        \"%d\": {\n", i);
            fprintf(file, "            \"owner\": \"%s\",\n", g_game_state.houses[i].owner);
            fprintf(file, "            \"level\": %d\n", g_game_state.houses[i].level);
            fprintf(file, "        }");
            has_houses = true;
        }
    }
    fprintf(file, "    },\n");
    
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

int load_game_preset(const char* filename) {
    // 简单的JSON加载实现（暂时跳过，后续可以完善）
    // 目前只初始化空状态
    init_game_state();
    (void)filename; // 避免未使用参数警告
    return 0;
}