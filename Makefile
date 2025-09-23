# 大富翁游戏 Makefile
# 支持渐进式开发和测试

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
SRC_DIR = src
TEST_DIR = tests
BUILD_DIR = build

# 自动发现源文件
GAME_SOURCES = $(wildcard $(SRC_DIR)/game/*.c)
IO_SOURCES = $(wildcard $(SRC_DIR)/io/*.c)
UTILS_SOURCES = $(wildcard $(SRC_DIR)/utils/*.c)
MAIN_SRC = $(SRC_DIR)/main.c

# 所有模块源文件
MODULE_SOURCES = $(GAME_SOURCES) $(IO_SOURCES) $(UTILS_SOURCES)
ALL_SOURCES = $(MODULE_SOURCES) $(MAIN_SRC)

# 目标文件
RICHMAN_BIN = rich

# 默认目标
all: $(RICHMAN_BIN)

# 编译游戏主程序
$(RICHMAN_BIN): $(ALL_SOURCES)
	@echo "🔨 编译大富翁游戏..."
	@echo "📦 模块源文件: $(MODULE_SOURCES)"
	$(CC) $(CFLAGS) -o $@ $(ALL_SOURCES)
	@echo "✅ 编译完成: $@"

# 运行测试（仅集成测试）
test: integration_test

# 运行集成测试
integration_test: $(RICHMAN_BIN)
	@echo "🔗 运行集成测试..."
	@python3 $(TEST_DIR)/scripts/run_integration_tests.py $(PWD)

# 运行所有测试（等同于集成测试）
test_all: test
	@echo "🎉 所有测试完成！"

# 清理构建文件
clean:
	@echo "🧹 清理构建文件..."
	rm -f $(RICHMAN_BIN)
	rm -f $(TEST_DIR)/integration/*/output.txt
	rm -f $(TEST_DIR)/integration/*/dump.json
	@echo "✅ 清理完成"

# 创建集成测试模板
create_test:
	@read -p "输入测试名称 (例如: test1): " test_name; \
	test_dir="$(TEST_DIR)/integration/$$test_name"; \
	if [ ! -d "$$test_dir" ]; then \
		mkdir -p "$$test_dir"; \
		echo "create_player 张三 1500" > "$$test_dir/cmdlist.txt"; \
		echo "status" >> "$$test_dir/cmdlist.txt"; \
		echo "quit" >> "$$test_dir/cmdlist.txt"; \
		echo "大富翁游戏启动" > "$$test_dir/expected_output.txt"; \
		echo "> 创建玩家成功: 张三" >> "$$test_dir/expected_output.txt"; \
		echo "> === 游戏状态 ===" >> "$$test_dir/expected_output.txt"; \
		echo "玩家数量: 1" >> "$$test_dir/expected_output.txt"; \
		echo "玩家0: 张三, 资金:1500, 位置:0, 存活:是" >> "$$test_dir/expected_output.txt"; \
		echo "当前玩家: 0" >> "$$test_dir/expected_output.txt"; \
		echo "游戏状态: 未开始" >> "$$test_dir/expected_output.txt"; \
		echo "> 游戏结束" >> "$$test_dir/expected_output.txt"; \
		echo "✅ 创建测试模板: $$test_dir"; \
		echo "📝 请编辑以下文件:"; \
		echo "   - $$test_dir/cmdlist.txt (命令输入)"; \
		echo "   - $$test_dir/expected_output.txt (期望输出)"; \
	else \
		echo "❌ 测试已存在: $$test_dir"; \
	fi

# 运行游戏
run: $(RICHMAN_BIN)
	@echo "🎮 启动大富翁游戏..."
	./$(RICHMAN_BIN)

# 调试模式编译
debug: CFLAGS += -DDEBUG
debug: $(RICHMAN_BIN)

# 显示帮助
help:
	@echo "大富翁游戏 Makefile 使用说明"
	@echo "=============================="
	@echo "make              - 编译游戏主程序"
	@echo "make test         - 运行单元测试"
	@echo "make integration_test - 运行集成测试"
	@echo "make test_all     - 运行所有测试"
	@echo "make run          - 启动游戏"
	@echo "make create_test  - 创建新的集成测试模板"
	@echo "make clean        - 清理构建文件"
	@echo "make debug        - 调试模式编译"
	@echo "make help         - 显示此帮助信息"

.PHONY: all test integration_test test_all clean create_test run debug help