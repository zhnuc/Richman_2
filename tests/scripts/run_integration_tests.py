#!/usr/bin/env python3
"""
大富翁游戏集成测试运行器
自动读取integration目录下的测试用例，进行批处理测试
"""

import os
import sys
import subprocess
import json
import difflib
from pathlib import Path

class IntegrationTestRunner:
    def __init__(self, project_root):
        self.project_root = Path(project_root)
        self.integration_dir = self.project_root / "tests" / "integration"
        self.game_binary = self.project_root / "rich"
        self.results = []
        
    def run_single_test(self, test_dir):
        """运行单个集成测试"""
        test_name = test_dir.name
        print(f"\n🧪 运行测试: {test_name}")
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
                cmd_input = f.read()
            
            result = subprocess.run(
                [str(self.game_binary)],
                input=cmd_input,
                text=True,
                capture_output=True,
                timeout=30,
                cwd=str(test_dir)
            )
            
            # 保存输出文件（便于检查日志）
            with open(output_file, 'w', encoding='utf-8') as f:
                f.write(result.stdout.rstrip() + '\n')
            
            
            # 检查是否有dump命令，如果有则保存dump.json
            if "dump" in cmd_input:
                # 从输出中提取dump文件路径
                lines = result.stdout.split('\n')
                dump_path = None
                for line in lines:
                    if "游戏状态已保存到:" in line:
                        dump_path = line.split(": ")[-1].strip()
                        break
                
                # 现在程序在test_dir中执行，dump.json应该直接在那里
                dump_source = test_dir / "dump.json"
                if dump_source.exists():
                    # 如果dump.json和目标文件不是同一个文件，才移动
                    if str(dump_source) != str(dump_file):
                        subprocess.run(["mv", str(dump_source), str(dump_file)], check=True)
                    print(f"📄 生成dump文件: {dump_file.name}")
            
        except subprocess.TimeoutExpired:
            print("❌ 测试超时")
            return False
        except Exception as e:
            print(f"❌ 执行错误: {e}")
            return False
        
        # 比较dump文件
        dump_match = True
        if dump_file.exists():
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
        
        return test_passed
    
    def compare_files(self, expected_file, actual_file, file_type):
        """比较两个文件内容"""
        try:
            with open(expected_file, 'r', encoding='utf-8') as f:
                expected_content = f.read().strip()
            
            with open(actual_file, 'r', encoding='utf-8') as f:
                actual_content = f.read().strip()
            
            if expected_content == actual_content:
                print(f"✅ {file_type} 匹配")
                return True
            else:
                print(f"❌ {file_type} 不匹配")
                print(f"📄 期望文件: {expected_file}")
                print(f"📄 实际文件: {actual_file}")
                
                # 显示差异
                print("\n📊 差异对比:")
                expected_lines = expected_content.splitlines()
                actual_lines = actual_content.splitlines()
                
                diff = difflib.unified_diff(
                    expected_lines, actual_lines,
                    fromfile=f"期望 {file_type}",
                    tofile=f"实际 {file_type}",
                    lineterm=""
                )
                
                for line in diff:
                    print(line)
                
                return False
                
        except Exception as e:
            print(f"❌ 比较文件时出错: {e}")
            return False
    
    def compare_json_files(self, expected_file, actual_file):
        """比较JSON文件（更智能的比较）"""
        try:
            with open(expected_file, 'r', encoding='utf-8') as f:
                expected_data = json.load(f)
            
            with open(actual_file, 'r', encoding='utf-8') as f:
                actual_data = json.load(f)
            
            # 递归比较JSON结构
            return self.deep_compare_json(expected_data, actual_data, "")
            
        except json.JSONDecodeError as e:
            print(f"❌ JSON解析错误: {e}")
            return False
        except Exception as e:
            print(f"❌ 比较JSON时出错: {e}")
            return False
    
    def deep_compare_json(self, expected, actual, path):
        """深度比较JSON结构"""
        if type(expected) != type(actual):
            print(f"❌ 类型不匹配在 {path}: 期望 {type(expected).__name__}, 实际 {type(actual).__name__}")
            return False
        
        if isinstance(expected, dict):
            for key in expected:
                if key not in actual:
                    print(f"❌ 缺少键 {path}.{key}")
                    return False
                if not self.deep_compare_json(expected[key], actual[key], f"{path}.{key}"):
                    return False
            
            for key in actual:
                if key not in expected:
                    print(f"⚠️  额外键 {path}.{key}")
            
        elif isinstance(expected, list):
            if len(expected) != len(actual):
                print(f"❌ 列表长度不匹配在 {path}: 期望 {len(expected)}, 实际 {len(actual)}")
                return False
            
            for i, (exp_item, act_item) in enumerate(zip(expected, actual)):
                if not self.deep_compare_json(exp_item, act_item, f"{path}[{i}]"):
                    return False
        
        else:
            if expected != actual:
                print(f"❌ 值不匹配在 {path}: 期望 {expected}, 实际 {actual}")
                return False
        
        return True
    
    def run_all_tests(self):
        """运行所有集成测试"""
        print("🚀 开始运行大富翁游戏集成测试")
        print("=" * 60)
        
        if not self.game_binary.exists():
            print(f"❌ 游戏程序不存在: {self.game_binary}")
            print("请先运行 'make' 编译游戏程序")
            return False
        
        if not self.integration_dir.exists():
            print(f"❌ 集成测试目录不存在: {self.integration_dir}")
            return False
        
        # 查找所有测试目录
        test_dirs = [d for d in self.integration_dir.iterdir() 
                    if d.is_dir() and d.name.startswith('test')]
        
        if not test_dirs:
            print("⚠️  未找到任何测试用例")
            return False
        
        test_dirs.sort()  # 按名称排序
        
        print(f"📋 发现 {len(test_dirs)} 个测试用例")
        
        passed_tests = 0
        total_tests = len(test_dirs)
        
        for test_dir in test_dirs:
            if self.run_single_test(test_dir):
                passed_tests += 1
        
        # 输出总结
        print("\n" + "=" * 60)
        print("📊 测试总结")
        print("=" * 60)
        print(f"总测试数: {total_tests}")
        print(f"通过数量: {passed_tests}")
        print(f"失败数量: {total_tests - passed_tests}")
        print(f"通过率: {passed_tests/total_tests*100:.1f}%")
        
        if passed_tests == total_tests:
            print("🎉 所有测试通过！")
            return True
        else:
            print("❌ 部分测试失败")
            return False

def main():
    if len(sys.argv) > 1:
        project_root = sys.argv[1]
    else:
        project_root = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    
    runner = IntegrationTestRunner(project_root)
    success = runner.run_all_tests()
    sys.exit(0 if success else 1)

if __name__ == "__main__":
    main()