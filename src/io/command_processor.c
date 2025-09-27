#include "command_processor.h"
#include "../game/game_state.h"
#include "../game/player.h"
#include "../game/character.h"
#include "../game/map.h"
#include "../game/land.h"
#include "../game/block_system.h"
#include "../game/god_system.h"
#include "../io/colors.h"
#include "json_serializer.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>

// 全局消息缓冲区
char g_last_action_message[1024] = {0};

// 函数声明
void handle_roll_command();
void handle_step_command(const char* command);
void handle_query_command();
void handle_help_command();
void handle_quit_command();
void handle_sell_command(int location);
void switch_to_next_player();


void process_command(const char* command) {
    // 不再在这里清空消息，而是在消息显示后清空
    char message_buffer[1024] = {0};

    char lower_command[100];
    for (int i = 0; command[i]; i++) {
        lower_command[i] = tolower(command[i]);
    }
    lower_command[strlen(command)] = '\0';

    if (strcmp(lower_command, "roll") == 0) {
        handle_roll_command();
    } else if (strncmp(lower_command, "step", 4) == 0) {
        handle_step_command(lower_command);
    } else if (strcmp(lower_command, "query") == 0) {
        handle_query_command();
    } else if (strcmp(lower_command, "help") == 0) {
        handle_help_command();
    } else if (strncmp(lower_command, "sell ", 5) == 0) {
        int location;
        if (sscanf(command, "sell %d", &location) == 1) {
            handle_sell_command(location);
        } else {
            snprintf(message_buffer, sizeof(message_buffer), "格式错误，请使用: sell <位置>\n");
        }
    } else if (strncmp(lower_command, "block ", 6) == 0) {
        int relative_distance;
        if (sscanf(command, "block %d", &relative_distance) == 1) {
            Player* current_player = &g_game_state.players[g_game_state.game.now_player_id];
            handle_block_command(current_player, relative_distance);
        } else {
            snprintf(message_buffer, sizeof(message_buffer), "格式错误，请使用: block <相对距离>\n");
        }
    } else if (strcmp(lower_command, "robot") == 0) {
        Player* current_player = &g_game_state.players[g_game_state.game.now_player_id];
        handle_robot_command(current_player);
    } else if (strcmp(lower_command, "quit") == 0) {
        handle_quit_command();
    } else if (strncmp(lower_command, "create_player", 13) == 0) {
        // 简单的创建玩家命令: create_player 张三 1500
        char name[32];
        int fund;
        if (sscanf(command, "create_player %s %d", name, &fund) == 2) {
            Player* p = create_player(g_game_state.player_count, name, fund);
            if (p) {
                snprintf(message_buffer, sizeof(message_buffer), "创建玩家成功: %s\n", p->name);
            } else {
                snprintf(message_buffer, sizeof(message_buffer), "创建玩家失败\n");
            }
        } else {
            snprintf(message_buffer, sizeof(message_buffer), "格式错误，请使用: create_player <姓名> <资金>\n");
        }
    } else if (strcmp(lower_command, "status") == 0) {
        print_game_state();
    } else if (strcmp(lower_command, "dump") == 0) {
        // dump命令: 默认保存为dump.json
        save_game_dump("dump.json");
        snprintf(message_buffer, sizeof(message_buffer), "游戏状态已保存到: dump.json\n");
    } else if (strncmp(lower_command, "dump ", 5) == 0) {
        // dump命令: dump filename.json (带文件名)
        char filename[256];
        if (sscanf(command, "dump %s", filename) == 1) {
            save_game_dump(filename);
            snprintf(message_buffer, sizeof(message_buffer), "游戏状态已保存到: %s\n", filename);
        } else {
            snprintf(message_buffer, sizeof(message_buffer), "格式错误，请使用: dump 或 dump <文件名>\n");
        }
    } else if (strncmp(lower_command, "load", 4) == 0) {
        // load命令: load filename.json
        char filename[256];
        if (sscanf(command, "load %s", filename) == 1) {
            if (load_game_preset(filename) == 0) {
                snprintf(message_buffer, sizeof(message_buffer), "游戏状态已从 %s 加载\n", filename);
            } else {
                snprintf(message_buffer, sizeof(message_buffer), "加载失败: %s\n", filename);
            }
        } else {
            snprintf(message_buffer, sizeof(message_buffer), "格式错误，请使用: load <文件名>\n");
        }
    } else {
        snprintf(message_buffer, sizeof(message_buffer), "未知命令: %s\n", command);
        handle_help_command();
    }

    // 如果有消息，则拼接到全局消息缓冲区
    if (strlen(message_buffer) > 0) {
        strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
    }
}

