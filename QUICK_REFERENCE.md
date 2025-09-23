# 大富翁游戏开发快速参考

## 🚀 常用命令

```bash
# 编译和运行
make                    # 编译游戏
make clean             # 清理构建文件
./richman              # 启动游戏

# 测试
make test              # 运行所有集成测试
make integration_test  # 只运行集成测试

# 创建测试用例
mkdir tests/integration/test_new
# 然后创建 cmdlist.txt, expected_output.txt, expected_dump.json
```

## 🎮 游戏命令

| 命令 | 说明 | 示例 |
|------|------|------|
| `1-4` | 选择角色 | `1` (钱夫人) |
| `status` | 显示游戏状态 | `status` |
| `dump` | 保存为dump.json | `dump` |
| `dump <文件>` | 保存为指定文件 | `dump save.json` |
| `quit` | 退出游戏 | `quit` |

## 📁 模块分工

| 模块 | 负责人 | 文件 | 职责 |
|------|--------|------|------|
| **游戏逻辑** | 开发者A | `src/game/` | 核心游戏逻辑 |
| **输入输出** | 开发者B | `src/io/` | 命令处理、JSON |
| **工具函数** | 开发者C | `src/utils/` | 通用工具 |
| **主程序** | 所有人 | `src/main.c` | 保持稳定 |

## 🧪 测试用例结构

```
tests/integration/test_name/
├── cmdlist.txt           # 命令序列
├── expected_output.txt   # 期望控制台输出
├── expected_dump.json    # 期望JSON状态
├── preset.json          # 初始状态（可选）
├── output.txt           # 实际输出（自动生成）
└── dump.json            # 实际JSON状态（自动生成）
```

## 📝 代码规范

### 命名规范
- 函数: `create_player()`
- 变量: `player_count`
- 常量: `MAX_PLAYERS`
- 文件: `game_state.c`

### 头文件模板
```c
#ifndef MODULE_NAME_H
#define MODULE_NAME_H

#include "game_types.h"

// 函数声明
ReturnType function_name(ParamType param);

#endif
```

### 源文件模板
```c
#include "module_name.h"
#include <stdio.h>
#include <stdlib.h>

ReturnType function_name(ParamType param) {
    // 实现
    return result;
}
```

## 🔧 开发流程

1. **创建分支**: `git checkout -b feature/new-feature`
2. **编写测试**: 创建测试用例
3. **实现功能**: 让测试通过
4. **运行测试**: `make test`
5. **提交代码**: `git commit -m "feat: 描述"`
6. **推送分支**: `git push origin feature/new-feature`

## 🐛 常见问题

| 问题 | 解决方案 |
|------|----------|
| 编译错误 | 检查Makefile是否包含新文件 |
| 测试失败 | 对比expected_output.txt和output.txt |
| JSON错误 | 使用`python3 -m json.tool file.json`验证 |
| 角色选择无效 | 确保输入1-4之间的数字 |

## 📊 JSON数据结构

```json
{
    "players": [
        {
            "index": 0,
            "name": "钱夫人",
            "fund": 1500,
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
    "houses": {},
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
}
```

## 🎯 角色信息

| ID | 角色名 | 说明 |
|----|--------|------|
| 1 | 钱夫人 | 默认角色 |
| 2 | 阿土伯 | 默认角色 |
| 3 | 孙小美 | 默认角色 |
| 4 | 金贝贝 | 默认角色 |

## 📞 获取帮助

- 查看 `DEVELOPMENT_GUIDE.md` 获取详细指南
- 查看 `README.md` 了解项目概述
- 检查现有测试用例了解功能实现
- 团队群讨论技术问题

---

**记住**: 保持代码简洁，测试完整，文档清晰！