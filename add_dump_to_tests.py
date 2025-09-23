#!/usr/bin/env python3
"""
为所有启动测试用例添加dump命令
"""

import os
from pathlib import Path

def add_dump_command(test_num):
    """为测试用例添加dump命令"""
    test_dir = Path(f"tests/integration/test_startup_{test_num:03d}")
    cmdlist_file = test_dir / "cmdlist.txt"
    
    if not cmdlist_file.exists():
        return
    
    # 读取当前内容
    with open(cmdlist_file, 'r', encoding='utf-8') as f:
        content = f.read().strip()
    
    # 在quit之前添加dump命令
    lines = content.split('\n')
    if 'quit' in lines:
        quit_index = lines.index('quit')
        lines.insert(quit_index, 'dump')
    
    # 写回文件
    with open(cmdlist_file, 'w', encoding='utf-8') as f:
        f.write('\n'.join(lines))
    
    print(f"✅ 已为 test_startup_{test_num:03d} 添加dump命令")

def main():
    """为所有启动测试用例添加dump命令"""
    
    for i in range(1, 21):  # test_startup_001 到 test_startup_020
        add_dump_command(i)
    
    print("✅ 所有启动测试用例已添加dump命令")

if __name__ == "__main__":
    main()