void handle_roll_command() {
    Player* current_player = &g_game_state.players[g_game_state.game.now_player_id];
    char message_buffer[256];
    
    int steps = rand() % 6 + 1;
    snprintf(message_buffer, sizeof(message_buffer), "玩家 %s 掷骰子，点数为 %d\n", current_player->name, steps);
    strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
    
    int original_location = current_player->location;
    int final_location = (original_location + steps) % MAP_SIZE;
    int final_steps = steps;
    bool stopped_by_block = false;
    
    // 检查移动路径上是否有路障或财神
    for (int i = 1; i <= steps; i++) {
        int next_location = (original_location + i) % MAP_SIZE;
        
        // 检查路障拦截
        if (check_block_interception(next_location)) {
            final_location = next_location;
            final_steps = i;
            stopped_by_block = true;
            break; // 找到第一个路障就停下
        }

        // 检查是否遇到财神
        if (check_god_encounter(next_location)) {
            // 遇到财神不停下，直接触发效果
            trigger_god_encounter(current_player, next_location);
        }
    }
    
    // 如果是被路障拦截，在移动和触发地点事件前，先触发路障效果
    if (stopped_by_block) {
        trigger_block_interception(current_player, final_location);
    }

    // 移动到最终位置
    current_player->location = final_location;
    snprintf(message_buffer, sizeof(message_buffer), "%s 前进 %d 步，到达位置 %d\n", current_player->name, final_steps, current_player->location);
    strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);

    // 不在这里触发事件，只标记需要交互
    g_game_state.game.interaction_pending = true;

    // 切换到下一个玩家（游戏未结束时）
    if (!g_game_state.game.ended) {
        switch_to_next_player();
    }
}

void handle_step_command(const char* command) {
    int steps;
    if (sscanf(command, "step %d", &steps) == 1) {
        Player* current_player = &g_game_state.players[g_game_state.game.now_player_id];
        char message_buffer[256];
        
        snprintf(message_buffer, sizeof(message_buffer), "遥控骰子，指定步数为 %d\n", steps);
        strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);

        int original_location = current_player->location;
        int final_location = (original_location + steps) % MAP_SIZE;
        int final_steps = steps;
        bool stopped_by_block = false;
        
        // 检查移动路径上是否有路障或财神
        for (int i = 1; i <= steps; i++) {
            int next_location = (original_location + i) % MAP_SIZE;
            
            // 检查路障拦截
            if (check_block_interception(next_location)) {
                final_location = next_location;
                final_steps = i;
                stopped_by_block = true;
                break; // 找到第一个路障就停下
            }

            // 检查是否遇到财神
            if (check_god_encounter(next_location)) {
                // 遇到财神不停下，直接触发效果
                trigger_god_encounter(current_player, next_location);
            }
        }
        
        // 如果是被路障拦截，在移动和触发地点事件前，先触发路障效果
        if (stopped_by_block) {
            trigger_block_interception(current_player, final_location);
        }
        
        // 移动到最终位置
        current_player->location = final_location;
        snprintf(message_buffer, sizeof(message_buffer), "%s 前进 %d 步，到达位置 %d\n", current_player->name, final_steps, current_player->location);
        strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
        
        // 不在这里触发事件，只标记需要交互
        g_game_state.game.interaction_pending = true;
        
        // 切换到下一个玩家（游戏未结束时）
        if (!g_game_state.game.ended) {
            switch_to_next_player();
        }
    } else {
        snprintf(g_last_action_message, sizeof(g_last_action_message), "无效的 step 命令格式, e.g., step 5\n");
    }
}

