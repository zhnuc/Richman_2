#!/usr/bin/env python3
"""
更新所有启动测试用例的expected_output.txt文件
现在使用preset.json配置，输出应该是：
大富翁游戏启动
使用预设配置
> 游戏状态已保存到: dump.json
> 游戏结束
"""

import os
from pathlib import Path

def update_expected_output(test_num):
    """更新测试用例的expected_output.txt文件"""
    test_dir = Path(f"tests/integration/test_startup_{test_num:03d}")
    expected_output_file = test_dir / "expected_output.txt"
    
    if not expected_output_file.exists():
        print(f"⚠️  test_startup_{test_num:03d} 的 expected_output.txt 不存在")
        return
    
    # 新的期望输出内容
    new_content = """大富翁游戏启动
使用预设配置
> 游戏状态已保存到: dump.json
> 游戏结束"""
    
    # 写入文件
    with open(expected_output_file, 'w', encoding='utf-8') as f:
        f.write(new_content)
    
    print(f"✅ 已更新 test_startup_{test_num:03d} 的 expected_output.txt")

def main():
    """更新所有启动测试用例的expected_output.txt文件"""
    print("📝 开始更新 expected_output.txt 文件...")
    
    for i in range(1, 21):  # test_startup_001 到 test_startup_020
        update_expected_output(i)
    
    print("✅ 所有启动测试用例的 expected_output.txt 更新完成")

if __name__ == "__main__":
    main()