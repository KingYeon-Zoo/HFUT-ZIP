#include "safe_codec.h"
#include "hfutzip.h"

void hfutzip::delete_node(node *n)
{
    if (n == NULL)
        return;
    delete_node(n->lchild);
    delete_node(n->rchild);
    delete n;
}

hfutzip::hfutzip() : QObject(NULL), count_char{0}, binary_coding{}
{
    jud_binary_out_finished = 0;
    use_optimized_format = true; // 默认使用优化格式
    compression_level = LEVEL_STANDARD; // 默认标准压缩级别
}

bool hfutzip::compare(node *n1, node *n2)
{
    return n1->count > n2->count; // 这里先后之后再试一试
}

void hfutzip::process_file_makezip() {
    try { SafeCodec::compress(path_in, path_out, compression_level); emit zip_finished(); }
    catch (const std::exception& e) { emit zip_failed(QString::fromUtf8(e.what())); }
}

void hfutzip::create_zip_file()
{
    QFile outfile(path_out);
    outfile.open(QIODevice::WriteOnly);

    // 写入后缀名,以及一个\0
    std::string str_suffix = QFileInfo(path_in).suffix().toStdString();
    char char_0 = '\0';
    for (int i = 0; i < (int)str_suffix.size(); i++)
    {
        outfile.write((char *)&str_suffix[i], 1);
    }
    outfile.write(&char_0, 1);

    // 写入数组
    outfile.write((char *)count_char, 256 * sizeof(int)); // 不对，写入顺序即可以//写的可以更少吗，怎么简化

    // 写入压缩后的编码
    std::string temp_string;
    QFile infile(path_in);
    infile.open(QIODevice::ReadOnly);
    unsigned char temp_char_in = 0;
    while (infile.read((char *)&temp_char_in, 1)) // 要是我填>1这时最后只剩下三个
    {
        temp_string += binary_coding[temp_char_in];
        while (temp_string.size() > 8)
        {
            unsigned char temp_char_out = 0;
            for (int i = 0; i < 8; i++)
            {
                temp_char_out = temp_char_out * 2 + temp_string[i] - '0'; // 无符号扩展
                // 或者强制转换为int计算就不会有问题
            }
            temp_string.erase(0, 8);
            outfile.write((char *)&temp_char_out, 1);
        }
    }
    infile.close(); // 关闭

    // 最后还剩下一点，特殊处理，也就是记录下倒二个字符为后temp_char_final_num个
    unsigned char temp_char_out_final = 0;
    for (int i = 0; i < (int)temp_string.size(); i++)
    {
        temp_char_out_final = temp_char_out_final * 2 + temp_string[i] - '0';
    }
    char temp_char_final_num;
    temp_char_final_num = temp_string.size();
    outfile.write((char *)&temp_char_out_final, 1);
    outfile.write(&temp_char_final_num, 1);
    outfile.close();
}

void hfutzip::binary_coding_tree(node *n)
{
    static std::string temp;
    if (n == NULL)
    {
        return;
    }
    n->binary_coding = temp;

    temp += '0';
    binary_coding_tree(n->lchild);
    temp.pop_back();

    temp += '1';
    binary_coding_tree(n->rchild);
    temp.pop_back();
}

void hfutzip::jud_binary_out(bool b) // 写入解压缩文件
{
    static QFile *outfile = NULL;
    if (outfile == NULL)
    {
        outfile = new QFile(path_out);
    }
    if (!outfile->isOpen())
    {
        outfile->open(QIODevice::WriteOnly); // 重复打开会重置文件指针
    }

    if (jud_binary_out_finished == 1)
    {
        outfile->close();
        delete outfile;
        outfile = NULL;
        jud_binary_out_finished = 0;
        return;
    }
    if (b == 0)
    {
        now = now->lchild;
    }
    else
    {
        now = now->rchild;
    }
    if (now->lchild == NULL && now->rchild == NULL) // 前后只要一个应该就可以了
    {
        outfile->write((char *)&(now->a), 1);
        now = root;
    }
}