void handle_query_command() {
    Player* p = &g_game_state.players[g_game_state.game.now_player_id];
    char buffer[1024] = {0};
    char line[256];

    strncat(buffer, "资产查询:\n", sizeof(buffer) - strlen(buffer) - 1);
    snprintf(line, sizeof(line), "  玩家: %s\n", p->name);
    strncat(buffer, line, sizeof(buffer) - strlen(buffer) - 1);
    snprintf(line, sizeof(line), "  资金: %d 元\n", p->fund);
    strncat(buffer, line, sizeof(buffer) - strlen(buffer) - 1);
    snprintf(line, sizeof(line), "  点数: %d 点\n", p->credit);
    strncat(buffer, line, sizeof(buffer) - strlen(buffer) - 1);
    snprintf(line, sizeof(line), "  位置: %d\n", p->location);
    strncat(buffer, line, sizeof(buffer) - strlen(buffer) - 1);
    strncat(buffer, "  道具:\n", sizeof(buffer) - strlen(buffer) - 1);
    snprintf(line, sizeof(line), "    - 路障: %d\n", p->prop.barrier);
    strncat(buffer, line, sizeof(buffer) - strlen(buffer) - 1);
    snprintf(line, sizeof(line), "    - 机器娃娃: %d\n", p->prop.robot);
    strncat(buffer, line, sizeof(buffer) - strlen(buffer) - 1);
    strncat(buffer, "  状态:\n", sizeof(buffer) - strlen(buffer) - 1);
    if (p->buff.god > 0) {
        snprintf(line, sizeof(line), "    - 财神附身: 剩余 %d 回合\n", p->buff.god);
        strncat(buffer, line, sizeof(buffer) - strlen(buffer) - 1);
    } else {
        strncat(buffer, "    - (无特殊状态)\n", sizeof(buffer) - strlen(buffer) - 1);
    }
    strncat(buffer, "  房产:\n", sizeof(buffer) - strlen(buffer) - 1);
    bool has_house = false;
    for (int i = 0; i < MAP_SIZE; i++) {
        if (g_game_state.houses[i].owner_id == p->index) {
            snprintf(line, sizeof(line), "    - 位置 %d (等级 %d)\n", i, g_game_state.houses[i].level);
            strncat(buffer, line, sizeof(buffer) - strlen(buffer) - 1);
            has_house = true;
        }
    }
    if (!has_house) {
        strncat(buffer, "    (无)\n", sizeof(buffer) - strlen(buffer) - 1);
    }

    // 显示地图财神状态
    strncat(buffer, "  地图财神状态:\n", sizeof(buffer) - strlen(buffer) - 1);
    if (g_game_state.god.location != -1) {
        snprintf(line, sizeof(line), "    - 位置: %d (剩余 %d 回合消失)\n", g_game_state.god.location, g_game_state.god.duration);
        strncat(buffer, line, sizeof(buffer) - strlen(buffer) - 1);
    } else {
        snprintf(line, sizeof(line), "    - (未出现，预计 %d 回合后出现)\n", g_game_state.god.spawn_cooldown);
        strncat(buffer, line, sizeof(buffer) - strlen(buffer) - 1);
    }
    
    // 将所有内容复制到全局消息缓冲区
    strncpy(g_last_action_message, buffer, sizeof(g_last_action_message) - 1);
}

void handle_help_command() {
    char help_text[] =
        "命令帮助:\n"
        "roll\n"
        "  掷骰子命令，行走1～6步，步数由随机算法产生。\n"
        "sell n\n"
        "  出售房产，n为房产在地图上的绝对位置，出售价格为投资总成本的2倍。\n"
        "block n\n"
        "  放置路障，n为前后相对距离（±10步），玩家经过将被拦截。\n"
        "robot\n"
        "  清扫前方10步内的障碍（路障）。\n"
        "query\n"
        "  显示自家资产信息。\n"
        "help\n"
        "  查看命令帮助。\n"
        "quit\n"
        "  强制退出游戏。\n"
        "step n\n"
        "  遥控骰子，指定行走步数。\n";
    strncpy(g_last_action_message, help_text, sizeof(g_last_action_message) - 1);
}

void handle_quit_command() {
    g_game_state.game.ended = true;
    strncpy(g_last_action_message, "游戏已退出。\n", sizeof(g_last_action_message) - 1);
    printf("%s", g_last_action_message); // 确保退出前能看到消息
    exit(0); // 强制退出程序
}


