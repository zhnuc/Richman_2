#include "land.h"
#include "game_state.h"
#include "prop_shop.h"
#include "gift_house.h"
#include "../io/command_processor.h" // 包含 g_last_action_message
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Forward declarations
void upgrade_land(Player* player, int location);
void pay_toll(Player* player, int location);

// 统一的胜利条件检查函数
void check_win_condition() {
    int alive_count = 0;
    int winner_id = -1;
    
    for (int i = 0; i < g_game_state.player_count; i++) {
        if (g_game_state.players[i].alive) {
            alive_count++;
            winner_id = i;
        }
    }
    
    if (alive_count <= 1 && !g_game_state.game.ended) {
        g_game_state.game.ended = true;
        if (winner_id != -1) {
            // 有胜利者
            g_game_state.game.winner_id = winner_id;
            g_game_state.game.now_player_id = winner_id;
            g_game_state.game.next_player_id = winner_id;
        } else {
            // 所有玩家都破产，没有胜利者
            g_game_state.game.winner_id = -1;
            g_game_state.game.now_player_id = -1;
            g_game_state.game.next_player_id = -1;
        }
    }
}

void buy_land(Player* player, int location) {
    House* land = &g_game_state.houses[location];
    char message_buffer[256];

    if (land->price == 0) {
        snprintf(message_buffer, sizeof(message_buffer), "此地为特殊地点，不可购买。\n");
        strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
        return;
    }

    if (land->owner_id != -1) {
        if (land->owner_id == player->index) {
            snprintf(message_buffer, sizeof(message_buffer), "您已经是这块地的主人了。\n");
        } else {
            snprintf(message_buffer, sizeof(message_buffer), "此地已被其他玩家购买。\n");
        }
        strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
        return;
    }

    if (player->fund < land->price) {
        snprintf(message_buffer, sizeof(message_buffer), "资金不足，无法购买此地。需要 %d，您只有 %d。\n", land->price, player->fund);
        strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
        return;
    }

    char input[10];
    int valid_input = 0;
    
    // 交互式提示需要立即显示，所以这里保留printf
    printf("您到达一块空地(价格: %d)，是否购买? (y/n): ", land->price);

    while (!valid_input) {
        if (fgets(input, sizeof(input), stdin) == NULL) {
            // 输入流结束，自动选择不购买
            snprintf(message_buffer, sizeof(message_buffer), "您放弃了购买此地。\n");
            strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
            return;
        }
        
        // 移除换行符
        input[strcspn(input, "\n")] = 0;
        
        // 检查输入是否是 y 或 n
        if (strcmp(input, "y") == 0) {
            player->fund -= land->price;
            land->owner_id = player->index;
            snprintf(message_buffer, sizeof(message_buffer), "恭喜！您成功购买了此地。剩余资金: %d\n", player->fund);
            strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
            valid_input = 1;
        } else if (strcmp(input, "n") == 0) {
            snprintf(message_buffer, sizeof(message_buffer), "您放弃了购买此地。\n");
            strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
            valid_input = 1;
        } else {
            // 输入不是 y 或 n，提示错误并循环
            printf("错误指令！请输入 y 或 n: "); // 交互式提示
        }
    }
}

void upgrade_land(Player* player, int location) {
    House* land = &g_game_state.houses[location];
    int upgrade_cost = land->price;
    char message_buffer[256];

    if (land->level >= 3) {
        snprintf(message_buffer, sizeof(message_buffer), "您的房产已是最高级(摩天楼)，无法再升级。\n");
        strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
        return;
    }

    if (player->fund < upgrade_cost) {
        snprintf(message_buffer, sizeof(message_buffer), "资金不足，无法升级。需要 %d，您只有 %d。\n", upgrade_cost, player->fund);
        strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
        return;
    }

    char input[10];
    // 交互式提示
    printf("您的房产当前为 %d 级，可升级至 %d 级(费用: %d)，是否升级? (y/n): ", land->level, land->level + 1, upgrade_cost);
    fgets(input, sizeof(input), stdin);

    if (tolower(input[0]) == 'y') {
        player->fund -= upgrade_cost;
        land->level++;
        snprintf(message_buffer, sizeof(message_buffer), "恭喜！升级成功。当前等级: %d，剩余资金: %d\n", land->level, player->fund);
        strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
    } else {
        snprintf(message_buffer, sizeof(message_buffer), "您放弃了升级。\n");
        strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
    }
}

void handle_sell_command(int location) {
    Player* player = &g_game_state.players[g_game_state.game.now_player_id];
    char message_buffer[256];

    if (location < 0 || location >= MAP_SIZE) {
        snprintf(message_buffer, sizeof(message_buffer), "出售失败：无效的位置。\n");
        strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
        return;
    }

    House* land = &g_game_state.houses[location];

    if (land->owner_id != player->index) {
        snprintf(message_buffer, sizeof(message_buffer), "出售失败：这不是您的房产。\n");
        strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
        return;
    }

    int total_investment = land->price * (1 + land->level);
    int sell_price = total_investment * 2;

    // 直接执行出售操作，不需要确认
    player->fund += sell_price;
    land->owner_id = -1;
    land->level = 0;
    snprintf(message_buffer, sizeof(message_buffer), "出售成功！您获得了 %d 元，当前总资金: %d\n", sell_price, player->fund);
    strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
}