void hfutzip::process_file() {
    try { SafeCodec::decompress(path_in, path_out); emit zip_finished(); }
    catch (const std::exception& e) { emit zip_failed(QString::fromUtf8(e.what())); }
}

QString hfutzip::get_suffix() // 读取文件，得到文件的后缀名
{
    QFile infile(path_in);
    if (!infile.open(QIODevice::ReadOnly)) {
        qDebug() << "无法打开文件读取后缀名:" << path_in;
        return QString("txt");
    }

    qDebug() << "开始解析文件后缀:" << path_in;

    // 直接读取第一个字节来检测格式
    char first_byte;
    if (infile.read(&first_byte, 1) != 1) {
        qDebug() << "文件为空或读取失败";
        infile.close();
        return QString("txt");
    }
    
    // 判断格式类型
    bool hasFormatFlag = false;
    if (first_byte == 'O') {
        hasFormatFlag = true;
        qDebug() << "检测到O格式，已跳过标识符";
    } else if (first_byte == 'U') {
        hasFormatFlag = true;
        qDebug() << "检测到U格式，已跳过标识符";
    } else {
        // 原始格式，需要回退一个字节
        hasFormatFlag = false;
        infile.seek(0);
        qDebug() << "检测到原始格式，回退文件指针到开始位置";
    }

    // 读取后缀名
    std::string temp_str_suffix;
    char temp_char_suffix;
    int char_count = 0;
    const int MAX_SUFFIX_LENGTH = 10; // 防止无限循环
    
    while (infile.read((char *)&temp_char_suffix, 1) && char_count < MAX_SUFFIX_LENGTH)
    {
        char_count++;
        qDebug() << "读取字符[" << char_count << "]:" << (int)temp_char_suffix << "'" << temp_char_suffix << "'";
        
        if (temp_char_suffix == '\0')
        {
            infile.close();
            QString result = QString::fromStdString(temp_str_suffix);
            qDebug() << "找到终止符，提取的后缀名:" << result;
            
            // 验证后缀名的合法性
            if (result.isEmpty()) {
                qDebug() << "后缀名为空，返回默认值txt";
                return QString("txt");
            }
            
            // 检查是否包含非法字符
            for (QChar c : result) {
                if (!c.isLetterOrNumber()) {
                    qDebug() << "后缀名包含非法字符:" << c << "，返回默认值txt";
                    return QString("txt");
                }
            }
            
            // 检查长度是否合理
            if (result.length() > 8) {
                qDebug() << "后缀名过长:" << result.length() << "，返回默认值txt";
                return QString("txt");
            }
            
            qDebug() << "成功提取有效后缀名:" << result;
            return result;
        }
        else
        {
            temp_str_suffix += temp_char_suffix;
        }
    }
    
    infile.close();
    qDebug() << "未找到终止符或达到最大长度，当前读取内容:" << QString::fromStdString(temp_str_suffix);
    return QString("txt"); // 返回默认后缀
}

