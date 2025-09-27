#include "gift_house.h"
#include "../io/command_processor.h" // 包含 g_last_action_message
#include <stdio.h>
#include <string.h>

void enter_gift_house(Player* player) {
    char message_buffer[256];
    
    snprintf(message_buffer, sizeof(message_buffer), "欢迎光临礼品屋，请选择一件您喜欢的礼品：\n1. 奖金 (2000元)\n2. 点数卡 (200点)\n3. 财神 (财神附身，5轮内免过路费)\n");
    strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);

    // 交互式提示
    printf("欢迎光临礼品屋，请选择一件您喜欢的礼品：\n");
    printf("1. 奖金 (2000元)\n");
    printf("2. 点数卡 (200点)\n");
    printf("3. 财神 (财神附身，5轮内免过路费)\n");
    printf("请输入礼品编号 (1-3): ");

    char input[10];
    fgets(input, sizeof(input), stdin);

    int choice = -1;
    sscanf(input, "%d", &choice);

    // 清空之前的欢迎消息，准备写入结果消息
    g_last_action_message[0] = '\0';

    switch (choice) {
        case 1:
            player->fund += 2000;
            snprintf(message_buffer, sizeof(message_buffer), "您获得了 2000 元奖金！\n");
            break;
        case 2:
            player->credit += 200;
            snprintf(message_buffer, sizeof(message_buffer), "您获得了 200 点数！\n");
            break;
        case 3:
            player->buff.god = 5;
            snprintf(message_buffer, sizeof(message_buffer), "财神已附身！5轮内免过路费。\n");
            break;
        default:
            snprintf(message_buffer, sizeof(message_buffer), "无效的选择，您放弃了这次机会。\n");
            break;
    }
    strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
}
