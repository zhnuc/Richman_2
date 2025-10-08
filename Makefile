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

# 运行测试（敏捷模式，只运行active和wip状态的测试）
test: agile_test

# 运行敏捷测试（智能跳过pending测试）
agile_test: $(RICHMAN_BIN)
	@echo "🔗 运行敏捷集成测试..."
	@python3 $(TEST_DIR)/scripts/run_agile_tests.py $(PWD)

# 运行传统集成测试（运行所有测试，包括会失败的）
integration_test: $(RICHMAN_BIN)
	@echo "🔗 运行传统集成测试..."
	@python3 $(TEST_DIR)/scripts/run_integration_tests.py $(PWD)

# 运行所有测试（等同于敏捷测试）
test_all: agile_test
	@echo "🎉 敏捷测试完成！"

# 测试管理命令
test_status:
	@echo "📋 当前测试状态配置:"
	@cat $(TEST_DIR)/test_status.config 2>/dev/null || echo "未找到测试状态配置文件"

# 将测试标记为pending（待实现）
mark_pending:
	@echo "请使用: make mark_test TEST=test_name STATUS=pending"

# 通用测试状态标记
mark_test:
	@if [ -z "$(TEST)" ] || [ -z "$(STATUS)" ]; then \
		echo "用法: make mark_test TEST=test_name STATUS=active|pending|disabled|wip"; \
		echo "示例: make mark_test TEST=test_help_001 STATUS=pending"; \
	else \
		echo "🏷️  标记测试 $(TEST) 为 $(STATUS)"; \
		sed -i "/^$(TEST):/d" $(TEST_DIR)/test_status.config 2>/dev/null || true; \
		echo "$(TEST): $(STATUS)" >> $(TEST_DIR)/test_status.config; \
		sort -o $(TEST_DIR)/test_status.config $(TEST_DIR)/test_status.config; \
	fi

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

# 批量测试管理 - 列出所有测试及状态
list_tests:
	@echo "📋 列出所有测试用例及其状态..."
	@python3 $(TEST_DIR)/scripts/run_agile_tests.py $(PWD) --list

# 批量更新测试状态
batch_update:
	@if [ -z "$(PATTERN)" ] || [ -z "$(STATUS)" ]; then \
		echo "用法: make batch_update PATTERN=pattern STATUS=status"; \
		echo "示例: make batch_update PATTERN=test_help STATUS=active"; \
		echo "状态选项: active, wip, pending, disabled"; \
		exit 1; \
	fi
	@echo "🔄 批量更新测试状态: $(PATTERN) -> $(STATUS)"
	@python3 $(TEST_DIR)/scripts/run_agile_tests.py $(PWD) --batch-update $(PATTERN) $(STATUS)

# 自动添加新测试用例
auto_add_tests:
	@echo "🔍 自动发现并添加新测试用例..."
	@python3 $(TEST_DIR)/scripts/run_agile_tests.py $(PWD) --auto-add $(or $(STATUS),pending)

# 查找新测试用例（不添加）
find_new_tests:
	@echo "🔍 查找未配置的新测试用例..."
	@python3 $(TEST_DIR)/scripts/run_agile_tests.py $(PWD) --find-new

# 禁用所有测试用例
disable_all_tests:
	@echo "🚫 禁用所有测试用例..."
	@$(MAKE) batch_update PATTERN=test_ STATUS=disabled

# 显示帮助
help:
	@echo "大富翁游戏 Makefile 使用说明"
	@echo "=============================="
	@echo "📦 编译和运行:"
	@echo "make              - 编译游戏主程序"
	@echo "make run          - 启动游戏"
	@echo "make debug        - 调试模式编译"
	@echo "make clean        - 清理构建文件"
	@echo ""
	@echo "🧪 测试管理:"
	@echo "make test         - 运行敏捷测试（active+wip状态）"
	@echo "make integration_test - 运行传统集成测试（所有测试）"
	@echo "make test_all     - 运行所有测试"
	@echo "make create_test  - 创建新的集成测试模板"
	@echo ""
	@echo "📋 测试状态管理:"
	@echo "make test_status  - 查看测试状态配置"
	@echo "make list_tests   - 列出所有测试用例及状态"
	@echo "make mark_test TEST=name STATUS=status - 标记单个测试状态"
	@echo ""
	@echo "🔄 批量测试管理:"
	@echo "make batch_update PATTERN=pattern STATUS=status - 批量更新测试状态"
	@echo "make auto_add_tests [STATUS=status] - 自动添加新测试用例"
	@echo "make find_new_tests - 查找新测试用例（不添加）"
	@echo "make disable_all_tests - 禁用所有测试用例"
	@echo ""
	@echo "💡 示例:"
	@echo "make batch_update PATTERN=test_startup* STATUS=active"
	@echo "make auto_add_tests STATUS=active"
	@echo "make mark_test TEST=test_help_00{1,2,5,6} STATUS=active"

.PHONY: all test integration_test test_all clean create_test run debug help \
        list_tests batch_update auto_add_tests find_new_tests disable_all_tests