void hfutzip::create_zip_file_optimized()
{
    QFile outfile(path_out);
    outfile.open(QIODevice::WriteOnly);

    // 写入优化格式标识符
    char format_flag = 'O'; // 'O' for Optimized
    outfile.write(&format_flag, 1);

    // 写入后缀名,以及一个\0
    std::string str_suffix = QFileInfo(path_in).suffix().toStdString();
    char char_0 = '\0';
    for (int i = 0; i < (int)str_suffix.size(); i++)
    {
        outfile.write((char *)&str_suffix[i], 1);
    }
    outfile.write(&char_0, 1);

    // 写入优化的频率表
    int freq_count = char_freq_optimized.size();
    outfile.write((char *)&freq_count, sizeof(int)); // 写入字符种类数量
    
    for (const auto& cf : char_freq_optimized) {
        outfile.write((char *)&cf.character, 1);
        outfile.write((char *)&cf.frequency, sizeof(int));
    }

    // 写入压缩后的编码
    std::string temp_string;
    QFile infile(path_in);
    infile.open(QIODevice::ReadOnly);
    unsigned char temp_char_in = 0;
    while (infile.read((char *)&temp_char_in, 1))
    {
        temp_string += binary_coding[temp_char_in];
        while (temp_string.size() > 8)
        {
            unsigned char temp_char_out = 0;
            for (int i = 0; i < 8; i++)
            {
                temp_char_out = temp_char_out * 2 + temp_string[i] - '0';
            }
            temp_string.erase(0, 8);
            outfile.write((char *)&temp_char_out, 1);
        }
    }
    infile.close();

    // 最后还剩下一点，特殊处理
    unsigned char temp_char_out_final = 0;
    for (int i = 0; i < (int)temp_string.size(); i++)
    {
        temp_char_out_final = temp_char_out_final * 2 + temp_string[i] - '0';
    }
    char temp_char_final_num;
    temp_char_final_num = temp_string.size();
    outfile.write((char *)&temp_char_out_final, 1);
    outfile.write(&temp_char_final_num, 1);
    outfile.close();
}

