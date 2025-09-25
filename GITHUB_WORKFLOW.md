项目结构
Richman_2/
├── .git/hooks/pre-push           # Git pre-push hook 脚本
├── .github/workflows/autotest.yml # GitHub Actions 自动化测试
├── tests/
│   ├── scripts/run_agile_tests.py # 敏捷测试运行脚本
│   └── test_status.config        # 测试状态配置文件
├── Makefile                      # 构建和测试目标
└── 敏捷测试管理说明.md           # 敏捷测试系统说明
一、快速开始
1. 克隆仓库并设置环境
# 克隆仓库
git clone https://github.com/zhnuc/Richman_2.git
cd Richman_2

# 确保 pre-push hook 可执行
chmod +x .git/hooks/pre-push

# 安装构建依赖
sudo apt-get update && sudo apt-get install -y build-essential

# 构建项目
make clean && make
2. 验证环境配置
# 运行敏捷测试，确保环境正常
make test

# 查看测试启用状态（也可直接查看test_status.config文件）
make test_status
二、日常开发流程
步骤 1: 同步最新代码
# 切换到主分支
git checkout main

# 拉取最新代码和测试用例
git pull origin main

# 确保测试通过
make test
步骤 2: 创建功能分支
# 创建并切换到自己的个人分支
git checkout -b your-feature-name


步骤 3: 开发和测试
# 进行代码开发
# ...编辑代码...

# 本地构建测试
make clean && make

# 运行敏捷测试
make test

# 查看测试状态和覆盖率（可跳过因为终端会打印测试状态信息，也可直接查看test_status.config文件）
make test_status
步骤 4: 配置测试状态
根据开发进度，在 tests/test_status.config 中配置测试状态：
# 测试状态配置文件说明:
# active: 活跃测试，必须通过 (🟢)
# pending: 待实现功能，暂时跳过 (🔵)  
# wip: 正在开发中，允许失败 (🟡)
# disabled: 已禁用的测试 (⚫)

test_startup_001: active    # 已完成功能
test_help_001: pending      # 待实现功能
test_query_001: wip         # 正在开发
test_old_feature: disabled  # 已禁用功能
步骤 5: 提交代码
# 添加更改
git add .
# 提交更改（提交信息要清晰）
git commit -m "feat: 添加新功能描述"

# 推送到远程分支
git push origin feature/your-feature-name
三、分支保护机制
Pre-push Hook 保护
之前基于github分支保护规则以及PR的main分支保护策略需要在github创建pr，还有代码审查啥的有点麻烦，所以改成本地的hook脚本进行自动保护
项目配置了 .git/hooks/pre-push hook，在使用命令push时会自动检查目标分支并跑一遍测试，如果是推送到主分支且测试不通过则会阻塞push请求，如果是推送到别的分支则无论测试成功还是失败都不会阻塞push请求：
#!/bin/bash
# 只有推送到 main 分支时才运行测试
protected_branch='main'
current_branch=$(git symbolic-ref HEAD | sed -e 's,.*/\(.*\),\1,')

if [ "$current_branch" = "$protected_branch" ]; then
    echo "检测到推送到主分支，运行测试..."
    make test
    if [ $? -ne 0 ]; then
        echo "测试失败！禁止推送到主分支。"
        exit 1
    fi
fi
说明
在开发中跑测试时
绕过 Hook（紧急情况）
# 仅在紧急情况下使用，跳过 pre-push hook
git push --no-verify origin main
 GitHub Actions 持续集成
这个在现在开发中的作用用hook脚本代替了，大家开发测试中可以忽略，做展示的时候丰富我们组的工作用的
.github/workflows/autotest.yml配置了完整的CI/CD 流程，触发条件：Push 到任意分支或PR 到 main 分支
在推送到github上后会又自动跑一遍测试，测试全部通过则会显示绿色小勾，不通过则会显示红叉
[图片]
在action界面能够查看每次提交的测试状态与输出
[图片]
冲突解决方案

检测冲突

# 同步主分支最新代码
git checkout main
git pull origin main

# 切回功能分支
git checkout feature/your-feature-name

# 尝试合并或变基
git merge main
# 或
git rebase main

解决冲突

当出现冲突时：

# 1. 查看冲突文件
git status

# 2. 编辑冲突文件，解决冲突标记
# <<<<<<< HEAD
# 你的更改
# =======
# 主分支的更改
# >>>>>>> main

# 3. 标记冲突已解决
git add <解决的文件>

# 4. 完成合并
git commit
# 或完成变基
git rebase --continue

# 5. 运行测试确保一切正常
make test

# 6. 推送解决后的代码
git push origin feature/your-feature-name

复杂冲突处理

# 使用可视化工具解决冲突
git mergetool

# 查看冲突历史
git log --merge --oneline

# 撤销合并（如果需要）
git merge --abort
# 或撤销变基
git rebase --abort

🧪 敏捷测试最佳实践

### 测试状态管理

```bash
# 查看当前测试状态
make test_status

# 运行所有敏捷测试
make test

# 运行集成测试
make integration_test

# 标记单个测试状态
make mark_test TEST_NAME=test_example_001 STATUS=active
```

### 🆕 批量测试管理功能