int get_initial_fund(void) {
    char input[20];
    int fund = 10000; // 默认资金
    printf("请设置玩家初始资金（范围：1000～50000，默认10000），直接回车使用默认资金: ");
    if (fgets(input, sizeof(input), stdin) != NULL && input[0] != '\n') {
        int temp_fund = atoi(input);
        if (temp_fund >= 1000 && temp_fund <= 50000) {
            fund = temp_fund;
        } else {
            printf("无效的资金数额，将使用默认资金 10000。\n");
        }
    }
    printf("初始资金设置为: %d\n", fund);
    return fund;
}

void show_welcome_and_select_character(int initial_fund) {
    init_characters();
    
    while (g_game_state.player_count < 2 || g_game_state.player_count > 4) {
        show_character_selection();
        
        char input[10];
        printf("请选择2～4位不重复玩家，输入编号即可（1、钱夫人；2、阿土伯；3、孙小美；4、金贝贝）: ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            return;
        }
        
        input[strcspn(input, "\n")] = 0;
        
        bool valid_input = true;
        bool used[5] = {false};
        int len = strlen(input);
        if (len < 2 || len > 4) {
            printf("请选择 2-4 位玩家。\n");
            continue;
        }
        
        for (int i = 0; i < len; i++) {
            if (input[i] < '1' || input[i] > '4') {
                valid_input = false;
                break;
            }
            int digit = input[i] - '0';
            if (used[digit]) {
                printf("角色选择重复。\n");
                valid_input = false; 
                break;
            }
            used[digit] = true;
        }
        
        if (!valid_input) {
            printf("无效选择，请输入1-4之间的不重复数字。\n");
            g_game_state.player_count = 0; // 重置玩家计数
            continue;
        }
        
        for (int i = 0; i < len; i++) {
            int choice = input[i] - '0';
            Player* player = create_player_by_character(choice, initial_fund);
            if (player) {
                 printf("玩家 %s (%s) 加入游戏。\n", player->name, get_character_by_id(choice)->display_name);
            }
        }
        
        if (g_game_state.player_count > 0) {
            printf("\n游戏开始！\n");
            g_game_state.game.started = true;
            break;
        }
    }
}