void hfutzip::process_file_optimized()
{
    QFile infile(path_in);
    if (!infile.open(QIODevice::ReadOnly)) {
        qDebug() << "无法打开文件进行解压缩:" << path_in;
        emit zip_finished();
        return;
    }
    
    // 跳过格式标识符
    char format_flag;
    if (infile.read(&format_flag, 1) != 1) {
        qDebug() << "读取格式标识符失败";
        infile.close();
        emit zip_finished();
        return;
    }
    
    // 跳过文件后缀名
    char temp_char_find0;
    int suffix_chars = 0;
    while (infile.read(&temp_char_find0, 1) == 1 && suffix_chars < 100) // 防止无限循环
    {
        suffix_chars++;
        if (temp_char_find0 == '\0')
        {
            break;
        }
    }
    
    if (suffix_chars >= 100) {
        qDebug() << "后缀名过长，文件格式可能损坏";
        infile.close();
        emit zip_finished();
        return;
    }
    
    // 读取优化的频率表
    int freq_count;
    if (infile.read((char *)&freq_count, sizeof(int)) != sizeof(int)) {
        qDebug() << "读取频率表数量失败";
        infile.close();
        emit zip_finished();
        return;
    }
    
    if (freq_count <= 0 || freq_count > 256) {
        qDebug() << "频率表数量无效:" << freq_count;
        infile.close();
        emit zip_finished();
        return;
    }
    
    // 清空原有数据
    memset(count_char, 0, sizeof(count_char));
    char_freq_optimized.clear();
    
    for (int i = 0; i < freq_count; i++) {
        CharFreq cf;
        if (infile.read((char *)&cf.character, 1) != 1 ||
            infile.read((char *)&cf.frequency, sizeof(int)) != sizeof(int)) {
            qDebug() << "读取频率表项失败:" << i;
            infile.close();
            emit zip_finished();
            return;
        }
        
        if (cf.frequency <= 0) {
            qDebug() << "无效的频率值:" << cf.frequency;
            infile.close();
            emit zip_finished();
            return;
        }
        
        char_freq_optimized.push_back(cf);
        count_char[cf.character] = cf.frequency;
    }
    
    // 统计剩余数据长度
    int count = 0;
    unsigned char count_temp_char;
    while (infile.read((char *)&count_temp_char, 1) == 1)
    {
        count++;
        if (count > 100000000) { // 防止过大文件导致内存问题
            qDebug() << "文件过大，停止处理";
            infile.close();
            emit zip_finished();
            return;
        }
    }
    infile.close();
    
    if (count < 2) {
        qDebug() << "压缩数据太短:" << count;
        emit zip_finished();
        return;
    }
    
    // 初始化vec
    vec_char_final.clear();
    for (const auto& cf : char_freq_optimized)
    {
        node *temp_n = new node(cf.frequency, cf.character);
        vec_char_final.push_back(temp_n);
    }
    
    if (vec_char_final.empty()) {
        qDebug() << "没有有效的字符频率数据";
        emit zip_finished();
        return;
    }
    
    // 创建优先队列
    char_final = std::priority_queue<node *, std::vector<node *>, decltype(&hfutzip::compare)>(vec_char_final.begin(), vec_char_final.end(), &hfutzip::compare);
    
    // 创建哈夫曼树
    while (char_final.size() > 1) // 修改条件，确保安全
    {
        if (char_final.empty()) {
            qDebug() << "优先队列意外为空";
            emit zip_finished();
            return;
        }
        
        node *temp_n1 = char_final.top();
        char_final.pop();
        
        if (char_final.empty()) {
            // 只有一个节点，直接作为根节点
            root = temp_n1;
            break;
        }
        
        node *temp_n2 = char_final.top();
        char_final.pop();
        node *temp_n3 = new node(temp_n1->count + temp_n2->count, 0);
        temp_n3->lchild = temp_n1;
        temp_n3->rchild = temp_n2;
        char_final.push(temp_n3);
    }
    
    if (!char_final.empty()) {
        root = char_final.top();
    }
    
    if (root == nullptr) {
        qDebug() << "哈夫曼树根节点为空";
        emit zip_finished();
        return;
    }
    
    // 赋予编码
    binary_coding_tree(root);
    
    // 重新打开文件进行解压缩
    QFile infile2(path_in);
    if (!infile2.open(QIODevice::ReadOnly)) {
        qDebug() << "重新打开文件失败";
        delete_node(root);
        emit zip_finished();
        return;
    }
    
    // 跳过格式标识符
    infile2.read(&format_flag, 1);
    
    // 跳过文件后缀名
    suffix_chars = 0;
    while (infile2.read(&temp_char_find0, 1) == 1 && suffix_chars < 100)
    {
        suffix_chars++;
        if (temp_char_find0 == '\0')
        {
            break;
        }
    }
    
    // 跳过频率表
    infile2.read((char *)&freq_count, sizeof(int));
    for (int i = 0; i < freq_count; i++) {
        unsigned char dummy_char;
        int dummy_freq;
        infile2.read((char *)&dummy_char, 1);
        infile2.read((char *)&dummy_freq, sizeof(int));
    }
    
    // 解压缩数据
    unsigned char temp_char = 0;
    now = root;
    for (int i = 0; i < count - 2; i++)
    {
        if (infile2.read((char *)&temp_char, 1) != 1) {
            qDebug() << "读取压缩数据失败:" << i;
            break;
        }
        int k = temp_char;
        for (int j = 7; j >= 0; j--)
        {
            jud_binary_out(((k >> j) & 1));
        }
    }
    
    // 最后两个字节特殊处理
    unsigned char temp_char2 = 0;
    if (infile2.read((char *)&temp_char, 1) == 1 && 
        infile2.read((char *)&temp_char2, 1) == 1) {
        for (int i = temp_char2 - 1; i >= 0; i--)
        {
            jud_binary_out(((temp_char >> i) & 1));
        }
    }
    
    jud_binary_out_finished = 1;
    jud_binary_out(1);
    
    infile2.close();
    delete_node(root);
    emit zip_finished();
}

bool hfutzip::detect_file_format()
{
    // 检测文件格式：优化格式以'O'开头，原始格式直接是后缀名
    QFile infile(path_in);
    if (!infile.open(QIODevice::ReadOnly)) {
        qDebug() << "无法打开文件进行格式检测:" << path_in;
        return false; // 默认使用原始格式
    }
    
    char first_byte;
    if (infile.read(&first_byte, 1) != 1) {
        qDebug() << "文件为空或读取失败:" << path_in;
        infile.close();
        return false; // 默认使用原始格式
    }
    
    infile.close();
    
    // 如果第一个字节是'O'，则为优化格式
    return (first_byte == 'O');
}

