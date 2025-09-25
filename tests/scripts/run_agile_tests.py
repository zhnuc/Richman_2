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
        return self.test_statuses.get(test_name, 'pending')
    
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

    def batch_update_status(self, pattern, new_status):
        """批量更新测试状态"""
        print(f"🔄 批量更新测试状态: {pattern} -> {new_status}")
        
        # 获取所有测试目录
        test_dirs = sorted([d for d in self.integration_dir.iterdir() 
                           if d.is_dir() and d.name.startswith('test_')])
        
        updated_tests = []
        
        # 匹配测试名称
        for test_dir in test_dirs:
            test_name = test_dir.name
            if self._match_pattern(test_name, pattern):
                self.test_statuses[test_name] = new_status
                updated_tests.append(test_name)
        
        if updated_tests:
            # 保存到配置文件
            self._save_test_statuses()
            print(f"✅ 已更新 {len(updated_tests)} 个测试用例:")
            for test in updated_tests:
                print(f"   • {test}: {new_status}")
        else:
            print(f"❌ 没有找到匹配模式 '{pattern}' 的测试用例")
    
    def _match_pattern(self, test_name, pattern):
        """匹配测试名称模式"""
        import re
        
        # 支持正则表达式（包含特殊字符如[]）
        if any(char in pattern for char in ['[', ']', '^', '$', '(', ')', '+']):
            try:
                return re.match(f"^{pattern}$", test_name) is not None
            except re.error:
                # 如果正则表达式无效，回退到通配符匹配
                pass
        
        # 支持通配符 * 和 ?
        if '*' in pattern or '?' in pattern:
            # 转换通配符为正则表达式
            regex_pattern = pattern.replace('*', '.*').replace('?', '.')
            return re.match(f"^{regex_pattern}$", test_name) is not None
        
        # 支持前缀匹配
        if pattern.endswith('*'):
            return test_name.startswith(pattern[:-1])
        
        # 精确匹配
        return test_name == pattern
    
    def _save_test_statuses(self):
        """保存测试状态到配置文件"""
        lines = []
        
        # 保留文件头部注释
        if self.status_config.exists():
            with open(self.status_config, 'r', encoding='utf-8') as f:
                for line in f:
                    stripped = line.strip()
                    if stripped.startswith('#') or not stripped:
                        lines.append(line)
                    elif ':' in stripped:
                        break  # 遇到第一个配置行就停止保留
        
        # 添加测试状态配置
        for test_name, status in sorted(self.test_statuses.items()):
            lines.append(f"{test_name}: {status}\n")
        
        # 写入文件
        with open(self.status_config, 'w', encoding='utf-8') as f:
            f.writelines(lines)
    
    def find_new_tests(self):
        """发现新的测试用例（未在配置文件中的）"""
        # 获取所有测试目录
        test_dirs = sorted([d for d in self.integration_dir.iterdir() 
                           if d.is_dir() and d.name.startswith('test_')])
        
        new_tests = []
        for test_dir in test_dirs:
            test_name = test_dir.name
            if test_name not in self.test_statuses:
                new_tests.append(test_name)
        
        return new_tests
    
    def auto_add_new_tests(self, default_status='pending'):
        """自动添加新发现的测试用例到配置文件"""
        new_tests = self.find_new_tests()
        
        if not new_tests:
            print("✅ 没有发现新的测试用例")
            return
        
        print(f"🆕 发现 {len(new_tests)} 个新测试用例:")
        for test in new_tests:
            print(f"   • {test}")
        
        # 添加到配置中
        for test in new_tests:
            self.test_statuses[test] = default_status
        
        # 保存配置文件
        self._save_test_statuses()
        print(f"✅ 已将 {len(new_tests)} 个新测试用例添加到配置文件，状态: {default_status}")

    def list_tests(self):
        """列出所有测试及其状态"""
        print("📋 测试用例状态列表")
        print("=" * 60)
        
        # 获取所有测试目录
        test_dirs = sorted([d for d in self.integration_dir.iterdir() 
                           if d.is_dir() and d.name.startswith('test_')])
        
        if not test_dirs:
            print("❌ 没有找到测试用例")
            return
        
        # 检查是否有新测试用例
        new_tests = self.find_new_tests()
        if new_tests:
            print(f"⚠️  发现 {len(new_tests)} 个未配置的测试用例:")
            for test in new_tests:
                print(f"   🆕 {test} (默认: pending)")
            print("   💡 使用 --auto-add 自动添加这些测试用例到配置文件")
            print()
        
        # 按状态分组
        status_groups = {
            'active': [],
            'wip': [],
            'pending': [],
            'disabled': []
        }
        
        for test_dir in test_dirs:
            test_name = test_dir.name
            status = self.get_test_status(test_name)
            if status in status_groups:
                status_groups[status].append(test_name)
            else:
                status_groups.setdefault('other', []).append(f"{test_name} ({status})")
        
        # 显示分组结果
        status_emoji = {
            'active': '🟢',
            'wip': '🟡', 
            'pending': '🔵',
            'disabled': '⚫'
        }
        
        for status, tests in status_groups.items():
            if tests:
                emoji = status_emoji.get(status, '⚪')
                status_cn = {
                    'active': '活跃测试',
                    'wip': '开发中测试',
                    'pending': '待实现测试',
                    'disabled': '禁用测试'
                }.get(status, f'{status.upper()}测试')
                
                print(f"\n{emoji} {status_cn} ({len(tests)}个):")
                for test in sorted(tests):
                    print(f"   • {test}")