void run_game_with_preset(const char* preset_file) {
    printf("大富翁游戏启动\n");
    srand(time(NULL)); // 初始化随机数种子
    
    init_game_state();
    bool game_started = false;
    
    const char* file_to_load = preset_file ? preset_file : "preset.json";
    if (load_game_preset(file_to_load) == 0) {
        printf("使用预设配置: %s\n", file_to_load);
        game_started = true; // 使用预设配置时，游戏已经开始
    } else {
        int initial_fund = get_initial_fund();
        show_welcome_and_select_character(initial_fund);
        
        if (g_game_state.player_count == 0) {
            printf("没有选择任何角色，游戏结束\n");
            return;
        }
        game_started = true;
    }
    
    char command[100];

    // 初始显示
    printf(CLEAR_SCREEN);
    display_map();

    while (true) {
        // 财神状态更新应该移到合适的地方，而不是在每个游戏循环中都调用

        // 检查胜利条件
        if (game_started) {
            int alive_count = 0;
            int winner_id = -1;
            for (int i = 0; i < g_game_state.player_count; i++) {
                if (g_game_state.players[i].alive) {
                    alive_count++;
                    winner_id = i;
                }
            }

            if (alive_count <= 1) {
                if (!g_game_state.game.ended) {
                    char message_buffer[256];
                    if (winner_id != -1) {
                        snprintf(message_buffer, sizeof(message_buffer), "游戏结束！胜利者是 %s！\n", g_game_state.players[winner_id].name);
                    } else {
                        snprintf(message_buffer, sizeof(message_buffer), "所有玩家都已破产，游戏结束！\n");
                    }
                    strncpy(g_last_action_message, message_buffer, sizeof(g_last_action_message) - 1);
                    
                    printf(CLEAR_SCREEN);
                    display_map();
                    printf("%s", g_last_action_message);

                    g_game_state.game.ended = true;
                    g_game_state.game.winner_id = winner_id;
                }
                // 游戏结束后仍然允许处理命令（如dump）
            }
        }

        Player* current_player = &g_game_state.players[g_game_state.game.now_player_id];

        // 检查财神附身状态
        if (current_player->buff.god > 0) {
            char message_buffer[256];
            snprintf(message_buffer, sizeof(message_buffer), "玩家 %s 有财神附身，免过路费，剩余 %d 回合。\n",
                   current_player->name, current_player->buff.god);
            strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
            current_player->buff.god--;
        }

        // 检查当前玩家是否已破产，如果是则自动跳过（游戏未结束时）
        if (!current_player->alive && !g_game_state.game.ended) {
            char message_buffer[256];
            snprintf(message_buffer, sizeof(message_buffer), "玩家 %s 已破产，自动跳过。\n", current_player->name);
            strncat(g_last_action_message, message_buffer, sizeof(g_last_action_message) - strlen(g_last_action_message) - 1);
            switch_to_next_player();
            continue; // 直接进入下一位玩家
        }
        
        // 检查当前玩家是否在住院，如果是则自动跳过
        // if (current_player->buff.hospital > 0) {
        //     printf("玩家 %s 正在住院治疗，剩余 %d 天，本轮自动跳过。\n", 
        //            current_player->name, current_player->buff.hospital);
        //     current_player->buff.hospital--;
        //     if (!g_game_state.game.ended) {
        //         switch_to_next_player();
        //     }
        //     //wait_for_enter();
        //     continue; // 直接进入下一轮
        // }
        
        // 检查当前玩家是否在监狱，如果是则自动跳过
        // if (current_player->buff.prison > 0) {
        //     printf("玩家 %s 正在监狱中，剩余 %d 天，本轮自动跳过。\n", 
        //            current_player->name, current_player->buff.prison);
        //     current_player->buff.prison--;
        //     if (!g_game_state.game.ended) {
        //         switch_to_next_player();
        //     }
        //     //wait_for_enter();
        //     continue; // 直接进入下一轮
        // }
        
        
        // 如果有待处理的交互，现在执行它
        if (g_game_state.game.interaction_pending) {
            Player* player_for_interaction = &g_game_state.players[g_game_state.game.last_player_id];
            
            // 先将移动消息打印出来
            if (strlen(g_last_action_message) > 0) {
                printf("%s", g_last_action_message);
                g_last_action_message[0] = '\0'; // 打印后清空
            }

            // 执行落地事件，这可能会产生新的交互或消息
            on_player_land(player_for_interaction);

            // 打印落地事件产生的消息（例如买地成功、获得点数等）
            if (strlen(g_last_action_message) > 0) {
                printf("%s", g_last_action_message);
            }

            // 完成后，清除所有状态
            g_game_state.game.interaction_pending = false;
            g_last_action_message[0] = '\0';
        } else {
            // 如果没有交互，但有其他消息（如财神出现），在这里打印
            if (strlen(g_last_action_message) > 0) {
                printf("%s", g_last_action_message);
                g_last_action_message[0] = '\0'; // 打印后清空
            }
        }

        printf("%s%c%s> ", current_player->color, current_player->name[0], COLOR_RESET);
        
        if (fgets(command, sizeof(command), stdin) == NULL) {
            break;
        }
        
        // Trim trailing newline
        command[strcspn(command, "\n")] = 0;
        
        process_command(command);

        // 在处理命令后清屏并重绘
        printf(CLEAR_SCREEN);
        display_map();

        // 在显示提示符前，打印命令处理后产生的消息
        if (strlen(g_last_action_message) > 0) {
            // 如果没有待处理的交互，就在这里打印消息
            if (!g_game_state.game.interaction_pending) {
                 printf("%s", g_last_action_message);
            }
        }
    }
}

void switch_to_next_player() {
    g_game_state.game.last_player_id = g_game_state.game.now_player_id;
    g_game_state.game.now_player_id = (g_game_state.game.now_player_id + 1) % g_game_state.player_count;
    g_game_state.game.next_player_id = (g_game_state.game.now_player_id + 1) % g_game_state.player_count;
    
    // 当轮到第一个玩家时，表示新一轮开始，更新财神状态
    if (g_game_state.game.now_player_id == 0) {
        update_god_status();
    }
}

void run_game(void) {
    run_game_with_preset(NULL);
}