char hfutzip::detect_file_format_type()
{
    // 检测具体的文件格式类型
    QFile infile(path_in);
    if (!infile.open(QIODevice::ReadOnly)) {
        qDebug() << "无法打开文件进行格式检测:" << path_in;
        return 0; // 原始格式
    }
    
    char first_byte;
    if (infile.read(&first_byte, 1) != 1) {
        qDebug() << "文件为空或读取失败:" << path_in;
        infile.close();
        return 0; // 原始格式
    }
    
    infile.close();
    
    if (first_byte == 'O') {
        return 'O'; // 优化格式
    } else if (first_byte == 'U') {
        return 'U'; // 超级优化格式
    } else {
        return 0;   // 原始格式
    }
}

bool hfutzip::shouldCompress(qint64 fileSize)
{
    // 检查文件是否适合压缩
    if (fileSize < COMPRESSION_THRESHOLD_MIN) {
        qDebug() << "文件太小，不进行压缩:" << fileSize << "字节";
        return false;
    }
    
    // 对于非常小的文件，即使超过阈值也要慎重
    if (fileSize < COMPRESSION_THRESHOLD_MEDIUM) {
        // 检查文件类型，某些二进制文件可能不适合压缩
        QString suffix = QFileInfo(path_in).suffix().toLower();
        QStringList binaryExtensions = {"jpg", "jpeg", "png", "gif", "mp3", "mp4", "zip", "rar", "7z"};
        if (binaryExtensions.contains(suffix)) {
            qDebug() << "检测到已压缩格式文件，跳过压缩:" << suffix;
            return false;
        }
    }
    
    return true;
}

double hfutzip::estimateCompressionRatio()
{
    // 预估压缩效果 - 基于字符频率分布的熵计算
    QFile infile(path_in);
    if (!infile.open(QIODevice::ReadOnly)) {
        return 1.0; // 无法估算，假设无压缩效果
    }
    
    // 重置字符统计
    memset(count_char, 0, sizeof(count_char));
    
    // 统计字符频率
    unsigned char temp;
    qint64 totalChars = 0;
    while (!infile.atEnd()) {
        if (infile.read(reinterpret_cast<char *>(&temp), 1) == 1) {
            count_char[temp]++;
            totalChars++;
        }
    }
    infile.close();
    
    if (totalChars == 0) return 1.0;
    
    // 计算信息熵
    double entropy = 0.0;
    int uniqueChars = 0;
    for (int i = 0; i < 256; i++) {
        if (count_char[i] > 0) {
            uniqueChars++;
            double probability = (double)count_char[i] / totalChars;
            entropy -= probability * log2(probability);
        }
    }
    
    // 理论压缩比 = 实际熵 / 8位
    double theoreticalRatio = entropy / 8.0;
    
    // 考虑头部开销的实际压缩比估算
    qint64 headerSize = 0;
    switch (compression_level) {
        case LEVEL_FAST:
            headerSize = 1024 + 20; // O格式头部
            break;
        case LEVEL_MAXIMUM:
            headerSize = uniqueChars * 2 + 20; // U格式头部估算
            break;
        default:
            headerSize = uniqueChars * 5 + 20; // 标准格式头部
            break;
    }
    
    qint64 compressedDataSize = (qint64)(totalChars * theoreticalRatio);
    double actualRatio = (double)(headerSize + compressedDataSize) / totalChars;
    
    qDebug() << "压缩比估算 - 文件大小:" << totalChars 
             << "唯一字符:" << uniqueChars 
             << "熵:" << entropy 
             << "理论压缩比:" << theoreticalRatio 
             << "实际压缩比:" << actualRatio;
    
    return actualRatio;
}

