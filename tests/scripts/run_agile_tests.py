#!/usr/bin/env python3
"""
敏捷开发测试管理器
支持测试状态管理，允许标记pending、active、disabled等状态
"""

import os
import sys
import subprocess
import json
import difflib
from pathlib import Path

class AgileTestManager:
    def __init__(self, project_root):
        self.project_root = Path(project_root)
        self.integration_dir = self.project_root / "tests" / "integration"
        self.game_binary = self.project_root / "rich"
        self.status_config = self.project_root / "tests" / "test_status.config"
        self.results = []
        self.test_statuses = self.load_test_statuses()
        
    def load_test_statuses(self):
        """加载测试状态配置"""
        statuses = {}
        if self.status_config.exists():
            with open(self.status_config, 'r', encoding='utf-8') as f:
                for line in f:
                    line = line.strip()
                    if line and not line.startswith('#'):
                        if ':' in line:
                            test_name, status_part = line.split(':', 1)
                            # 移除状态后面的注释
                            status = status_part.split('#')[0].strip()
                            statuses[test_name.strip()] = status
        return statuses
    
    def get_test_status(self, test_name):
        """获取测试状态"""
        return self.test_statuses.get(test_name, 'active')
    
    def should_run_test(self, test_name):
        """判断是否应该运行测试"""
        status = self.get_test_status(test_name)
        return status in ['active', 'wip']
    
    def is_pending_test(self, test_name):
        """判断是否是待实现的测试"""
        status = self.get_test_status(test_name)
        return status == 'pending'
    
    def run_tests(self):
        """运行所有集成测试"""
        print("🚀 开始运行大富翁游戏集成测试（敏捷模式）")
        print("=" * 60)
        
        # 检查游戏二进制文件
        if not self.game_binary.exists():
            print(f"❌ 找不到游戏二进制文件: {self.game_binary}")
            return False
        
        # 获取所有测试目录
        test_dirs = sorted([d for d in self.integration_dir.iterdir() 
                           if d.is_dir() and d.name.startswith('test_')])
        
        if not test_dirs:
            print("❌ 没有找到测试用例")
            return False
        
        # 分类统计测试
        active_tests = []
        pending_tests = []
        disabled_tests = []
        wip_tests = []
        
        for test_dir in test_dirs:
            test_name = test_dir.name
            status = self.get_test_status(test_name)
            
            if status == 'active':
                active_tests.append(test_name)
            elif status == 'pending':
                pending_tests.append(test_name)
            elif status == 'disabled':
                disabled_tests.append(test_name)
            elif status == 'wip':
                wip_tests.append(test_name)
        
        print(f"📋 测试分类统计:")
        print(f"   🟢 活跃测试: {len(active_tests)}")
        print(f"   🟡 开发中测试: {len(wip_tests)}")
        print(f"   🔵 待实现测试: {len(pending_tests)}")
        print(f"   ⚫ 禁用测试: {len(disabled_tests)}")
        print()
        
        # 显示待实现的测试
        if pending_tests:
            print("📝 待实现功能的测试用例:")
            for test in pending_tests:
                print(f"   ⏳ {test} - 等待功能实现")
            print()
        
        # 运行活跃和开发中的测试
        tests_to_run = active_tests + wip_tests
        passed = 0
        failed = 0
        
        for test_dir in test_dirs:
            test_name = test_dir.name
            if test_name in tests_to_run:
                if self.run_single_test(test_dir):
                    passed += 1
                else:
                    failed += 1
        
        # 输出摘要
        print("=" * 60)
        print("📊 敏捷测试摘要")
        print("=" * 60)
        total_executed = passed + failed
        print(f"执行测试数: {total_executed}")
        print(f"通过数量: {passed}")
        print(f"失败数量: {failed}")
        if pending_tests:
            print(f"待实现数量: {len(pending_tests)}")
        if disabled_tests:
            print(f"禁用数量: {len(disabled_tests)}")
        
        if total_executed > 0:
            pass_rate = (passed / total_executed) * 100
            print(f"通过率: {pass_rate:.1f}%")
        
        if failed == 0:
            print("✅ 所有活跃测试通过！")
            return True
        else:
            print("❌ 部分测试失败")
            return False
    
    def run_single_test(self, test_dir):
        """运行单个集成测试"""
        test_name = test_dir.name
        status = self.get_test_status(test_name)
        
        status_emoji = {
            'active': '🟢',
            'wip': '🟡',
            'pending': '🔵',
            'disabled': '⚫'
        }
        
        print(f"\n{status_emoji.get(status, '⚪')} 运行测试: {test_name} ({status})")
        print("=" * 50)
        
        # 检查必需文件
        required_files = ["input.txt", "expected_result.json"]
        missing_files = []
        for file in required_files:
            if not (test_dir / file).exists():
                missing_files.append(file)
        
        if missing_files:
            print(f"❌ 缺少文件: {', '.join(missing_files)}")
            return False
        
        # 检查可选文件
        preset_file = test_dir / "preset.json"
        if preset_file.exists():
            print(f"📋 使用预设文件: {preset_file.name}")
        
        # 运行游戏程序
        input_file = test_dir / "input.txt"
        output_file = test_dir / "output.txt"
        dump_file = test_dir / "dump.json"
        
        try:
            # 执行游戏程序
            with open(input_file, 'r', encoding='utf-8') as f:
                input_data = f.read()
            
            # 构建命令
            cmd = [str(self.game_binary)]
            if preset_file.exists():
                cmd.extend(["-i", str(preset_file)])
            
            # 执行程序
            result = subprocess.run(
                cmd,
                input=input_data,
                text=True,
                capture_output=True,
                timeout=30,
                cwd=test_dir
            )
            
            # 保存输出
            with open(output_file, 'w', encoding='utf-8') as f:
                f.write(result.stdout)
            
            if result.stderr:
                print(f"⚠️  程序stderr: {result.stderr}")
            
        except subprocess.TimeoutExpired:
            print("❌ 测试超时")
            return False
        except Exception as e:
            print(f"❌ 运行测试时出错: {e}")
            return False
        
        # 检查dump文件是否生成
        if dump_file.exists():
            print(f"📄 生成dump文件: {dump_file.name}")
            dump_match = self.compare_json_files(
                test_dir / "expected_result.json",
                dump_file
            )
        else:
            print("⚠️  未生成dump.json文件")
            dump_match = False
        
        # 返回测试结果
        test_passed = dump_match
        if test_passed:
            print(f"✅ {test_name} 通过")
        else:
            print(f"❌ {test_name} 失败")
            if status == 'wip':
                print("   💡 提示: 这是开发中的功能，失败是正常的")
        
        return test_passed
    
    def compare_json_files(self, expected_file, actual_file):
        """比较JSON文件"""
        try:
            with open(expected_file, 'r', encoding='utf-8') as f:
                expected_data = json.load(f)
            
            with open(actual_file, 'r', encoding='utf-8') as f:
                actual_data = json.load(f)
            
            if expected_data == actual_data:
                print("✅ JSON状态 匹配")
                return True
            else:
                print("❌ JSON状态 不匹配")
                print(f"📄 期望文件: {expected_file}")
                print(f"📄 实际文件: {actual_file}")
                
                # 显示差异
                print("\n📊 差异对比:")
                expected_str = json.dumps(expected_data, indent=4, ensure_ascii=False)
                actual_str = json.dumps(actual_data, indent=4, ensure_ascii=False)
                
                expected_lines = expected_str.splitlines()
                actual_lines = actual_str.splitlines()
                
                diff = difflib.unified_diff(
                    expected_lines, actual_lines,
                    fromfile="期望 JSON状态",
                    tofile="实际 JSON状态",
                    lineterm=""
                )
                
                for line in diff:
                    print(line)
                
                return False
                
        except Exception as e:
            print(f"❌ 比较JSON时出错: {e}")
            return False

def main():
    if len(sys.argv) != 2:
        print("用法: python run_agile_tests.py <项目root路径>")
        sys.exit(1)
    
    project_root = sys.argv[1]
    test_manager = AgileTestManager(project_root)
    
    success = test_manager.run_tests()
    sys.exit(0 if success else 1)

if __name__ == "__main__":
    main()
