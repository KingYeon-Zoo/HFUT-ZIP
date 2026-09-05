#ifndef HFUTZIP_H
#define HFUTZIP_H
#include <string>
#include <vector>
#include <queue>
#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include "node.h"

// 压缩优化常量
const qint64 COMPRESSION_THRESHOLD_MIN = 512;      // 最小压缩阈值(字节)
const qint64 COMPRESSION_THRESHOLD_MEDIUM = 2048;  // 中等文件阈值(字节)
const double MIN_COMPRESSION_RATIO = 0.95;         // 最小有效压缩比

// 压缩级别枚举
enum CompressionLevel {
    LEVEL_FAST = 0,      // 快速压缩(当前O格式)
    LEVEL_STANDARD = 1,   // 标准压缩(智能选择)
    LEVEL_MAXIMUM = 2     // 最大压缩(U格式)
};

// 优化的字符频率存储结构
struct CharFreq {
    unsigned char character;
    int frequency;
    CharFreq(unsigned char c = 0, int f = 0) : character(c), frequency(f) {}
};

// 字符映射结构
struct CharMapping {
    std::vector<unsigned char> char_to_index;  // 字符到索引的映射
    std::vector<unsigned char> index_to_char;  // 索引到字符的映射
    int unique_chars_count;                    // 唯一字符数量
};

class hfutzip : public QObject
{
    Q_OBJECT
private:
    void create_zip_file();
    void create_zip_file_optimized(); // 优化版本的压缩文件创建
    void create_zip_file_ultra();     // 超级优化版本('U'格式)
    void binary_coding_tree(node *n);
    void process_file_optimized(); // 优化版本的解压缩
    void process_file_ultra();     // 超级优化版本的解压缩
    node *root;
    node *now;

public:
    hfutzip();
    QString path_in;
    QString path_out;

    // 每次完成需要初始化，所以public便于操作，或者用信号和槽函数
    static bool compare(node *n1, node *n2); // 记住static //外部初始化char_final有用
    void jud_binary_out(bool b);
    std::priority_queue<node *, std::vector<node *>, decltype(&hfutzip::compare)> char_final; // 记住要std，当然qt中也有对应容器
    int count_char[256];
    std::vector<CharFreq> char_freq_optimized; // 优化的频率存储
    std::vector<node *> vec_char_final;
    std::string binary_coding[256];
    bool jud_binary_out_finished;
    bool use_optimized_format; // 是否使用优化格式
    CompressionLevel compression_level; // 压缩级别
    QString get_suffix();
    
    // 文件格式检测函数
    bool detect_file_format(); // 返回true表示优化格式，false表示原始格式
    char detect_file_format_type(); // 返回格式类型字符：原始、'O'、'U'
    
    // 压缩优化相关函数
    bool shouldCompress(qint64 fileSize);                    // 检查文件是否适合压缩
    double estimateCompressionRatio();                       // 预估压缩效果
    char selectBestCompressionFormat();                      // 自动选择最佳压缩格式
    
    // 变长编码函数
    int writeVariableLength(QFile &file, int value);         // 写入变长编码
    int readVariableLength(QFile &file);                     // 读取变长编码
    
    // 字符映射优化函数
    CharMapping createCharacterMapping();                    // 创建字符映射表
    void applyCharacterMapping(const CharMapping &mapping);  // 应用字符映射

    void process_file();         // 解压缩
    void process_file_makezip(); // 压缩
    void delete_node(node *n);   // 释放空间

signals:
    void zip_failed(const QString& message);
    void zip_finished(); // 已完成
};

#endif
