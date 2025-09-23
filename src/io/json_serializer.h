#ifndef JSON_SERIALIZER_H
#define JSON_SERIALIZER_H

#include "../game/game_types.h"

// JSON序列化函数声明
void save_game_dump(const char* filename);
int load_game_preset(const char* filename);

#endif // JSON_SERIALIZER_H