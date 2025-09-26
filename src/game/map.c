#include "map.h"
#include "game_state.h"
#include "character.h"
#include "block_system.h"
#include "../io/colors.h"
#include <stdio.h>
#include <string.h>

void display_map() {
    char map[8][30]; // Increased size for null terminator
    // Initialize map with spaces
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 29; j++) {
            map[i][j] = ' ';
        }
        map[i][29] = '\0';
    }

    // Static map elements
    map[0][0] = 'S';
    for (int i = 1; i <= 13; i++) map[0][i] = '0';
    map[0][14] = 'H';
    for (int i = 15; i <= 27; i++) map[0][i] = '0';
    map[0][28] = 'T';

    for (int i = 1; i <= 6; i++) map[i][28] = '0';

    map[7][28] = 'G';
    for (int i = 27; i >= 15; i--) map[7][i] = '0';
    map[7][14] = 'P';
    for (int i = 13; i >= 1; i--) map[7][i] = '0';
    map[7][0] = 'M';

    for (int i = 1; i <= 6; i++) map[i][0] = '$';

    // Place players
    for (int i = 0; i < g_game_state.player_count; i++) {
        Player* p = &g_game_state.players[i];
        if (!p->alive) continue;

        int loc = p->location;
        char symbol = p->name[0];

        if (loc >= 0 && loc <= 28) { // Top row (S...H...T)
            map[0][loc] = symbol;
        } else if (loc >= 29 && loc <= 35) { // Right column (T...G)
            map[loc - 28][28] = symbol;
        } else if (loc >= 36 && loc <= 63) { // Bottom row (G...P...M)
            map[7][28 - (loc - 35)] = symbol;
        } else if (loc >= 64 && loc <= 69) { // Left column (M...S)
            map[7 - (loc - 63)][0] = symbol;
        }
    }

    // Print map with annotations
    for (int i = 0; i < 8; i++) {
        if (i == 0) {
            printf("            地段 1\n");
        }
        
        for (int j = 0; j < 29; j++) {
            Player* top_player = NULL;
            int last_moved_player_id = (g_game_state.game.now_player_id + g_game_state.player_count - 1) % g_game_state.player_count;

            for (int k = 0; k < g_game_state.player_count; k++) {
                Player* p = &g_game_state.players[k];
                if (p->alive) {
                    int loc = p->location;
                    int map_i = -1, map_j = -1;

                    if (loc >= 0 && loc <= 28) { map_i = 0; map_j = loc; } 
                    else if (loc >= 29 && loc <= 35) { map_i = loc - 28; map_j = 28; } 
                    else if (loc >= 36 && loc <= 63) { map_i = 7; map_j = 28 - (loc - 35); } 
                    else if (loc >= 64 && loc <= 69) { map_i = 7 - (loc - 63); map_j = 0; }

                    if (i == map_i && j == map_j) {
                        if (top_player == NULL || k == last_moved_player_id) {
                            top_player = p;
                        }
                    }
                }
            }

            if (top_player) {
                // If the top player is the last one who moved, display them.
                // Otherwise, we need to find the last moved player if they are on this spot.
                Player* player_to_show = NULL;
                bool last_moved_player_on_spot = false;
                (void)last_moved_player_on_spot; // 避免未使用变量警告
                 for (int k = 0; k < g_game_state.player_count; k++) {
                    Player* p = &g_game_state.players[k];
                     if (p->alive) {
                        int loc = p->location;
                        int map_i = -1, map_j = -1;
                        if (loc >= 0 && loc <= 28) { map_i = 0; map_j = loc; } 
                        else if (loc >= 29 && loc <= 35) { map_i = loc - 28; map_j = 28; } 
                        else if (loc >= 36 && loc <= 63) { map_i = 7; map_j = 28 - (loc - 35); } 
                        else if (loc >= 64 && loc <= 69) { map_i = 7 - (loc - 63); map_j = 0; }

                        if (i == map_i && j == map_j) {
                            if (k == last_moved_player_id) {
                                player_to_show = p;
                                last_moved_player_on_spot = true;
                                break;
                            }
                            player_to_show = p; // default to last player in array if last moved is not here
                        }
                    }
                }


                if (player_to_show) {
                     printf("%s%c%s", player_to_show->color, player_to_show->name[0], COLOR_RESET);
                } else {
                    printf("%c", map[i][j]);
                }
            } else {
                int loc = -1;
                if (i == 0) loc = j;
                else if (i == 7) {
                    if (j <= 13) loc = 63 - j;
                    else if (j == 14) loc = 49; // P
                    else if (j >= 15) loc = 36 + (28-j);
                }
                else if (j == 0 && i > 0 && i < 7) loc = 69 - (i-1);
                else if (j == 28 && i > 0 && i < 7) loc = 28 + i;

                // 检查是否有路障
                if (loc != -1 && has_bomb_at_location(loc)) {
                    printf("%c", BOMB_SYMBOL);   // 显示炸弹符号 @
                } else if (loc != -1 && has_block_at_location(loc)) {
                    printf("%c", BLOCK_SYMBOL);  // 显示路障符号 #
                } else if (loc != -1 && g_game_state.houses[loc].owner_id != -1) {
                    Player* owner = &g_game_state.players[g_game_state.houses[loc].owner_id];
                    int level = g_game_state.houses[loc].level;
                    char symbol = (level > 0 && level <= 3) ? level + '0' : map[i][j];
                    printf("%s%c%s", owner->color, symbol, COLOR_RESET);
                } else {
                    printf("%c", map[i][j]);
                }
            }
        }

        if (i == 3) {
            printf("    地段 2");
        }
        printf("\n");
        if (i == 7) {
            printf("            地段 3\n");
        }
    }
}
