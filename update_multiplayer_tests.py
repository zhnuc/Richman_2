#!/usr/bin/env python3
"""
更新多角色选择测试用例的期望输出
"""

import os
from pathlib import Path

def update_multiplayer_test(test_num, character_names):
    """更新多角色测试用例的期望输出"""
    test_dir = Path(f"tests/integration/test_startup_{test_num:03d}")
    expected_output_file = test_dir / "expected_output.txt"
    
    if not expected_output_file.exists():
        return
    
    # 读取当前内容
    with open(expected_output_file, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # 构建新的角色选择行
    character_list = ", ".join(character_names)
    new_line = f"请输入选择 (1-4): 您选择了: {character_list}"
    
    # 替换角色选择行
    lines = content.split('\n')
    for i, line in enumerate(lines):
        if "您选择了:" in line:
            lines[i] = new_line
            break
    
    # 写回文件
    with open(expected_output_file, 'w', encoding='utf-8') as f:
        f.write('\n'.join(lines))
    
    print(f"✅ 已更新 test_startup_{test_num:03d}")

def main():
    """更新所有多角色测试用例"""
    
    # 多角色组合测试用例
    multi_player_tests = [
        (16, ["钱夫人", "阿土伯"]),      # test_startup_016 - 角色组合12
        (17, ["钱夫人", "孙小美"]),      # test_startup_017 - 角色组合13
        (18, ["钱夫人", "金贝贝"]),      # test_startup_018 - 角色组合14
        (19, ["阿土伯", "孙小美"]),      # test_startup_019 - 角色组合23
        (20, ["阿土伯", "金贝贝"]),      # test_startup_020 - 角色组合24
    ]
    
    for test_num, character_names in multi_player_tests:
        update_multiplayer_test(test_num, character_names)
    
    print("✅ 所有多角色测试用例已更新")

if __name__ == "__main__":
    main()