int hfutzip::writeVariableLength(QFile &file, int value) {
    try { QByteArray bytes; SafeCodec::putVar(bytes, value); return file.write(bytes)==bytes.size()?bytes.size():-1; }
    catch (const std::exception&) { return -1; }
}

int hfutzip::readVariableLength(QFile &file) {
    char first; if(file.read(&first,1)!=1)return -1;
    const unsigned head=static_cast<unsigned char>(first);
    const int size=!(head&128)?1:((head&192)==128?2:4);
    QByteArray bytes(1,first); bytes+=file.read(size-1);
    try { SafeCodec::Reader reader{bytes};return reader.variable(); }
    catch (const std::exception&) { return -1; }
}

CharMapping hfutzip::createCharacterMapping()
{
    CharMapping mapping;
    mapping.char_to_index.resize(256, 255); // 初始化为无效值
    mapping.index_to_char.clear();
    mapping.unique_chars_count = 0;
    
    // 收集所有使用的字符
    for (int i = 0; i < 256; i++) {
        if (count_char[i] > 0) {
            mapping.char_to_index[i] = mapping.unique_chars_count;
            mapping.index_to_char.push_back((unsigned char)i);
            mapping.unique_chars_count++;
        }
    }
    
    qDebug() << "创建字符映射表，唯一字符数:" << mapping.unique_chars_count;
    return mapping;
}

void hfutzip::applyCharacterMapping(const CharMapping &mapping)
{
    // 应用字符映射到频率表
    char_freq_optimized.clear();
    for (int i = 0; i < mapping.unique_chars_count; i++) {
        unsigned char originalChar = mapping.index_to_char[i];
        char_freq_optimized.push_back(CharFreq(originalChar, count_char[originalChar]));
    }
}

void hfutzip::create_zip_file_ultra()
{
    QFile outfile(path_out);
    outfile.open(QIODevice::WriteOnly);

    // 写入超级优化格式标识符
    char format_flag = 'U'; // 'U' for Ultra-optimized
    outfile.write(&format_flag, 1);

    // 写入后缀名,以及一个\0
    std::string str_suffix = QFileInfo(path_in).suffix().toStdString();
    char char_0 = '\0';
    for (int i = 0; i < (int)str_suffix.size(); i++)
    {
        outfile.write((char *)&str_suffix[i], 1);
    }
    outfile.write(&char_0, 1);

    // 创建字符映射表
    CharMapping mapping = createCharacterMapping();
    
    // 写入字符数量
    unsigned char char_count = (unsigned char)mapping.unique_chars_count;
    outfile.write((char*)&char_count, 1);
    
    // 写入字符映射表
    for (int i = 0; i < mapping.unique_chars_count; i++) {
        outfile.write((char*)&mapping.index_to_char[i], 1);
    }
    
    // 写入变长编码的频率表
    for (int i = 0; i < mapping.unique_chars_count; i++) {
        unsigned char originalChar = mapping.index_to_char[i];
        writeVariableLength(outfile, count_char[originalChar]);
    }

    // 写入压缩后的编码
    std::string temp_string;
    QFile infile(path_in);
    infile.open(QIODevice::ReadOnly);
    unsigned char temp_char_in = 0;
    while (infile.read((char *)&temp_char_in, 1))
    {
        temp_string += binary_coding[temp_char_in];
        while (temp_string.size() > 8)
        {
            unsigned char temp_char_out = 0;
            for (int i = 0; i < 8; i++)
            {
                temp_char_out = temp_char_out * 2 + temp_string[i] - '0';
            }
            temp_string.erase(0, 8);
            outfile.write((char *)&temp_char_out, 1);
        }
    }
    infile.close();

    // 最后还剩下一点，特殊处理
    unsigned char temp_char_out_final = 0;
    for (int i = 0; i < (int)temp_string.size(); i++)
    {
        temp_char_out_final = temp_char_out_final * 2 + temp_string[i] - '0';
    }
    char temp_char_final_num;
    temp_char_final_num = temp_string.size();
    outfile.write((char *)&temp_char_out_final, 1);
    outfile.write(&temp_char_final_num, 1);
    outfile.close();
}

