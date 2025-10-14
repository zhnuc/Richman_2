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
        self.report_file = self.project_root / "test_report.html"
        
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
        self.results = [] # 清空历史结果
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
            success = True
        else:
            print("❌ 部分测试失败")
            success = False
        
        self.generate_html_report()
        return success
    
    def run_single_test(self, test_dir):
        """运行单个集成测试"""
        test_name = test_dir.name
        status = self.get_test_status(test_name)
        
        test_result = {
            'name': test_name,
            'status': status,
            'passed': False,
            'diff': '',
            'reason': ''
        }
        
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
            test_result['reason'] = f"缺少文件: {', '.join(missing_files)}"
            self.results.append(test_result)
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
            test_result['reason'] = "测试超时"
            self.results.append(test_result)
            return False
        except Exception as e:
            print(f"❌ 运行测试时出错: {e}")
            test_result['reason'] = f"运行测试时出错: {e}"
            self.results.append(test_result)
            return False
        
        # 检查dump文件是否生成
        if dump_file.exists():
            print(f"📄 生成dump文件: {dump_file.name}")
            dump_match, diff_output = self.compare_json_files(
                test_dir / "expected_result.json",
                dump_file
            )
            test_result['diff'] = diff_output
        else:
            print("⚠️  未生成dump.json文件")
            dump_match = False
            test_result['reason'] = "未生成dump.json文件"
        
        # 返回测试结果
        test_passed = dump_match
        test_result['passed'] = test_passed
        
        if test_passed:
            print(f"✅ {test_name} 通过")
        else:
            print(f"❌ {test_name} 失败")
            if status == 'wip':
                print("   💡 提示: 这是开发中的功能，失败是正常的")
        
        self.results.append(test_result)
        return test_passed
    
    def compare_json_files(self, expected_file, actual_file):
        """比较JSON文件，返回 (is_match, diff_string)"""
        try:
            with open(expected_file, 'r', encoding='utf-8') as f:
                expected_data = json.load(f)
            
            with open(actual_file, 'r', encoding='utf-8') as f:
                actual_data = json.load(f)
            
            if expected_data == actual_data:
                print("✅ JSON状态 匹配")
                return True, ""
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
                
                diff_str = "\n".join(diff)
                print(diff_str)
                
                return False, diff_str
                
        except Exception as e:
            error_msg = f"❌ 比较JSON时出错: {e}"
            print(error_msg)
            return False, error_msg

    def categorize_test(self, test_name):
        """根据测试名称自动分类"""
        if not test_name.startswith('test_'):
            return '其他'
        
        # 提取test_后的第一个单词作为类别
        rest_name = test_name[5:]  # 去掉'test_'前缀
        
        # 找到第一个下划线或数字的位置，作为类别的结束
        category_end = len(rest_name)
        for i, char in enumerate(rest_name):
            if char.isdigit() or (char == '_' and i > 0 and not rest_name[i-1].isalpha()):
                category_end = i
                break
        
        category = rest_name[:category_end]
        
        # 如果类别为空或太短，返回完整的前缀
        if len(category) < 2:
            # 尝试找到更长的有意义前缀
            parts = rest_name.split('_')
            if len(parts) >= 2:
                category = '_'.join(parts[:2])
            else:
                category = parts[0] if parts else 'unknown'
        
        return category

    def generate_html_report(self):
        """生成HTML格式的测试报告"""
        print(f"\n📄 生成HTML测试报告: {self.report_file}")
        
        passed_count = sum(1 for r in self.results if r['passed'])
        failed_count = len(self.results) - passed_count
        pass_rate = (passed_count / len(self.results) * 100) if self.results else 0
        
        # 按类别统计
        category_stats = {}
        for result in self.results:
            category = self.categorize_test(result['name'])
            if category not in category_stats:
                category_stats[category] = {'total': 0, 'passed': 0, 'failed': 0, 'tests': []}
            category_stats[category]['total'] += 1
            category_stats[category]['tests'].append(result)
            if result['passed']:
                category_stats[category]['passed'] += 1
            else:
                category_stats[category]['failed'] += 1
        
        from datetime import datetime
        now = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

        html = f"""
        <!DOCTYPE html>
        <html lang="zh-CN">
        <head>
            <meta charset="UTF-8">
            <title>大富翁游戏 - 测试报告</title>
            <style>
                body {{ font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, "Helvetica Neue", Arial, sans-serif; margin: 40px; background-color: #f8f9fa; color: #212529; }}
                h1, h2, h3 {{ color: #343a40; }}
                .container {{ max-width: 1200px; margin: auto; background: white; padding: 20px; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }}
                .summary {{ border-bottom: 1px solid #dee2e6; padding-bottom: 15px; margin-bottom: 20px; }}
                .summary p {{ margin: 5px 0; }}
                .category-stats {{ border-bottom: 1px solid #dee2e6; padding-bottom: 20px; margin-bottom: 20px; }}
                .category-table {{ width: 100%; border-collapse: collapse; margin-top: 15px; }}
                .category-table th, .category-table td {{ padding: 8px 12px; text-align: left; border: 1px solid #dee2e6; }}
                .category-table th {{ background-color: #f8f9fa; font-weight: bold; }}
                .category-table .category-name {{ font-weight: bold; }}
                .category-table .pass-rate {{ font-weight: bold; }}
                .test-case {{ border: 1px solid #dee2e6; padding: 15px; margin-bottom: 15px; border-radius: 5px; }}
                .passed {{ border-left: 5px solid #28a745; }}
                .failed {{ border-left: 5px solid #dc3545; }}
                .test-case h4 {{ margin-top: 0; color: #495057; }}
                .status-passed {{ color: #28a745; font-weight: bold; }}
                .status-failed {{ color: #dc3545; font-weight: bold; }}
                .category-section {{ margin-bottom: 30px; }}
                .category-header {{ background-color: #f8f9fa; padding: 10px 15px; border-radius: 5px; margin-bottom: 15px; }}
                .category-header h3 {{ margin: 0; color: #343a40; }}
                .category-summary {{ font-size: 14px; color: #6c757d; margin-top: 5px; }}
                pre {{ background-color: #e9ecef; padding: 15px; border-radius: 5px; white-space: pre-wrap; word-wrap: break-word; }}
                .diff-del {{ color: #dc3545; text-decoration: line-through; }}
                .diff-add {{ color: #28a745; }}
                .toc {{ background-color: #f8f9fa; padding: 15px; border-radius: 5px; margin-bottom: 20px; }}
                .toc h3 {{ margin-top: 0; }}
                .toc ul {{ list-style-type: none; padding-left: 0; }}
                .toc li {{ margin: 5px 0; }}
                .toc a {{ text-decoration: none; color: #007bff; }}
                .toc a:hover {{ text-decoration: underline; }}
            </style>
        </head>
        <body>
            <div class="container">
                <h1>大富翁游戏 - 测试报告</h1>
                
                <div class="summary">
                    <h2>测试摘要</h2>
                    <p><strong>报告生成时间:</strong> {now}</p>
                    <p><strong>总测试数:</strong> {len(self.results)}</p>
                    <p><strong>通过:</strong> <span class="status-passed">{passed_count}</span></p>
                    <p><strong>失败:</strong> <span class="status-failed">{failed_count}</span></p>
                    <p><strong>通过率:</strong> {pass_rate:.2f}%</p>
                </div>

                <div class="category-stats">
                    <h2>各类别测试统计</h2>
                    <table class="category-table">
                        <thead>
                            <tr>
                                <th>测试类别</th>
                                <th>总数</th>
                                <th>通过</th>
                                <th>失败</th>
                                <th>通过率</th>
                            </tr>
                        </thead>
                        <tbody>
        """
        
        # 添加类别统计表
        for category, stats in sorted(category_stats.items()):
            pass_rate_cat = (stats['passed'] / stats['total'] * 100) if stats['total'] > 0 else 0
            html += f"""
                            <tr>
                                <td class="category-name"><a href="#category-{category}">{category}</a></td>
                                <td>{stats['total']}</td>
                                <td><span class="status-passed">{stats['passed']}</span></td>
                                <td><span class="status-failed">{stats['failed']}</span></td>
                                <td class="pass-rate">{pass_rate_cat:.1f}%</td>
                            </tr>
            """
        
        html += """
                        </tbody>
                    </table>
                </div>

                <div class="toc">
                    <h3>快速导航</h3>
                    <ul>
        """
        
        # 添加目录
        for category in sorted(category_stats.keys()):
            stats = category_stats[category]
            html += f'<li><a href="#category-{category}">{category} ({stats["total"]}个测试)</a></li>'
        
        html += """
                    </ul>
                </div>

                <h2>测试详情</h2>
        """

        # 按类别显示测试详情
        for category in sorted(category_stats.keys()):
            stats = category_stats[category]
            pass_rate_cat = (stats['passed'] / stats['total'] * 100) if stats['total'] > 0 else 0
            
            html += f"""
                <div class="category-section" id="category-{category}">
                    <div class="category-header">
                        <h3>{category}</h3>
                        <div class="category-summary">
                            总计: {stats['total']} | 
                            通过: <span class="status-passed">{stats['passed']}</span> | 
                            失败: <span class="status-failed">{stats['failed']}</span> | 
                            通过率: {pass_rate_cat:.1f}%
                        </div>
                    </div>
            """
            
            # 排序：失败的测试在前，通过的测试在后
            sorted_tests = sorted(stats['tests'], key=lambda x: (x['passed'], x['name']))
            
            for result in sorted_tests:
                result_class = "passed" if result['passed'] else "failed"
                result_text = "通过" if result['passed'] else "失败"
                result_status_class = "status-passed" if result['passed'] else "status-failed"
                
                html += f"""
                    <div class="test-case {result_class}">
                        <h4>{result['name']}</h4>
                        <p><strong>状态:</strong> <span class="{result_status_class}">{result_text}</span></p>
                """
                
                if not result['passed']:
                    if result['reason']:
                        html += f"<p><strong>原因:</strong> {result['reason']}</p>"
                    if result['diff']:
                        # 简单的diff高亮，正确处理换行符
                        diff_html = ""
                        for line in result['diff'].splitlines():
                            if line.startswith('-'):
                                diff_html += f'<span class="diff-del">{line}</span>\n'
                            elif line.startswith('+'):
                                diff_html += f'<span class="diff-add">{line}</span>\n'
                            else:
                                diff_html += f'{line}\n'
                        html += f"<h5>差异对比:</h5><pre>{diff_html}</pre>"

                html += "</div>"
            
            html += "</div>"

        html += """
            </div>
        </body>
        </html>
        """
        
        try:
            with open(self.report_file, 'w', encoding='utf-8') as f:
                f.write(html)
            print(f"✅ 报告已保存到 {self.report_file}")
        except Exception as e:
            print(f"❌ 保存HTML报告时出错: {e}")

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
