# HFUT 哈夫曼压缩工具

基于 Qt Widgets 的文件压缩与解压缩课程项目，使用字符频率表和哈夫曼树生成编码。

## 实现内容

- `hfutzip.cpp`：频率统计、优先队列建树、压缩和解压缩；包含原始格式、O 格式与 U 格式处理。
- `mainwindow.cpp`：文件选择、压缩级别与结果统计，通过 QThread 调度压缩任务。
- `fileedit.cpp`：文件拖放与路径输入。
- `node.cpp`：哈夫曼树结点。

U 格式的变长编码边界已修复；256 种字符自动使用 O 格式。当前没有可复现的性能基准，本页不承诺固定的压缩率提升。

## 构建

需要 Qt Widgets、qmake 和支持 C++17 的编译器。工程入口是 `hfutzip.pro`。

```bash
git clone https://github.com/KingYeon-Zoo/HFUT-ZIP.git
cd HFUT-ZIP
mkdir build
cd build
qmake ../hfutzip.pro
make
```

Windows 请使用对应 Qt 工具链提供的构建命令。项目未提供 CMakeLists.txt。

## 使用与验证

构建后打开应用，选择输入文件、输出路径及压缩或解压缩模式。验证压缩正确性时，应比较原始文件和解压文件的哈希，并覆盖空文件、单一字节重复、中文文本和二进制数据。

已完成 Qt 应用编译与无界面核心回归验证。许可证见 [LICENSE](LICENSE)，贡献说明见 [CONTRIBUTING.md](CONTRIBUTING.md)。

## 复核与验证

[2026-09-05 复核记录](docs/reviews/2026-09-05.md)记录本次检查、结果与未覆盖部分。

## 格式安全检查

压缩和解压缩统一检查文件长度、频率表、尾部位数和解码结果；输入、压缩结果和解压结果均限制在 100 MiB 内。失败时保留已有目标文件，只在全部校验和写入成功后原子提交。旧版损坏的 U 编码无法可靠猜测恢复，会明确报错。

已使用 Qt Core 配合地址与未定义行为检查器验证三种旧格式兼容、24 组往返、变长边界和异常输入。测试入口为 `tests/codec.pro`；CI 自动执行，不需要打开图形界面。
