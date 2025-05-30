# 贡献指南

感谢您对 HFUT 哈夫曼压缩工具项目的关注！我们欢迎各种形式的贡献，包括但不限于：

- 🐛 Bug 报告
- ✨ 新功能建议
- 📚 文档改进
- 🔧 代码贡献
- 🧪 测试用例添加
- 🎨 UI/UX 改进

## 📋 目录

- [开始之前](#开始之前)
- [开发环境搭建](#开发环境搭建)
- [项目结构](#项目结构)
- [编码规范](#编码规范)
- [提交流程](#提交流程)
- [Bug 报告](#bug-报告)
- [功能请求](#功能请求)
- [代码审查](#代码审查)
- [发布流程](#发布流程)

## 开始之前

### 行为准则

请阅读并遵守我们的[行为准则](CODE_OF_CONDUCT.md)。我们致力于为所有参与者提供友好、安全和包容的环境。

### 许可证

通过向此项目贡献代码，您同意您的贡献将在 [MIT License](LICENSE) 下获得许可。

## 开发环境搭建

### 1. 系统要求

- **Qt 6.0+** 及相关开发工具
- **C++17** 兼容的编译器
- **Git** 版本控制系统

### 2. 获取代码

```bash
# Fork 项目到你的 GitHub 账户
# 然后克隆你的 fork
git clone https://github.com/YOUR_USERNAME/HFUT-Huffman-Compressor.git
cd HFUT-Huffman-Compressor

# 添加上游仓库
git remote add upstream https://github.com/ORIGINAL_OWNER/HFUT-Huffman-Compressor.git
```

### 3. 安装依赖

**Ubuntu/Debian:**
```bash
sudo apt update
sudo apt install qtcreator qt6-base-dev qt6-tools-dev build-essential
```

**Windows:**
- 下载并安装 Qt 6.x from https://www.qt.io/download
- 选择 MinGW 或 MSVC 编译器

**macOS:**
```bash
brew install qt6
brew install --cask qt-creator
```

### 4. 编译项目

```bash
# 使用 Qt Creator（推荐）
qtcreator hfutzip.pro

# 或使用命令行
qmake hfutzip.pro
make
```

## 项目结构

```
HUFFMAN/
├── src/                    # 源代码
│   ├── main.cpp           # 程序入口
│   ├── mainwindow.cpp/h   # 主窗口
│   ├── hfutzip.cpp/h      # 核心压缩算法
│   ├── node.cpp/h         # 哈夫曼树节点
│   └── fileedit.cpp/h     # 文件拖拽组件
├── ui/                    # 界面文件
│   └── mainwindow.ui      # Qt 界面设计
├── resources/             # 资源文件
│   └── styles.qss         # 样式表
├── docs/                  # 文档
├── tests/                 # 测试文件
├── scripts/               # 构建脚本
├── .gitignore            # Git 忽略规则
├── hfutzip.pro           # Qt 项目文件
├── LICENSE               # 许可证
├── README.md             # 项目说明
├── CHANGELOG.md          # 变更日志
└── CONTRIBUTING.md       # 本文件
```

### 核心模块说明

#### hfutzip.cpp/h - 压缩算法核心
- `process_file_makezip()` - 主压缩入口
- `create_zip_file_ultra()` - U格式压缩
- `process_file_ultra()` - U格式解压
- `selectBestCompressionFormat()` - 智能格式选择

#### mainwindow.cpp/h - 用户界面
- 界面逻辑控制
- 用户交互处理
- 统计信息显示

#### node.cpp/h - 数据结构
- 哈夫曼树节点定义
- 树结构操作

## 编码规范

### C++ 编码标准

#### 命名约定
```cpp
// 类名：PascalCase
class CompressionEngine;

// 函数名：camelCase
void calculateCompressionRatio();

// 变量名：snake_case
int compression_level;
int file_size;

// 常量：UPPER_SNAKE_CASE
const int MAX_FILE_SIZE = 2147483647;

// 成员变量：加前缀
class MyClass {
private:
    int m_memberVariable;  // 或使用 trailing underscore
    static int s_staticVar;
};
```

#### 代码格式
```cpp
// 使用 4 空格缩进，不使用 Tab
if (condition) {
    doSomething();
} else {
    doOtherThing();
}

// 函数定义
ReturnType ClassName::functionName(Type parameter) 
{
    // 函数体
    return value;
}

// 注释格式
/**
 * @brief 函数功能简述
 * @param param1 参数1说明
 * @param param2 参数2说明
 * @return 返回值说明
 */
int calculateSomething(int param1, bool param2);
```

#### 头文件保护
```cpp
#ifndef FILENAME_H
#define FILENAME_H

// 头文件内容

#endif // FILENAME_H
```

### Qt 特定规范

#### 信号和槽
```cpp
// 信号定义
signals:
    void compressionFinished(bool success);
    
// 槽函数
private slots:
    void onCompressionFinished();
    
// 连接信号槽
connect(compressor, &Compressor::finished, 
        this, &MainWindow::onCompressionFinished);
```

#### 资源管理
```cpp
// 使用智能指针
std::unique_ptr<QFile> file(new QFile(filename));

// 或确保proper cleanup
QFile* file = new QFile(filename);
// ... 使用文件
delete file;
```

## 提交流程

### 1. 创建功能分支

```bash
# 确保在最新的主分支上
git checkout main
git pull upstream main

# 创建新的功能分支
git checkout -b feature/your-feature-name
# 或
git checkout -b bugfix/issue-number-description
```

### 2. 进行开发

- 遵循编码规范
- 添加必要的测试
- 更新相关文档
- 确保代码可以正常编译

### 3. 提交代码

#### 提交信息格式
```
type(scope): subject

body

footer
```

**类型 (type):**
- `feat`: 新功能
- `fix`: Bug 修复
- `docs`: 仅文档更改
- `style`: 代码格式修改
- `refactor`: 重构代码
- `test`: 添加测试
- `chore`: 构建过程或辅助工具的变动

**示例:**
```bash
git commit -m "feat(compression): add ultra compression format

- Implement variable-length frequency encoding
- Add character mapping optimization  
- Reduce header overhead by 90%

Closes #123"
```

### 4. 推送并创建 PR

```bash
# 推送到你的 fork
git push origin feature/your-feature-name

# 在 GitHub 上创建 Pull Request
```

## Bug 报告

### 在提交 Bug 之前

1. 检查 [现有 Issues](https://github.com/your-username/HFUT-Huffman-Compressor/issues)
2. 尝试在最新版本中重现问题
3. 收集相关信息

### Bug 报告模板

```markdown
**Bug 描述**
简要描述遇到的问题

**重现步骤**
1. 执行操作 '...'
2. 点击 '....'
3. 滚动到 '....'
4. 看到错误

**期望行为**
描述你期望发生的情况

**实际行为**
描述实际发生的情况

**环境信息**
- OS: [e.g. Windows 11, Ubuntu 20.04]
- Qt 版本: [e.g. 6.2.0]
- 编译器: [e.g. MinGW 8.1.0]

**附加信息**
- 错误日志
- 截图
- 测试文件（如果适用）
```

## 功能请求

### 功能请求模板

```markdown
**功能描述**
描述你希望添加的功能

**问题或需求**
这个功能解决什么问题？

**解决方案**
你希望如何实现这个功能？

**替代方案**
有没有其他可能的实现方式？

**附加信息**
其他相关信息、示例或参考
```

## 代码审查

### 提交 PR 前的检查清单

- [ ] 代码遵循项目编码规范
- [ ] 所有测试通过
- [ ] 添加了必要的测试用例
- [ ] 更新了相关文档
- [ ] 提交信息格式正确
- [ ] 没有引入新的编译警告
- [ ] 没有未解决的 TODO 或 FIXME
- [ ] 代码已经自测

### 审查标准

我们在代码审查中关注：

1. **功能性**: 代码是否正确实现了预期功能
2. **可读性**: 代码是否清晰易懂
3. **性能**: 是否有明显的性能问题
4. **安全性**: 是否存在安全漏洞
5. **一致性**: 是否符合项目编码规范
6. **测试**: 是否有足够的测试覆盖

### 响应反馈

- 及时回应审查意见
- 感谢审查者的时间和建议
- 如有不同意见，礼貌地讨论
- 根据反馈修改代码并推送更新

## 发布流程

### 版本号规则

遵循 [语义化版本](https://semver.org/lang/zh-CN/)：

- **MAJOR.MINOR.PATCH** (例如 2.1.0)
- **MAJOR**: 不兼容的 API 修改
- **MINOR**: 向下兼容的功能新增
- **PATCH**: 向下兼容的问题修正

### 发布检查清单

- [ ] 所有测试通过
- [ ] 文档已更新
- [ ] CHANGELOG.md 已更新
- [ ] 版本号已更新
- [ ] 创建 Git 标签
- [ ] 准备发布说明

## 获得帮助

如果你在贡献过程中遇到问题：

1. 查看 [文档](README.md)
2. 搜索 [现有 Issues](https://github.com/your-username/HFUT-Huffman-Compressor/issues)
3. 创建新的 Issue 寻求帮助
4. 加入我们的讨论区

## 致谢

感谢所有为项目做出贡献的开发者！你们的努力让这个项目变得更好。

---

再次感谢您的贡献！ 🎉 