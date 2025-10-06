#ifndef LAND_H
#define LAND_H

#include "../game/game_types.h"

void on_player_land(Player* player);
void handle_sell_command(int location);
void check_win_condition();

#endif // LAND_H
