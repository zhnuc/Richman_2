#!/usr/bin/env python3
"""
创建回合切换机制测试用例的脚本
"""

import os
import json

def create_test_case(test_name, description_content, preset_content, input_content, expected_content):
    """创建单个测试用例"""
    test_dir = f"tests/integration/{test_name}"
    os.makedirs(test_dir, exist_ok=True)
    
    # 创建description.txt
    with open(f"{test_dir}/description.txt", "w", encoding="utf-8") as f:
        f.write(description_content)
    
    # 创建preset.json（如果提供）
    if preset_content:
        with open(f"{test_dir}/preset.json", "w", encoding="utf-8") as f:
            json.dump(preset_content, f, indent=4, ensure_ascii=False)
    
    # 创建input.txt
    with open(f"{test_dir}/input.txt", "w", encoding="utf-8") as f:
        f.write(input_content)
    
    # 创建expected_result.json
    with open(f"{test_dir}/expected_result.json", "w", encoding="utf-8") as f:
        json.dump(expected_content, f, indent=4, ensure_ascii=False)

# test_turn_004 - 坐牢状态递减测试
create_test_case("test_turn_004", 
"""测试用例：test_turn_004
功能模块：回合管理
测试目标：验证坐牢玩家的状态递减

测试描述：
1. 使用preset加载玩家坐牢状态（prison=2）
2. 执行dump命令获取当前状态
3. 验证坐牢天数从2递减为1
4. 验证其他状态保持不变

验证内容：
- prison状态从2递减为1
- 玩家其他属性保持不变
- 游戏状态保持started=true
- 玩家位置保持在45

测试重点：
- 验证坐牢状态的正确递减
- 确保状态递减不影响其他属性
- 验证回合跳过机制的状态更新""",
{
    "players": [
        {
            "index": 0,
            "name": "Q",
            "fund": 10000,
            "credit": 0,
            "location": 45,
            "alive": True,
            "prop": {"bomb": 0, "barrier": 0, "robot": 0, "total": 0},
            "buff": {"god": 0, "prison": 2, "hospital": 0}
        }
    ],
    "houses": {},
    "placed_prop": {"bomb": [], "barrier": []},
    "game": {"now_player": 0, "next_player": 0, "started": True, "ended": False, "winner": -1}
},
"dump",
{
    "players": [
        {
            "index": 0,
            "name": "Q",
            "fund": 10000,
            "credit": 0,
            "location": 45,
            "alive": True,
            "prop": {"bomb": 0, "barrier": 0, "robot": 0, "total": 0},
            "buff": {"god": 0, "prison": 1, "hospital": 0}
        }
    ],
    "houses": {},
    "placed_prop": {"bomb": [], "barrier": []},
    "game": {"now_player": 0, "next_player": 0, "started": True, "ended": False, "winner": -1}
})

# test_turn_005 - 坐牢状态结束测试
create_test_case("test_turn_005",
"""测试用例：test_turn_005
功能模块：回合管理
测试目标：验证坐牢状态结束时正确清零

测试描述：
1. 使用preset加载玩家坐牢状态（prison=1）
2. 执行dump命令获取当前状态
3. 验证坐牢状态从1变为0
4. 验证玩家恢复正常状态

验证内容：
- prison状态从1变为0
- 玩家恢复正常状态
- 其他属性保持不变
- 玩家位置保持在45

测试重点：
- 验证坐牢状态结束时的清零处理
- 确保状态恢复的正确性
- 验证特殊状态生命周期的完整性""",
{
    "players": [
        {
            "index": 0,
            "name": "Q",
            "fund": 10000,
            "credit": 0,
            "location": 45,
            "alive": True,
            "prop": {"bomb": 0, "barrier": 0, "robot": 0, "total": 0},
            "buff": {"god": 0, "prison": 1, "hospital": 0}
        }
    ],
    "houses": {},
    "placed_prop": {"bomb": [], "barrier": []},
    "game": {"now_player": 0, "next_player": 0, "started": True, "ended": False, "winner": -1}
},
"dump",
{
    "players": [
        {
            "index": 0,
            "name": "Q",
            "fund": 10000,
            "credit": 0,
            "location": 45,
            "alive": True,
            "prop": {"bomb": 0, "barrier": 0, "robot": 0, "total": 0},
            "buff": {"god": 0, "prison": 0, "hospital": 0}
        }
    ],
    "houses": {},
    "placed_prop": {"bomb": [], "barrier": []},
    "game": {"now_player": 0, "next_player": 0, "started": True, "ended": False, "winner": -1}
})

print("✅ 已创建 test_turn_004 和 test_turn_005")
