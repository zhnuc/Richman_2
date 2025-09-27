#include "gift_house.h"
#include <stdio.h>

void enter_gift_house(Player* player) {
    printf("欢迎光临礼品屋，请选择一件您喜欢的礼品：\n");
    printf("1. 奖金 (2000元)\n");
    printf("2. 点数卡 (200点)\n");
    printf("3. 财神 (财神附身，5轮内免过路费)\n");

    char input[10];
    printf("请输入礼品编号 (1-3): ");
    fgets(input, sizeof(input), stdin);

    int choice = -1;
    sscanf(input, "%d", &choice);

    switch (choice) {
        case 1:
            player->fund += 2000;
            printf("您获得了 2000 元奖金！\n");
            break;
        case 2:
            player->credit += 200;
            printf("您获得了 200 点数！\n");
            break;
        case 3:
            player->buff.god = 5;
            printf("财神已附身！5轮内免过路费。\n");
            break;
        default:
            printf("无效的选择，您放弃了这次机会。\n");
            break;
    }
}
