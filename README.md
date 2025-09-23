# 大富翁游戏 - 协作开发版

## 🎯 项目简介

基于C语言开发的大富翁游戏，采用模块化设计，支持多人协作开发和TDD（测试驱动开发）。

## 📁 项目结构

```
Richman_2/
├── src/                          # 源代码目录
│   ├── main.c                    # 主程序入口（稳定，极少修改）
│   ├── game/                     # 游戏逻辑模块
│   │   ├── game_types.h          # 核心数据结构定义
│   │   ├── game_state.h/c        # 游戏状态管理
│   │   └── player.h/c            # 玩家管理
│   ├── io/                       # 输入输出模块
│   │   ├── command_processor.h/c # 命令行处理
│   │   └── json_serializer.h/c   # JSON序列化
│   └── utils/                    # 工具函数模块
│       └── string_utils.h/c      # 字符串工具
├── tests/                        # 测试目录
│   ├── integration/              # 集成测试用例
│   │   └── test1/                # 测试用例示例
│   │       ├── cmdlist.txt       # 命令输入
│   │       ├── expected_output.txt # 期望输出
│   │       ├── expected_dump.json # 期望JSON状态
│   │       ├── preset.json       # 初始状态
│   │       ├── output.txt        # 实际输出（自动生成）
│   │       └── dump.json         # 实际JSON状态（自动生成）
│   └── scripts/                  # 测试脚本
│       └── run_integration_tests.py # 自动化集成测试
├── Makefile                      # 构建脚本
├── README.md                     # 项目说明
├── DEVELOPMENT_GUIDE.md          # 详细开发指南
└── QUICK_REFERENCE.md            # 快速参考
```

## 🚀 快速开始

### 编译游戏
```bash
make              # 编译游戏程序
make clean        # 清理构建文件
```

### 运行游戏
```bash
./richman         # 启动游戏
```

### 运行测试
```bash
make test         # 运行所有集成测试
make integration_test  # 运行集成测试
```

## 🎮 游戏命令

- `create_player <姓名> <资金>` - 创建玩家
- `status` - 显示游戏状态
- `dump <文件名>` - 保存游戏状态到JSON文件
- `load <文件名>` - 从JSON文件加载游戏状态
- `quit` - 退出游戏

## 🤝 协作开发指南

### 模块分工

#### 🎯 游戏逻辑模块 (`src/game/`)
- **负责人**: 可分配给专注游戏逻辑的开发者
- **文件**: `player.c`, `game_state.c`, 以及未来的 `dice.c`, `board.c`, `property.c` 等
- **职责**: 游戏核心逻辑、玩家管理、棋盘管理、道具系统等
- **接口**: 通过头文件定义清晰的函数接口

#### 📥 输入输出模块 (`src/io/`)
- **负责人**: 可分配给专注UI/交互的开发者
- **文件**: `command_processor.c`, `json_serializer.c`
- **职责**: 命令行处理、JSON序列化、文件读写等
- **接口**: 提供命令处理和数据序列化接口

#### 🔧 工具模块 (`src/utils/`)
- **负责人**: 可分配给任何开发者
- **文件**: `string_utils.c`, 以及未来的其他工具函数
- **职责**: 通用工具函数、辅助功能
- **接口**: 提供可复用的工具函数

### 协作原则

#### 1. **主程序稳定性**
- `src/main.c` 保持极简，基本不修改
- 所有功能通过模块接口调用
- 避免直接修改主程序逻辑

#### 2. **接口优先**
- 修改功能前先确定接口（头文件）
- 接口变更需要团队讨论
- 保持向后兼容性

#### 3. **独立开发**
- 每个模块可以独立开发和测试
- 通过头文件约定接口
- 减少模块间的直接依赖

#### 4. **集成测试保障**
- 每个功能都要有对应的集成测试用例
- 提交代码前必须通过所有测试
- 测试用例作为功能规范

## 🧪 测试驱动开发

### 集成测试用例结构

每个测试用例包含以下文件：
- `cmdlist.txt` - 命令序列
- `expected_output.txt` - 期望的控制台输出
- `expected_dump.json` - 期望的游戏状态JSON
- `preset.json` - 初始游戏状态（可选）

### 创建新测试用例

```bash
# 使用Makefile自动创建测试模板
make create_test

# 手动创建
mkdir tests/integration/test2
# 然后创建相应的文件
```

### 测试流程

1. **编写测试用例** - 定义期望的行为
2. **运行测试** - `make test`（应该失败）
3. **实现功能** - 让测试通过
4. **重构优化** - 保持测试通过

## 📊 JSON数据格式

游戏状态使用JSON格式序列化，结构如下：

```json
{
    "players": [
        {
            "index": 0,
            "name": "玩家名称",
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
    "houses": {
        "位置索引": {
            "owner": "所有者名称",
            "level": 1
        }
    },
    "placed_prop": {
        "bomb": [位置列表],
        "barrier": [位置列表]
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

## 🔄 开发工作流

### 添加新功能
1. 在对应模块中添加函数声明（.h文件）
2. 实现函数（.c文件）
3. 创建集成测试用例
4. 运行测试确保功能正确
5. 提交代码

### 修改现有功能
1. 先编写测试用例验证新行为
2. 修改实现代码
3. 确保所有测试通过
4. 提交代码

## 🛠️ 扩展指南

### 添加新模块
1. 在 `src/` 下创建新目录
2. 创建对应的 `.h` 和 `.c` 文件
3. 更新 `Makefile` 中的源文件列表
4. 添加相应的测试用例

### 添加新命令
1. 在 `command_processor.c` 中添加命令处理逻辑
2. 更新帮助信息
3. 创建测试用例验证命令功能

## 📝 提交规范

- 提交前运行 `make test` 确保所有测试通过
- 提交信息要清晰描述修改内容
- 大的功能变更要先讨论接口设计
- 保持代码风格一致

## 📚 开发文档

- **[开发指南](DEVELOPMENT_GUIDE.md)** - 详细的开发流程和规范
- **[快速参考](QUICK_REFERENCE.md)** - 常用命令和代码模板

## 🎉 优势总结

1. **模块化设计** - 支持并行开发，减少冲突
2. **接口稳定** - 主程序和接口极少变动
3. **测试保障** - 集成测试确保功能正确性
4. **易于扩展** - 新功能可以独立添加
5. **协作友好** - 清晰的分工和开发流程
6. **文档完善** - 详细的开发指南和快速参考