void pay_toll(Player* player, int location) {
    House* land = &g_game_state.houses[location];
    
    // 检查土地是否真的有主人
    if (land->owner_id == -1 || land->owner_id >= g_game_state.player_count) {
        // 土地无主或主人无效，不应该收取过路费
        char message_buffer[256];
        snprintf(message_buffer, sizeof(message_buffer), "您到达了一块空地，无需支付过路费。\n");
        strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
        return;
    }
    
    Player* owner = &g_game_state.players[land->owner_id];
    int toll = (land->price * (land->level + 1)) / 2;
    char message_buffer[512];

    // 检查财神附身
    if (player->buff.god > 0) {
        snprintf(message_buffer, sizeof(message_buffer), "财神附身，免除本次过路费！\n");
        strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
        return;
    }

    snprintf(message_buffer, sizeof(message_buffer), "您到达了玩家 %s 的地盘(等级 %d)，需支付过路费 %d 元。\n", owner->name, land->level, toll);
    strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);

    if (player->fund < toll) {
        snprintf(message_buffer, sizeof(message_buffer), "您的资金不足以支付过路费 %d 元，您已破产！\n您的所有资产（包括剩余资金 %d 元）已被系统没收。\n", toll, player->fund);
        strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
        
        player->fund = 0; // 玩家资金归零
        player->alive = false;
        
        // 清空破产玩家的道具
        player->prop.bomb = 0;
        player->prop.barrier = 0;
        player->prop.robot = 0;
        
        // 清空破产玩家的状态效果
        player->buff.god = 0;
        player->buff.prison = 0;
        player->buff.hospital = 0;

        // 将破产玩家的房产变为空地
        for (int i = 0; i < MAP_SIZE; i++) {
            if (g_game_state.houses[i].owner_id == player->index) {
                g_game_state.houses[i].owner_id = -1;
                g_game_state.houses[i].level = 0;
            }
        }
        
        // 清空破产玩家放置的道具
        for (int i = 0; i < MAP_SIZE; i++) {
            if (g_game_state.placed_prop.barrier[i] == 1) {
                // 检查是否是当前玩家放置的路障，如果是则移除
                // 注意：这里简化处理，实际应该记录道具的放置者
                g_game_state.placed_prop.barrier[i] = 0;
            }
            if (g_game_state.placed_prop.bomb[i] == 1) {
                // 检查是否是当前玩家放置的炸弹，如果是则移除
                g_game_state.placed_prop.bomb[i] = 0;
            }
        }
        
        snprintf(message_buffer, sizeof(message_buffer), "玩家 %s 的所有房产和道具已被清空。\n", player->name);
        strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
        
        // 检查游戏是否结束
        check_win_condition();

    } else {
        player->fund -= toll;
        owner->fund += toll;
        snprintf(message_buffer, sizeof(message_buffer), "支付成功。您的剩余资金: %d\n", player->fund);
        strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
    }
}

void on_player_land(Player* player) {
    int location = player->location;
    House* land = &g_game_state.houses[location];
    char message_buffer[256];

    // 检查是否是道具屋 (位置28 - T)
    if (location == PROP_SHOP_LOCATION) {
        snprintf(message_buffer, sizeof(message_buffer), "您到达了道具屋。\n");
        strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
        enter_prop_shop(player);
        return;
    }

    if (land->owner_id == player->index) {
        // Owned by self
        upgrade_land(player, location);
    } else if (land->owner_id != -1 && land->owner_id != player->index) {
        // Owned by another player
        pay_toll(player, location);
    } else if (land->owner_id == -1 && land->price > 0) {
        // Unowned land
        buy_land(player, location);
    } else {
        // 其他特殊地点
        switch (location) {
            case 0:   // S - 起点
                snprintf(message_buffer, sizeof(message_buffer), "您到达了起点。\n");
                strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
                break;
            case 14:  // H - 医院 -> 公园
            case 49:  // P - 监狱 -> 公园
            case 63:  // M - 魔法屋 -> 公园
                snprintf(message_buffer, sizeof(message_buffer), "您到达了公园。\n");
                strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
                break;
            case 35:  // G - 礼品屋
                enter_gift_house(player);
                break;
            default:
                // 检查是否是矿地 ($) - 位置64-69
                if (location >= 64 && location <= 69) {
                    // 矿地点数：从上到下依次为 20、80、100、40、80、60
                    int credits[] = {60, 80, 40, 100, 80, 20};
                    int index = location - 64;
                    if (index >= 0 && index < 6) {
                        player->credit += credits[index];
                        snprintf(message_buffer, sizeof(message_buffer), "您到达了矿地，获得了 %d 点数！当前点数：%d\n", 
                               credits[index], player->credit);
                        strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
                    } else {
                        snprintf(message_buffer, sizeof(message_buffer), "您到达了特殊地点。\n");
                        strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
                    }
                } else {
                    snprintf(message_buffer, sizeof(message_buffer), "您到达了特殊地点。\n");
                    strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
                }
                break;
        }
    }
}
