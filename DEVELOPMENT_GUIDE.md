# Rich 大富翁游戏开发指南

## 📋 目录
- [快速开始](#快速开始)
- [项目结构](#项目结构)
- [开发环境设置](#开发环境设置)
- [协作开发流程](#协作开发流程)
- [模块开发指南](#模块开发指南)
- [测试驱动开发](#测试驱动开发)
- [代码规范](#代码规范)
- [常见问题](#常见问题)

## 🚀 快速开始

### 1. 克隆项目
```bash
git clone <repository-url>
cd Richman_2
```

### 2. 编译游戏
```bash
make              # 编译游戏
make clean        # 清理构建文件
```

### 3. 运行游戏
```bash
./rich            # 启动游戏
```

### 4. 运行测试
```bash
make test         # 运行所有集成测试
```

## 📁 项目结构

```
Richman_2/
├── src/                          # 源代码目录
│   ├── main.c                    # 主程序入口（稳定，极少修改）
│   ├── game/                     # 游戏逻辑模块
│   │   ├── game_types.h          # 核心数据结构定义
│   │   ├── game_state.h/c        # 游戏状态管理
│   │   ├── player.h/c            # 玩家管理
│   │   └── character.h/c         # 角色管理
│   ├── io/                       # 输入输出模块
│   │   ├── command_processor.h/c # 命令行处理
│   │   └── json_serializer.h/c   # JSON序列化
│   └── utils/                    # 工具函数模块
│       └── string_utils.h/c      # 字符串工具
├── tests/                        # 测试目录
│   ├── integration/              # 集成测试用例
│   │   ├── test_startup_001/     # 启动测试用例1（钱夫人）
│   │   ├── test_startup_002/     # 启动测试用例2（多角色）
│   │   ├── test_startup_003/     # 启动测试用例3（三角色）
│   │   ├── test_startup_004/     # 启动测试用例4（四角色）
│   │   ├── test_startup_005/     # 启动测试用例5（无效输入）
│   │   ├── test_startup_006/     # preset初始化测试1（单玩家）
│   │   └── test_startup_007/     # preset初始化测试2（多玩家）
│   └── scripts/                  # 测试脚本
│       └── run_integration_tests.py # 自动化集成测试
├── Makefile                      # 构建脚本
├── README.md                     # 项目说明
└── DEVELOPMENT_GUIDE.md          # 开发指南（本文件）
```

## 🛠️ 开发环境设置

### 必需工具
- **GCC编译器** (支持C99标准)
- **Python 3** (用于集成测试脚本)
- **Git** (版本控制)
- **文本编辑器** (推荐VS Code, Vim, Emacs等)

### 环境检查
```bash
# 检查编译器
gcc --version

# 检查Python
python3 --version

# 检查Git
git --version
```

## 🤝 协作开发流程

### 1. 分支管理
```bash
# 创建功能分支
git checkout -b feature/dice-system

# 开发完成后提交
git add .
git commit -m "feat: 添加骰子系统"

# 推送到远程
git push origin feature/dice-system
```

### 2. 代码审查
- 所有代码必须通过集成测试
- 提交前运行 `make test`
- 代码审查通过后才能合并到主分支

### 3. 冲突解决
- 优先修改模块内部实现
- 避免修改接口文件（.h文件）
- 如有接口变更，需要团队讨论

## 🎯 模块开发指南

### 游戏逻辑模块 (`src/game/`)

#### 添加新功能
1. **定义数据结构** (在 `game_types.h` 中)
```c
typedef struct {
    int value;
    char name[32];
} NewFeature;
```

2. **创建头文件** (如 `new_feature.h`)
```c
#ifndef NEW_FEATURE_H
#define NEW_FEATURE_H

#include "game_types.h"

// 函数声明
NewFeature* create_new_feature(int value, const char* name);
void destroy_new_feature(NewFeature* feature);

#endif
```

3. **实现功能** (在 `new_feature.c` 中)
```c
#include "new_feature.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

NewFeature* create_new_feature(int value, const char* name) {
    NewFeature* feature = malloc(sizeof(NewFeature));
    if (!feature) return NULL;
    
    feature->value = value;
    strncpy(feature->name, name, sizeof(feature->name) - 1);
    feature->name[sizeof(feature->name) - 1] = '\0';
    
    return feature;
}

void destroy_new_feature(NewFeature* feature) {
    if (feature) {
        free(feature);
    }
}
```

#### 现有模块说明

**角色管理 (`character.h/c`)**
- 管理4个预设角色（钱夫人、阿土伯、孙小美、金贝贝）
- 提供角色选择和验证功能

**玩家管理 (`player.h/c`)**
- 玩家创建、管理和验证
- 支持通过角色ID创建玩家

**游戏状态 (`game_state.h/c`)**
- 全局游戏状态管理
- 游戏初始化和状态显示

### 输入输出模块 (`src/io/`)

#### 添加新命令
1. **在 `command_processor.c` 中添加命令处理**
```c
} else if (strcmp(command, "new_command") == 0) {
    // 处理新命令
    printf("执行新命令\n");
} else {
```

2. **更新帮助信息**
```c
printf("可用命令: ..., new_command, ...\n");
```

#### 现有模块说明

**命令处理器 (`command_processor.h/c`)**
- 处理所有游戏命令
- 角色选择界面
- 游戏主循环

**JSON序列化 (`json_serializer.h/c`)**
- 游戏状态保存和加载
- 支持dump和load命令

### 工具模块 (`src/utils/`)

#### 添加工具函数
1. **在现有文件中添加** (如 `string_utils.c`)
2. **或创建新文件** (如 `math_utils.h/c`)

## 🧪 测试驱动开发

### 创建新测试用例

#### 1. 创建测试目录
```bash
mkdir tests/integration/test_new_feature
```

#### 2. 创建测试文件
```bash
# cmdlist.txt - 命令序列
echo "10000
1
dump
quit" > tests/integration/test_new_feature/cmdlist.txt

# expected_output.txt - 期望输出
echo "大富翁游戏启动
请输入初始资金: 欢迎来到大富翁，请按数字键选择你的角色：
1.钱夫人
2.阿土伯
3.孙小美
4.金贝贝
请输入选择 (1-4): 您选择了: 钱夫人

游戏开始！
> 游戏状态已保存到: dump.json
> 游戏结束" > tests/integration/test_new_feature/expected_output.txt

# expected_dump.json - 期望的JSON状态
echo '{
    "players": [
        {
            "index": 0,
            "name": "Q",
            "fund": 10000,
            "credit": 0,
            "location": 0,
            "alive": true,
            "prop": {
                "bomb": 0,
                "barrier": 0,
                "robot": 0,
                "total": 0
            },
            "buff": {
                "god": 0,
                "prison": 0,
                "hospital": 0
            }
        }
    ],
    "houses": {
    },
    "placed_prop": {
        "bomb": [],
        "barrier": []
    },
    "game": {
        "now_player": 0,
        "next_player": 1,
        "started": false,
        "ended": false,
        "winner": -1
    }
}' > tests/integration/test_new_feature/expected_dump.json
```

#### 3. 运行测试
```bash
make test
```

### TDD开发流程

#### 1. 红 (Red) - 编写失败的测试
```bash
# 创建测试用例，期望新功能存在
# 运行测试，应该失败
make test
```

#### 2. 绿 (Green) - 实现功能让测试通过
```c
// 实现最小功能让测试通过
int new_function(void) {
    return 42;  // 最简单的实现
}
```

#### 3. 重构 (Refactor) - 优化代码
```c
// 优化实现，保持测试通过
int new_function(void) {
    // 更好的实现
    return calculate_complex_value();
}
```

## 📝 代码规范

### 命名规范
- **函数名**: `snake_case` (如 `create_player`)
- **变量名**: `snake_case` (如 `player_count`)
- **常量名**: `UPPER_CASE` (如 `MAX_PLAYERS`)
- **文件名**: `snake_case` (如 `game_state.c`)

### 代码风格
```c
// 好的示例
Player* create_player_by_character(int character_id, int fund) {
    if (!is_valid_character_id(character_id) || g_game_state.player_count >= 4) {
        return NULL;
    }
    
    Character* character = get_character_by_id(character_id);
    if (!character) {
        return NULL;
    }
    
    Player* player = &g_game_state.players[g_game_state.player_count];
    player->index = g_game_state.player_count;
    strncpy(player->name, character->name, sizeof(player->name) - 1);
    player->name[sizeof(player->name) - 1] = '\0';
    
    return player;
}
```

### 注释规范
```c
/**
 * 创建玩家
 * @param character_id 角色ID (1-4)
 * @param fund 初始资金
 * @return 创建的玩家指针，失败返回NULL
 */
Player* create_player_by_character(int character_id, int fund);
```

### 错误处理
```c
// 总是检查返回值
Player* player = create_player_by_character(1, 1500);
if (!player) {
    printf("创建玩家失败\n");
    return -1;
}

// 使用断言进行调试
#include <assert.h>
assert(player != NULL);
```

## 🔧 常用命令

### 开发命令
```bash
# 编译
make                    # 编译游戏
make clean             # 清理构建文件

# 测试
make test              # 运行所有测试
make integration_test  # 只运行集成测试

# 游戏
./rich                # 启动游戏
```

### Git命令
```bash
# 分支管理
git checkout -b feature/new-feature    # 创建功能分支
git checkout main                      # 切换到主分支

# 提交
git add .                              # 添加所有更改
git commit -m "feat: 添加新功能"        # 提交更改
git push origin feature/new-feature    # 推送分支

# 合并
git checkout main                      # 切换到主分支
git merge feature/new-feature          # 合并功能分支
```

## 🐛 常见问题

### Q: 编译错误 "undefined reference"
**A**: 检查Makefile是否包含了新的.c文件
```bash
# 查看当前源文件
make -n richman
```

### Q: 测试失败 "控制台输出不匹配"
**A**: 检查expected_output.txt是否与实际输出一致
```bash
# 查看实际输出
cat tests/integration/test1/output.txt
```

### Q: JSON格式错误
**A**: 使用JSON验证器检查格式
```bash
# 验证JSON文件
python3 -m json.tool tests/integration/test1/dump.json
```

### Q: 角色选择无效
**A**: 确保输入的是1-4之间的数字
```bash
# 测试角色选择
echo -e "10000\n1" | ./rich
```

### Q: dump命令不工作
**A**: 确保cmdlist.txt中只有`dump`，没有额外参数
```bash
# 正确的cmdlist.txt格式
echo "10000
1
dump
quit" > cmdlist.txt
```

## 📚 扩展阅读

### 游戏功能规划
- [ ] 骰子系统 (`dice.h/c`)
- [ ] 棋盘管理 (`board.h/c`)
- [ ] 房产系统 (`property.h/c`)
- [ ] 道具系统 (`props.h/c`)
- [ ] 回合管理 (`turn.h/c`)
- [ ] 获胜条件 (`win_condition.h/c`)

### 技术栈
- **语言**: C99
- **构建**: Make
- **测试**: Python集成测试
- **版本控制**: Git
- **文档**: Markdown

### 参考资源
- [C语言编程规范](https://www.gnu.org/prep/standards/html_node/Writing-C.html)
- [Makefile教程](https://makefiletutorial.com/)
- [Git工作流](https://www.atlassian.com/git/tutorials/comparing-workflows)

## 🆘 获取帮助

### 团队协作
- 遇到问题先在团队群中讨论
- 复杂功能设计需要团队评审
- 定期进行代码审查

### 技术支持
- 查看项目README.md
- 检查现有测试用例
- 参考现有代码实现

---

**记住**: 保持代码简洁、测试完整、文档清晰。协作开发的关键是沟通和规范！