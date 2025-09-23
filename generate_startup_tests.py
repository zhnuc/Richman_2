#!/usr/bin/env python3
"""
批量生成游戏启动测试用例脚本
"""

import os
import json
from pathlib import Path

def create_test_case(test_num, cmdlist, expected_output, expected_dump):
    """创建单个测试用例"""
    test_dir = Path(f"tests/integration/test_startup_{test_num:03d}")
    test_dir.mkdir(exist_ok=True)
    
    # 创建cmdlist.txt
    with open(test_dir / "cmdlist.txt", "w", encoding="utf-8") as f:
        f.write(cmdlist)
    
    # 创建expected_output.txt
    with open(test_dir / "expected_output.txt", "w", encoding="utf-8") as f:
        f.write(expected_output)
    
    # 创建expected_dump.json
    with open(test_dir / "expected_dump.json", "w", encoding="utf-8") as f:
        json.dump(expected_dump, f, indent=4, ensure_ascii=False)

def generate_startup_tests():
    """生成所有启动测试用例"""
    
    # 基础输出模板
    base_output = """大富翁游戏启动
欢迎来到大富翁，请按数字键选择你的角色：
1.钱夫人
2.阿土伯
3.孙小美
4.金贝贝
请输入选择 (1-4): 您选择了: {character}
游戏开始！
> 游戏结束"""
    
    # 无效选择输出模板
    invalid_output = """大富翁游戏启动
欢迎来到大富翁，请按数字键选择你的角色：
1.钱夫人
2.阿土伯
3.孙小美
4.金贝贝
请输入选择 (1-4): 无效选择，请输入1-4之间的数字
欢迎来到大富翁，请按数字键选择你的角色：
1.钱夫人
2.阿土伯
3.孙小美
4.金贝贝
请输入选择 (1-4): 您选择了: {character}
游戏开始！
> 游戏结束"""
    
    # 角色信息
    characters = {
        1: "钱夫人",
        2: "阿土伯", 
        3: "孙小美",
        4: "金贝贝"
    }
    
    # 生成基础dump模板
    def create_dump_template(player_count, character_names):
        players = []
        for i in range(player_count):
            players.append({
                "index": i,
                "name": character_names[i],
                "fund": 1500,
                "credit": 0,
                "location": 0,
                "alive": True,
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
            })
        
        return {
            "players": players,
            "houses": {},
            "placed_prop": {
                "bomb": [],
                "barrier": []
            },
            "game": {
                "now_player": 0,
                "next_player": 1 if player_count > 1 else 0,
                "started": False,
                "ended": False,
                "winner": -1
            }
        }
    
    # 测试用例7-10：角色选择界面显示测试
    test_cases = [
        # test_startup_007 - 角色选择界面显示测试
        {
            "cmdlist": "1\nquit",
            "output": base_output.format(character="钱夫人"),
            "dump": create_dump_template(1, ["钱夫人"])
        },
        # test_startup_008 - 角色确认信息测试
        {
            "cmdlist": "2\nquit", 
            "output": base_output.format(character="阿土伯"),
            "dump": create_dump_template(1, ["阿土伯"])
        },
        # test_startup_009 - 角色颜色显示测试
        {
            "cmdlist": "3\nquit",
            "output": base_output.format(character="孙小美"),
            "dump": create_dump_template(1, ["孙小美"])
        },
        # test_startup_010 - 角色在地图上的显示测试
        {
            "cmdlist": "4\nquit",
            "output": base_output.format(character="金贝贝"),
            "dump": create_dump_template(1, ["金贝贝"])
        }
    ]
    
    # 创建测试用例7-10
    for i, test_case in enumerate(test_cases, 7):
        create_test_case(i, test_case["cmdlist"], test_case["output"], test_case["dump"])
    
    # 测试用例11-15：各种无效输入测试
    invalid_tests = [
        # test_startup_011 - 输入0
        {
            "cmdlist": "0\n1\nquit",
            "output": invalid_output.format(character="钱夫人"),
            "dump": create_dump_template(1, ["钱夫人"])
        },
        # test_startup_012 - 输入负数
        {
            "cmdlist": "-1\n2\nquit",
            "output": invalid_output.format(character="阿土伯"),
            "dump": create_dump_template(1, ["阿土伯"])
        },
        # test_startup_013 - 输入字母
        {
            "cmdlist": "a\n3\nquit",
            "output": invalid_output.format(character="孙小美"),
            "dump": create_dump_template(1, ["孙小美"])
        },
        # test_startup_014 - 输入特殊字符
        {
            "cmdlist": "@\n4\nquit",
            "output": invalid_output.format(character="金贝贝"),
            "dump": create_dump_template(1, ["金贝贝"])
        },
        # test_startup_015 - 输入空行
        {
            "cmdlist": "\n1\nquit",
            "output": invalid_output.format(character="钱夫人"),
            "dump": create_dump_template(1, ["钱夫人"])
        }
    ]
    
    # 创建测试用例11-15
    for i, test_case in enumerate(invalid_tests, 11):
        create_test_case(i, test_case["cmdlist"], test_case["output"], test_case["dump"])
    
    # 测试用例16-20：多角色组合测试
    multi_player_tests = [
        # test_startup_016 - 角色组合12
        {
            "cmdlist": "12\nquit",
            "output": base_output.format(character="钱夫人"),
            "dump": create_dump_template(2, ["钱夫人", "阿土伯"])
        },
        # test_startup_017 - 角色组合13
        {
            "cmdlist": "13\nquit",
            "output": base_output.format(character="钱夫人"),
            "dump": create_dump_template(2, ["钱夫人", "孙小美"])
        },
        # test_startup_018 - 角色组合14
        {
            "cmdlist": "14\nquit",
            "output": base_output.format(character="钱夫人"),
            "dump": create_dump_template(2, ["钱夫人", "金贝贝"])
        },
        # test_startup_019 - 角色组合23
        {
            "cmdlist": "23\nquit",
            "output": base_output.format(character="阿土伯"),
            "dump": create_dump_template(2, ["阿土伯", "孙小美"])
        },
        # test_startup_020 - 角色组合24
        {
            "cmdlist": "24\nquit",
            "output": base_output.format(character="阿土伯"),
            "dump": create_dump_template(2, ["阿土伯", "金贝贝"])
        }
    ]
    
    # 创建测试用例16-20
    for i, test_case in enumerate(multi_player_tests, 16):
        create_test_case(i, test_case["cmdlist"], test_case["output"], test_case["dump"])
    
    print("✅ 已生成20个游戏启动测试用例")

if __name__ == "__main__":
    generate_startup_tests()