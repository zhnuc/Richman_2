#ifndef COMMAND_PROCESSOR_H
#define COMMAND_PROCESSOR_H

// 全局消息缓冲区
extern char g_last_action_message[1024];

// 命令行处理函数声明
void process_command(const char* command);
void run_game(void);
void run_game_with_preset(const char* preset_file);
int get_initial_fund(void);
void show_welcome_and_select_character(int initial_fund);

#endif // COMMAND_PROCESSOR_H