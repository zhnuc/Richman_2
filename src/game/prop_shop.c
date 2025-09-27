#include "prop_shop.h"
#include "game_state.h"
#include "../io/command_processor.h" // 包含 g_last_action_message
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// 道具信息结构体
typedef struct {
    int id;
    const char* name;
    int price;
    const char* display_symbol;
} PropInfo;

// 道具信息数据表
static PropInfo prop_info[] = {
    {1, "路障", PROP_BARRIER_PRICE, "#"},
    {2, "机器娃娃", PROP_ROBOT_PRICE, "无"}
};

static int prop_count = sizeof(prop_info) / sizeof(PropInfo);

// 获取道具价格
int get_prop_price(int prop_id) {
    if (prop_id < 1 || prop_id > prop_count) {
        return -1;
    }
    return prop_info[prop_id - 1].price;
}

// 获取道具名称
const char* get_prop_name(int prop_id) {
    if (prop_id < 1 || prop_id > prop_count) {
        return "未知道具";
    }
    return prop_info[prop_id - 1].name;
}

// 检查玩家是否有足够点数购买道具
bool can_afford_prop(Player* player, int prop_id) {
    int price = get_prop_price(prop_id);
    return price > 0 && player->credit >= price;
}

// 检查玩家是否还有道具空间
bool has_prop_space(Player* player) {
    return player->prop.total < MAX_PROPS;
}

// 显示道具屋菜单
void show_prop_shop_menu(void) {
    printf("\n");
    printf("欢迎光临道具屋，请选择您所需要的道具：\n");
    printf("\n");
    printf("道具    编号    价值（点数）    显示方式\n");
    
    for (int i = 0; i < prop_count; i++) {
        PropInfo* prop = &prop_info[i];
        printf("%-8s %-8d %-12d %s\n", 
               prop->name, prop->id, prop->price, prop->display_symbol);
    }
    printf("\n");
    printf("请输入道具编号选择道具，按F退出道具屋：");
}

// 购买道具核心逻辑
bool buy_prop(Player* player, int prop_id) {
    char message_buffer[256];

    // 验证道具ID
    if (prop_id < 1 || prop_id > prop_count) {
        snprintf(message_buffer, sizeof(message_buffer), "无效的道具编号。\n");
        strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
        return false;
    }
    
    // 检查道具空间
    if (!has_prop_space(player)) {
        snprintf(message_buffer, sizeof(message_buffer), "您的道具已达到上限（%d个），无法购买更多道具。\n", MAX_PROPS);
        strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
        return false;
    }
    
    // 检查点数是否足够
    if (!can_afford_prop(player, prop_id)) {
        int price = get_prop_price(prop_id);
        snprintf(message_buffer, sizeof(message_buffer), "点数不足，无法购买%s。需要%d点数，您只有%d点数。\n", 
               get_prop_name(prop_id), price, player->credit);
        strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
        return false;
    }
    
    // 执行购买交易
    int price = get_prop_price(prop_id);
    player->credit -= price;
    
    // 根据道具类型增加对应道具数量
    switch (prop_id) {
        case 1: // 路障
            player->prop.barrier++;
            break;
        case 2: // 机器娃娃
            player->prop.robot++;
            break;
    }
    
    player->prop.total++;
    
    snprintf(message_buffer, sizeof(message_buffer), "购买成功！您获得了%s。剩余点数：%d\n", 
           get_prop_name(prop_id), player->credit);
    strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
    
    return true;
}

// 进入道具屋主逻辑
void enter_prop_shop(Player* player) {
    char message_buffer[256];
    // 首先检查玩家是否有点数购买最便宜的道具
    int min_price = PROP_ROBOT_PRICE; // 机器娃娃是最便宜的(30点)
    if (player->credit < min_price) {
        snprintf(message_buffer, sizeof(message_buffer), "您的点数不足以购买任何道具，自动退出道具屋。\n");
        strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
        return;
    }
    
    char input[10];
    
    while (true) {
        // 交互式内容保留 printf
        show_prop_shop_menu();
        
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }
        
        // 移除换行符
        input[strcspn(input, "\n")] = 0;
        
        // 检查是否退出 (F或f)
        if (tolower(input[0]) == 'f') {
            snprintf(message_buffer, sizeof(message_buffer), "您退出了道具屋。\n");
            strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
            break;
        }
        
        // 尝试解析道具编号
        int prop_id = atoi(input);
        if (prop_id == 0 && input[0] != '0') {
            // 交互式错误提示
            printf("无效输入，请输入道具编号或F退出。\n");
            continue;
        }
        
        // 尝试购买道具
        if (buy_prop(player, prop_id)) {
            // 购买成功后，消息已在 buy_prop 中处理
            // 检查是否还能继续购买
            bool can_buy_any = false;
            for (int i = 1; i <= prop_count; i++) {
                if (can_afford_prop(player, i) && has_prop_space(player)) {
                    can_buy_any = true;
                    break;
                }
            }
            
            // 如果不能再购买任何道具，自动退出
            if (!can_buy_any) {
                if (!has_prop_space(player)) {
                    snprintf(message_buffer, sizeof(message_buffer), "您的道具已满，自动退出道具屋。\n");
                } else {
                    snprintf(message_buffer, sizeof(message_buffer), "您的点数不足以购买任何道具，自动退出道具屋。\n");
                }
                strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
                break;
            }
        }
    }
}