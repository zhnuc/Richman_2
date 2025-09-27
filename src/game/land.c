#include "land.h"
#include "game_state.h"
#include "prop_shop.h"
#include "gift_house.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Forward declarations
void upgrade_land(Player* player, int location);
void pay_toll(Player* player, int location);

void buy_land(Player* player, int location) {
    House* land = &g_game_state.houses[location];

    if (land->price == 0) {
        printf("此地为特殊地点，不可购买。\n");
        return;
    }

    if (land->owner_id != -1) {
        if (land->owner_id == player->index) {
            printf("您已经是这块地的主人了。\n");
        } else {
            printf("此地已被其他玩家购买。\n");
        }
        return;
    }

    if (player->fund < land->price) {
        printf("资金不足，无法购买此地。需要 %d，您只有 %d。\n", land->price, player->fund);
        return;
    }

    char input[10];
    int valid_input = 0;
    
    while (!valid_input) {
        printf("您到达一块空地(价格: %d)，是否购买? (y/n): ", land->price);
        if (fgets(input, sizeof(input), stdin) == NULL) {
            // 输入流结束，自动选择不购买
            printf("您放弃了购买此地。\n");
            return;
        }
        
        // 移除换行符
        input[strcspn(input, "\n")] = 0;
        
        // 检查输入是否是 y 或 n
        if (strcmp(input, "y") == 0) {
            player->fund -= land->price;
            land->owner_id = player->index;
            printf("恭喜！您成功购买了此地。剩余资金: %d\n", player->fund);
            valid_input = 1;
        } else if (strcmp(input, "n") == 0) {
            printf("您放弃了购买此地。\n");
            valid_input = 1;
        } else {
            // 输入不是 y 或 n，提示错误并循环
            printf("错误指令！请输入 y 或 n。\n");
        }
    }
}

void upgrade_land(Player* player, int location) {
    House* land = &g_game_state.houses[location];
    int upgrade_cost = land->price;

    if (land->level >= 3) {
        printf("您的房产已是最高级(摩天楼)，无法再升级。\n");
        return;
    }

    if (player->fund < upgrade_cost) {
        printf("资金不足，无法升级。需要 %d，您只有 %d。\n", upgrade_cost, player->fund);
        return;
    }

    char input[10];
    printf("您的房产当前为 %d 级，可升级至 %d 级(费用: %d)，是否升级? (y/n): ", land->level, land->level + 1, upgrade_cost);
    fgets(input, sizeof(input), stdin);

    if (tolower(input[0]) == 'y') {
        player->fund -= upgrade_cost;
        land->level++;
        printf("恭喜！升级成功。当前等级: %d，剩余资金: %d\n", land->level, player->fund);
    } else {
        printf("您放弃了升级。\n");
    }
}

void handle_sell_command(int location) {
    Player* player = &g_game_state.players[g_game_state.game.now_player_id];

    if (location < 0 || location >= MAP_SIZE) {
        printf("出售失败：无效的位置。\n");
        return;
    }

    House* land = &g_game_state.houses[location];

    if (land->owner_id != player->index) {
        printf("出售失败：这不是您的房产。\n");
        return;
    }

    int total_investment = land->price * (1 + land->level);
    int sell_price = total_investment * 2;

    // 直接执行出售操作，不需要确认
    player->fund += sell_price;
    land->owner_id = -1;
    land->level = 0;
    printf("出售成功！您获得了 %d 元，当前总资金: %d\n", sell_price, player->fund);
}

void pay_toll(Player* player, int location) {
    House* land = &g_game_state.houses[location];
    Player* owner = &g_game_state.players[land->owner_id];
    int toll = (land->price * (land->level + 1)) / 2;

    // 检查房主是否在医院或监狱
    if (owner->buff.hospital > 0 || owner->buff.prison > 0) {
        printf("房主 %s 正在医院或监狱中，免除本次过路费！\n", owner->name);
        return;
    }

    // 检查财神附身
    if (player->buff.god > 0) {
        printf("财神附身，免除本次过路费！\n");
        return;
    }

    printf("您到达了玩家 %s 的地盘(等级 %d)，需支付过路费 %d 元。\n", owner->name, land->level, toll);

    if (player->fund < toll) {
        printf("您的资金不足以支付过路费 %d 元，您已破产！\n", toll);
        printf("您的所有资产（包括剩余资金 %d 元）已被系统没收。\n", player->fund);

        
        player->fund = 0; // 玩家资金归零
        player->alive = false;
        
        // 清空破产玩家的道具
        player->prop.bomb = 0;
        player->prop.barrier = 0;
        player->prop.robot = 0;

        // 将破产玩家的房产变为空地
        for (int i = 0; i < MAP_SIZE; i++) {
            if (g_game_state.houses[i].owner_id == player->index) {
                g_game_state.houses[i].owner_id = -1;
                g_game_state.houses[i].level = 0;
            }
        }
        printf("玩家 %s 的所有房产和道具已被清空。\n", player->name);
        
        // 检查游戏是否结束
        int alive_count = 0;
        int winner_id = -1;
        for (int i = 0; i < g_game_state.player_count; i++) {
            if (g_game_state.players[i].alive) {
                alive_count++;
                winner_id = i;
            }
        }
        
        if (alive_count <= 1) {
            g_game_state.game.ended = true;
            if (winner_id != -1) {
                g_game_state.game.winner_id = winner_id;
                g_game_state.game.now_player_id = winner_id;
                g_game_state.game.next_player_id = winner_id;
            }
        }

    } else {
        player->fund -= toll;
        owner->fund += toll;
        printf("支付成功。您的剩余资金: %d\n", player->fund);
    }
}

void on_player_land(Player* player) {
    int location = player->location;
    House* land = &g_game_state.houses[location];

    // 检查是否是道具屋 (位置28 - T)
    if (location == PROP_SHOP_LOCATION) {
        printf("您到达了道具屋。\n");
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
                printf("您到达了起点。\n");
                break;
            case 14:  // H - 医院
                printf("您到达了医院。\n");
                break;
            case 49:  // P - 监狱 
                printf("您到达了监狱，被关押2天。\n");
                player->buff.prison = 2;
                break;
            case 35:  // G - 礼品屋
                enter_gift_house(player);
                break;
            case 63:  // M - 魔法屋
                printf("您到达了魔法屋，暂时无效果\n");
                break;
            default:
                // 检查是否是矿地 ($) - 位置64-69
                if (location >= 64 && location <= 69) {
                    // 矿地点数：从上到下依次为 20、80、100、40、80、60
                    int credits[] = {60, 80, 40, 100, 80, 20};
                    int index = location - 64;
                    if (index >= 0 && index < 6) {
                        player->credit += credits[index];
                        printf("您到达了矿地，获得了 %d 点数！当前点数：%d\n", 
                               credits[index], player->credit);
                    } else {
                        printf("您到达了特殊地点。\n");
                    }
                } else {
                    printf("您到达了特殊地点。\n");
                }
                break;
        }
    }
}
