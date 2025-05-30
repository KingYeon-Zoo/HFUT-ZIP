#include "fileedit.h"
#include <QDebug>

FileEdit::FileEdit(QWidget *parent) : QLineEdit(parent)
{
    setDragEnabled(true); // 启用拖放功能
    bt_show = 1;          // 初始化
}

void FileEdit::dragEnterEvent(QDragEnterEvent *event)
{
    const QMimeData *data = event->mimeData(); // 获取拖入的数据
    QList<QUrl> urls = data->urls();           // 获取拖入的文件的路径
    if (urls.size() == 1 && urls[0].isLocalFile())
    {                                  // 如果只有一个本地文件
        event->acceptProposedAction(); // 接受事件   什么意思
    }
}

void FileEdit::dragMoveEvent(QDragMoveEvent *event)
{
    const QMimeData *data = event->mimeData(); // 获取拖入的数据
    QList<QUrl> urls = data->urls();           // 获取拖入的文件的路径
    if (urls.size() == 1 && urls[0].isLocalFile())
    {                                  // 如果只有一个本地文件
        event->acceptProposedAction(); // 接受事件  什么意思
    }
}

void FileEdit::dropEvent(QDropEvent *event)
{
    QString path_in, path_out;
    const QMimeData *data = event->mimeData();     // 获取拖入的数据
    QList<QUrl> urls = data->urls();               // 获取拖入的文件的路径
    if (urls.size() == 1 && urls[0].isLocalFile()) // 如果只有一个本地文件
    {
        // 如果是文件夹就返回
        if (!QFileInfo(urls[0].toLocalFile()).isFile())
        {
            QMessageBox::information(this, "错误", "文件不能为文件夹", QMessageBox::Ok);
            return;
        }

        if (bt_show == 1)
        {

            // path_in的初始化
            path_in = urls[0].toLocalFile();
            if (!path_in.isEmpty())
            {
                // 发送信号
                emit set_in_path(path_in);
                QFileInfo temp(path_in);
                // 获取文件的绝对路径
                QString path = temp.absoluteFilePath();
                // 获取文件的后缀
                QString suffix = temp.suffix();
                // 更改文件的后缀
                path_out = path.replace(suffix, "hfut");
                emit set_out_path(path_out);
            }
        }
        else // bt_show==0
        {

            path_in = urls[0].toLocalFile();
            // 判断文件格式
            if (QFileInfo(path_in).suffix() != "hfut")
            {
                QMessageBox::information(this, "提示", "文件格式应为.hfut", QMessageBox::Ok);
                path_in.clear();
                return;
            }
            emit set_in_path(path_in);

            if (!path_in.isEmpty())
            {
                QFileInfo temp(path_in);
                // 获取文件的绝对路径
                QString path = temp.absoluteFilePath();
                // 获取文件的后缀
                QString suffix = temp.suffix();
                // 更改文件的后缀
                path_out = path.replace(suffix, get_suffix(path_in));
                emit set_out_path(path_out);
            }
        }
    }
}

QString FileEdit::get_suffix(QString path_in) // 得到压缩前文件后缀
{
    QFile infile(path_in);
    if (!infile.open(QIODevice::ReadOnly)) {
        qDebug() << "FileEdit: 无法打开文件读取后缀名:" << path_in;
        return QString("txt");
    }

    qDebug() << "FileEdit: 开始解析文件后缀:" << path_in;

    // 检测格式类型并跳过标识符
    char first_byte;
    if (infile.read(&first_byte, 1) != 1) {
        qDebug() << "FileEdit: 文件为空或读取失败";
        infile.close();
        return QString("txt");
    }
    
    // 判断格式类型
    if (first_byte == 'O') {
        qDebug() << "FileEdit: 检测到O格式，已跳过标识符";
    } else if (first_byte == 'U') {
        qDebug() << "FileEdit: 检测到U格式，已跳过标识符";
    } else {
        // 原始格式，需要回退一个字节
        infile.seek(0);
        qDebug() << "FileEdit: 检测到原始格式，回退文件指针";
    }

    // 读取后缀名
    std::string temp_str_suffix;
    char temp_char_suffix;
    int char_count = 0;
    const int MAX_SUFFIX_LENGTH = 10; // 防止无限循环
    
    while (infile.read((char *)&temp_char_suffix, 1) && char_count < MAX_SUFFIX_LENGTH)
    {
        char_count++;
        
        if (temp_char_suffix == '\0')
        {
            infile.close();
            QString result = QString::fromStdString(temp_str_suffix);
            qDebug() << "FileEdit: 成功提取文件后缀名:" << result;
            
            // 验证后缀名的合法性
            if (result.isEmpty() || result.length() > 8) {
                qDebug() << "FileEdit: 后缀名无效，返回默认值txt";
                return QString("txt");
            }
            
            // 检查是否包含非法字符
            for (QChar c : result) {
                if (!c.isLetterOrNumber()) {
                    qDebug() << "FileEdit: 后缀名包含非法字符，返回默认值txt";
                    return QString("txt");
                }
            }
            
            return result;
        }
        else
        {
            temp_str_suffix += temp_char_suffix;
        }
    }
    
    infile.close();
    qDebug() << "FileEdit: 获取后缀名失败，返回默认值txt";
    return QString("txt"); // 返回默认后缀
}
