#ifndef GOD_SYSTEM_H
#define GOD_SYSTEM_H

#include "game_types.h"

void update_god_status();
// 检查指定位置是否有财神
bool check_god_encounter(int location);
// 触发财神效果
void trigger_god_encounter(Player* player, int location);

#endif // GOD_SYSTEM_H