#### 1. 查看所有测试状态
```bash
# 列出所有测试用例及其状态，会自动检测新测试用例
python3 tests/scripts/run_agile_tests.py . --list

# 简化命令（如果配置了alias）
make list_tests
```

#### 2. 自动发现新测试用例
当您添加新的测试用例目录时，系统会自动检测：

```bash
# 只查找未配置的新测试用例
python3 tests/scripts/run_agile_tests.py . --find-new

# 自动添加新测试用例到配置文件（默认状态：pending）
python3 tests/scripts/run_agile_tests.py . --auto-add

# 自动添加新测试用例并指定状态
python3 tests/scripts/run_agile_tests.py . --auto-add wip
```

#### 3. 批量更新测试状态
支持通配符和正则表达式的强大批量操作：

```bash
# 批量更新所有startup测试为active状态
python3 tests/scripts/run_agile_tests.py . --batch-update "test_startup_*" active

# 批量更新所有move测试为wip状态  
python3 tests/scripts/run_agile_tests.py . --batch-update "test_move_*" wip

# 使用正则表达式：批量更新前3个startup测试
python3 tests/scripts/run_agile_tests.py . --batch-update "test_startup_00[123]" active

# 禁用所有help相关测试
python3 tests/scripts/run_agile_tests.py . --batch-update "test_help_*" disabled

# 批量设置多个测试为pending状态
python3 tests/scripts/run_agile_tests.py . --batch-update "test_property_*" pending
```

#### 4. 测试人员工作流程

**新功能开发阶段：**
```bash
# 1. 添加新测试用例目录和文件
mkdir tests/integration/test_new_feature_001
# ... 创建 input.txt, expected_result.json 等文件

# 2. 自动发现并添加到配置
python3 tests/scripts/run_agile_tests.py . --auto-add pending

# 3. 开始开发时，批量标记为wip
python3 tests/scripts/run_agile_tests.py . --batch-update "test_new_feature_*" wip

# 4. 功能完成后，批量标记为active
python3 tests/scripts/run_agile_tests.py . --batch-update "test_new_feature_*" active
```

**测试状态批量调整：**
```bash
# 项目初期：大部分测试设为pending
python3 tests/scripts/run_agile_tests.py . --batch-update "test_*" pending

# 开发阶段：激活核心功能测试
python3 tests/scripts/run_agile_tests.py . --batch-update "test_startup_*" active
python3 tests/scripts/run_agile_tests.py . --batch-update "test_basic_*" active

# 临时禁用有问题的测试模块
python3 tests/scripts/run_agile_tests.py . --batch-update "test_advanced_*" disabled
```

#### 5. 支持的状态类型

| 状态 | 图标 | 说明 | 行为 |
|------|------|------|------|
| `active` | 🟢 | 活跃测试，必须通过 | 运行测试，失败则报错 |
| `wip` | 🟡 | 开发中的功能 | 运行测试，允许失败 |
| `pending` | 🔵 | 待实现功能 | 跳过测试，显示提醒 |
| `disabled` | ⚫ | 已禁用的测试 | 完全跳过 |

#### 6. 高级模式匹配

```bash
# 通配符匹配
"test_*"           # 匹配所有测试
"test_startup_*"   # 匹配所有startup测试
"*_001"           # 匹配所有结尾为_001的测试

# 正则表达式匹配
"test_startup_00[1-3]"     # 匹配 001, 002, 003
"test_(move|property)_*"   # 匹配move或property测试
"test_startup_0[0-9][13579]" # 匹配奇数编号的startup测试
```

测试驱动开发 (TDD)

1. 编写失败测试：先写测试，标记为 `wip`
2. 实现功能：编写最少代码让测试通过  
3. 重构优化：改进代码质量
4. 标记完成：将测试状态改为 `active`

测试状态转换

pending (🔵) → wip (🟡) → active (🟢)
     ↓           ↓          ↓
  待实现      开发中      已完成

🔧 常用命令总结

Git 操作

# 基本操作
git status                    # 查看状态
git log --oneline -10        # 查看提交历史
git branch -a               # 查看所有分支
git remote -v               # 查看远程仓库

# 分支操作
git checkout -b new-branch   # 创建并切换分支
git branch -d old-branch     # 删除本地分支
git push origin --delete old-branch  # 删除远程分支

# 同步操作
git fetch origin            # 获取远程更新
git pull origin main        # 拉取并合并主分支
git push origin feature     # 推送功能分支

构建和测试

# 构建操作
make clean                  # 清理构建文件
make                       # 构建项目
make rebuild               # 重新构建

# 测试操作
make test                  # 运行敏捷测试
make test_status          # 查看测试状态
make integration_test     # 运行集成测试

🆘 故障排除

常见问题

1. Hook 脚本不执行
chmod +x .git/hooks/pre-push

2. 测试失败但不知原因
python3 tests/scripts/run_agile_tests.py . --verbose

3. 构建失败
make clean
sudo apt-get install build-essential
make

4. CI 一直失败
- 检查 .github/workflows/autotest.yml
- 确保本地 make test 通过
- 查看 GitHub Actions 详细日志
获取帮助

# 查看 Makefile 所有目标
make help

# 查看敏捷测试脚本帮助
python3 tests/scripts/run_agile_tests.py --help

# 查看 Git 帮助
git help <command>