void hfutzip::process_file_ultra()
{
    QFile infile(path_in);
    if (!infile.open(QIODevice::ReadOnly)) {
        qDebug() << "无法打开文件进行Ultra解压缩:" << path_in;
        emit zip_finished();
        return;
    }
    
    // 跳过格式标识符
    char format_flag;
    if (infile.read(&format_flag, 1) != 1) {
        qDebug() << "读取Ultra格式标识符失败";
        infile.close();
        emit zip_finished();
        return;
    }
    
    // 跳过文件后缀名
    char temp_char_find0;
    int suffix_chars = 0;
    while (infile.read(&temp_char_find0, 1) == 1 && suffix_chars < 100)
    {
        suffix_chars++;
        if (temp_char_find0 == '\0')
        {
            break;
        }
    }
    
    if (suffix_chars >= 100) {
        qDebug() << "Ultra格式后缀名过长，文件可能损坏";
        infile.close();
        emit zip_finished();
        return;
    }
    
    // 读取字符数量
    unsigned char char_count;
    if (infile.read((char*)&char_count, 1) != 1) {
        qDebug() << "读取字符数量失败";
        infile.close();
        emit zip_finished();
        return;
    }
    
    if (char_count == 0) {
        qDebug() << "字符数量为0，无效的Ultra格式文件";
        infile.close();
        emit zip_finished();
        return;
    }
    
    // 读取字符映射表
    CharMapping mapping;
    mapping.unique_chars_count = char_count;
    mapping.index_to_char.resize(char_count);
    mapping.char_to_index.resize(256, 255);
    
    for (int i = 0; i < char_count; i++) {
        if (infile.read((char*)&mapping.index_to_char[i], 1) != 1) {
            qDebug() << "读取字符映射表失败:" << i;
            infile.close();
            emit zip_finished();
            return;
        }
        mapping.char_to_index[mapping.index_to_char[i]] = i;
    }
    
    // 读取变长编码的频率表
    memset(count_char, 0, sizeof(count_char));
    char_freq_optimized.clear();
    
    for (int i = 0; i < char_count; i++) {
        unsigned char originalChar = mapping.index_to_char[i];
        int frequency = readVariableLength(infile);
        if (frequency <= 0) {
            qDebug() << "读取频率失败或频率无效:" << frequency;
            infile.close();
            emit zip_finished();
            return;
        }
        count_char[originalChar] = frequency;
        char_freq_optimized.push_back(CharFreq(originalChar, frequency));
    }
    
    // 统计剩余数据长度
    int count = 0;
    unsigned char count_temp_char;
    while (infile.read((char *)&count_temp_char, 1))
    {
        count++;
        if (count > 100000000) { // 防止过大文件
            qDebug() << "Ultra格式文件过大，停止处理";
            infile.close();
            emit zip_finished();
            return;
        }
    }
    infile.close();
    
    if (count < 2) {
        qDebug() << "Ultra格式压缩数据太短:" << count;
        emit zip_finished();
        return;
    }
    
    // 初始化vec
    vec_char_final.clear();
    for (const auto& cf : char_freq_optimized)
    {
        node *temp_n = new node(cf.frequency, cf.character);
        vec_char_final.push_back(temp_n);
    }
    
    // 创建优先队列
    char_final = std::priority_queue<node *, std::vector<node *>, decltype(&hfutzip::compare)>(vec_char_final.begin(), vec_char_final.end(), &hfutzip::compare);
    
    // 创建哈夫曼树
    while (char_final.size() > 1)
    {
        if (char_final.empty()) {
            qDebug() << "Ultra格式哈夫曼树构建失败：队列为空";
            emit zip_finished();
            return;
        }
        
        node *temp_n1 = char_final.top();
        char_final.pop();
        
        if (char_final.empty()) {
            root = temp_n1;
            break;
        }
        
        node *temp_n2 = char_final.top();
        char_final.pop();
        node *temp_n3 = new node(temp_n1->count + temp_n2->count, 0);
        temp_n3->lchild = temp_n1;
        temp_n3->rchild = temp_n2;
        char_final.push(temp_n3);
    }
    
    if (!char_final.empty()) {
        root = char_final.top();
    }
    
    if (root == nullptr) {
        qDebug() << "Ultra格式哈夫曼树根节点为空";
        emit zip_finished();
        return;
    }
    
    // 赋予编码
    binary_coding_tree(root);
    
    // 重新打开文件进行解压缩
    QFile infile2(path_in);
    if (!infile2.open(QIODevice::ReadOnly)) {
        qDebug() << "重新打开Ultra格式文件失败";
        delete_node(root);
        emit zip_finished();
        return;
    }
    
    // 跳过格式标识符
    infile2.read(&format_flag, 1);
    
    // 跳过文件后缀名
    suffix_chars = 0;
    while (infile2.read(&temp_char_find0, 1) == 1 && suffix_chars < 100)
    {
        suffix_chars++;
        if (temp_char_find0 == '\0')
        {
            break;
        }
    }
    
    // 跳过字符数量
    infile2.read((char*)&char_count, 1);
    
    // 跳过字符映射表
    for (int i = 0; i < char_count; i++) {
        unsigned char dummy_char;
        infile2.read((char*)&dummy_char, 1);
    }
    
    // 跳过变长频率表
    for (int i = 0; i < char_count; i++) {
        readVariableLength(infile2);
    }
    
    // 解压缩数据
    unsigned char temp_char = 0;
    now = root;
    for (int i = 0; i < count - 2; i++)
    {
        if (infile2.read((char *)&temp_char, 1) != 1) {
            qDebug() << "读取Ultra格式压缩数据失败:" << i;
            break;
        }
        int k = temp_char;
        for (int j = 7; j >= 0; j--)
        {
            jud_binary_out(((k >> j) & 1));
        }
    }
    
    // 最后两个字节特殊处理
    unsigned char temp_char2 = 0;
    if (infile2.read((char *)&temp_char, 1) == 1 && 
        infile2.read((char *)&temp_char2, 1) == 1) {
        for (int i = temp_char2 - 1; i >= 0; i--)
        {
            jud_binary_out(((temp_char >> i) & 1));
        }
    }
    
    jud_binary_out_finished = 1;
    jud_binary_out(1);
    
    infile2.close();
    delete_node(root);
    emit zip_finished();
}