def main():
    import argparse
    
    parser = argparse.ArgumentParser(description='敏捷测试管理器')
    parser.add_argument('project_root', help='项目根目录路径')
    parser.add_argument('--list', '-l', action='store_true', help='列出所有测试及其状态')
    parser.add_argument('--batch-update', '-b', nargs=2, metavar=('PATTERN', 'STATUS'),
                       help='批量更新测试状态 (支持通配符 *, ?, 正则表达式)')
    parser.add_argument('--auto-add', '-a', nargs='?', const='pending', metavar='STATUS',
                       help='自动添加新发现的测试用例 (默认状态: pending)')
    parser.add_argument('--find-new', '-n', action='store_true', help='只查找新测试用例，不添加')
    parser.add_argument('--verbose', '-v', action='store_true', help='详细输出')
    
    args = parser.parse_args()
    
    test_manager = AgileTestManager(args.project_root)
    
    if args.find_new:
        new_tests = test_manager.find_new_tests()
        if new_tests:
            print(f"🆕 发现 {len(new_tests)} 个新测试用例:")
            for test in new_tests:
                print(f"   • {test}")
        else:
            print("✅ 没有发现新的测试用例")
        sys.exit(0)
    
    if args.auto_add:
        valid_statuses = ['active', 'wip', 'pending', 'disabled']
        if args.auto_add not in valid_statuses:
            print(f"❌ 无效状态: {args.auto_add}")
            print(f"   有效状态: {', '.join(valid_statuses)}")
            sys.exit(1)
        test_manager.auto_add_new_tests(args.auto_add)
        sys.exit(0)
    
    if args.list:
        test_manager.list_tests()
        sys.exit(0)
    
    if args.batch_update:
        pattern, status = args.batch_update
        valid_statuses = ['active', 'wip', 'pending', 'disabled']
        if status not in valid_statuses:
            print(f"❌ 无效状态: {status}")
            print(f"   有效状态: {', '.join(valid_statuses)}")
            sys.exit(1)
        
        test_manager.batch_update_status(pattern, status)
        sys.exit(0)
    
    # 默认运行测试
    success = test_manager.run_tests()
    sys.exit(0 if success else 1)

if __name__ == "__main__":
    main()
