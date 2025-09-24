#!/bin/bash
# 安全推送脚本 - 确保测试通过后再推送

set -e

TARGET_BRANCH=${1:-main}
CURRENT_BRANCH=$(git symbolic-ref --short HEAD)

echo "🚀 安全推送脚本"
echo "当前分支: $CURRENT_BRANCH"
echo "目标分支: $TARGET_BRANCH"
echo ""

# 如果是推送到主分支，强制运行测试
if [ "$TARGET_BRANCH" = "main" ]; then
    echo "🔍 推送到主分支，运行完整测试检查..."
    
    # 检查是否有未提交的更改
    if ! git diff-index --quiet HEAD --; then
        echo "⚠️  检测到未提交的更改，请先提交："
        git status --porcelain
        exit 1
    fi
    
    # 运行测试
    echo "🧪 运行敏捷测试..."
    make test
    
    if [ $? -eq 0 ]; then
        echo "✅ 测试通过！"
    else
        echo "❌ 测试失败！请修复后重试。"
        echo ""
        echo "调试建议："
        echo "  make test_status    # 查看测试状态"
        echo "  make integration_test # 运行完整测试"
        exit 1
    fi
    
    # 拉取最新代码
    echo "📥 拉取最新代码..."
    git pull origin $TARGET_BRANCH
    
    # 推送
    echo "📤 推送到远程仓库..."
    git push origin $TARGET_BRANCH
    
    echo "🎉 推送成功！"
else
    # 非主分支直接推送
    echo "📤 推送到分支 $TARGET_BRANCH..."
    git push origin $TARGET_BRANCH
    echo "✅ 推送完成！"
fi