char hfutzip::selectBestCompressionFormat()
{
    // 自动选择最佳压缩格式
    QFile infile(path_in);
    qint64 fileSize = infile.size();
    
    // 检查文件是否适合压缩
    if (!shouldCompress(fileSize)) {
        qDebug() << "文件不适合压缩，建议直接存储";
        return 0; // 不压缩
    }
    
    // 预估压缩效果
    double estimated_ratio = estimateCompressionRatio();
    
    // 根据文件大小和压缩效果选择格式
    if (fileSize < COMPRESSION_THRESHOLD_MEDIUM) {
        // 小文件优先使用轻量格式
        if (estimated_ratio < 0.8) {
            qDebug() << "小文件选择优化格式'O'";
            return 'O';
        } else {
            qDebug() << "小文件压缩效果不佳，使用原始格式";
            return 0; // 原始格式
        }
    } else {
        // 大文件根据压缩效果选择
        if (estimated_ratio < 0.6) {
            qDebug() << "大文件压缩效果好，选择超级优化格式'U'";
            return 'U';
        } else if (estimated_ratio < 0.8) {
            qDebug() << "大文件选择优化格式'O'";
            return 'O';
        } else {
            qDebug() << "大文件压缩效果不佳，使用原始格式";
            return 0; // 原始格式
        }
    }
}
