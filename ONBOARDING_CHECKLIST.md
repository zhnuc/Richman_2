# 开发者入门检查清单

## ✅ 环境准备

- [ ] 安装GCC编译器 (`gcc --version`)
- [ ] 安装Python 3 (`python3 --version`)
- [ ] 安装Git (`git --version`)
- [ ] 克隆项目代码
- [ ] 成功编译游戏 (`make`)
- [ ] 成功运行游戏 (`./richman`)
- [ ] 成功运行测试 (`make test`)

## ✅ 项目理解

- [ ] 阅读 [README.md](README.md) 了解项目概述
- [ ] 阅读 [DEVELOPMENT_GUIDE.md](DEVELOPMENT_GUIDE.md) 了解开发流程
- [ ] 阅读 [QUICK_REFERENCE.md](QUICK_REFERENCE.md) 熟悉常用命令
- [ ] 理解项目模块化结构
- [ ] 理解集成测试框架
- [ ] 理解JSON数据格式

## ✅ 代码熟悉

- [ ] 查看 `src/main.c` 了解程序入口
- [ ] 查看 `src/game/game_types.h` 了解数据结构
- [ ] 查看 `src/game/character.h/c` 了解角色系统
- [ ] 查看 `src/game/player.h/c` 了解玩家管理
- [ ] 查看 `src/io/command_processor.h/c` 了解命令处理
- [ ] 查看 `src/io/json_serializer.h/c` 了解JSON序列化
- [ ] 查看 `Makefile` 了解构建过程

## ✅ 测试理解

- [ ] 查看 `tests/integration/test1/` 了解测试用例结构
- [ ] 查看 `tests/integration/test2/` 了解不同测试场景
- [ ] 查看 `tests/scripts/run_integration_tests.py` 了解测试脚本
- [ ] 理解cmdlist.txt的作用
- [ ] 理解expected_output.txt的作用
- [ ] 理解expected_dump.json的作用
- [ ] 理解dump命令的工作原理

## ✅ 实际操作

- [ ] 手动运行游戏并选择不同角色
- [ ] 使用status命令查看游戏状态
- [ ] 使用dump命令保存游戏状态
- [ ] 创建新的测试用例
- [ ] 修改代码并验证测试通过
- [ ] 使用Git创建功能分支
- [ ] 提交代码并推送分支

## ✅ 团队协作

- [ ] 了解模块分工原则
- [ ] 了解代码审查流程
- [ ] 了解冲突解决策略
- [ ] 了解提交规范
- [ ] 加入团队沟通群
- [ ] 了解项目进度和规划

## 🎯 第一个任务建议

完成入门检查清单后，建议从以下简单任务开始：

### 任务1: 添加新命令
- 在 `command_processor.c` 中添加 `help` 命令
- 显示所有可用命令的说明
- 创建对应的测试用例
- 确保测试通过

### 任务2: 扩展角色信息
- 为角色添加更多属性（如初始资金、特殊技能等）
- 更新JSON序列化以包含新属性
- 更新测试用例验证新功能

### 任务3: 添加工具函数
- 在 `utils/` 中添加数学工具函数
- 如随机数生成、数值计算等
- 创建简单的测试验证功能

## 📞 需要帮助时

- 查看 [DEVELOPMENT_GUIDE.md](DEVELOPMENT_GUIDE.md) 的常见问题部分
- 检查现有代码实现作为参考
- 在团队群中提问
- 查看Git提交历史了解开发过程

## 🎉 完成标志

当你能够：
- [ ] 独立编译和运行项目
- [ ] 理解代码结构和模块分工
- [ ] 创建和运行测试用例
- [ ] 修改代码并验证功能
- [ ] 使用Git进行版本控制

恭喜！你已经准备好参与大富翁游戏的协作开发了！

---

**记住**: 不要害怕犯错，测试框架会帮你发现问题。保持代码简洁，测试完整！