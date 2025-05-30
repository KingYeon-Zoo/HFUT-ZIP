#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), msgBox(this) // 记住父窗口，
      ,
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 初始化成员
    setWindowTitle("HFUT 哈夫曼压缩工具");
    
    // 加载Apple风格样式表
    QFile styleFile(":/styles.qss");
    if (!styleFile.exists()) {
        // 如果资源文件不存在，尝试从当前目录加载
        styleFile.setFileName("styles.qss");
    }
    if (styleFile.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        this->setStyleSheet(styleSheet);
        styleFile.close();
    }
    
    path_in_cor = 0;
    path_out_cor = 0;
    zip = new hfutzip;
    msgBox.setWindowTitle("请等待 ＞﹏＜");
    msgBox.setText("......");
    msgBox.setStandardButtons(QMessageBox::NoButton); // 重要（没了它可以尝试一下，会导致可以在子线程运行的时候更改zip->path_out,导致结果出错（数据竞争））

    zip_thread = new QThread;
    zip->moveToThread(zip_thread);
    ui->progressBar->hide();

    // 连接信号和槽，启用线程
    connect(ui->lineEdit_3, &FileEdit::set_in_path, [&](QString s)
            {
        zip->path_in=s;
        ui->lineEdit->setText(zip->path_in); });
    connect(ui->lineEdit_3, &FileEdit::set_out_path, [&](QString s)
            {
            path_in_cor=1;
            path_out_cor=1;
            zip->path_out=s;
            ui->lineEdit_2->setText(zip->path_out); });
    connect(this, &MainWindow::make_zip, zip, &hfutzip::process_file_makezip);
    connect(this, &MainWindow::process_zip, zip, &hfutzip::process_file);
    connect(zip, &hfutzip::zip_finished, this, &MainWindow::zip_finished);
    zip_thread->start(); // 记住
    
    // 初始化压缩级别选择
    ui->compressionLevelComboBox->setCurrentIndex(1); // 默认选择智能压缩
    zip->compression_level = LEVEL_STANDARD; // 设置默认压缩级别
}

void MainWindow::zip_finished()
{
    msgBox.setWindowTitle("处理完成");
    msgBox.setText("正在生成统计信息...");
    
    if (ui->lineEdit_3->bt_show == 1)
    {
        // 压缩完成 - 计算并显示详细统计信息
        calculateCompressionStats();
        showCompressionStats();
    } else {
        // 解压缩完成提示
        QMessageBox messageBox;
        messageBox.setWindowTitle("解压缩完成");
        messageBox.setText("✅ 文件解压缩成功完成！\n\n文件已保存到指定位置。");
        messageBox.setIcon(QMessageBox::Information);
        messageBox.setStandardButtons(QMessageBox::Ok);
        messageBox.exec();
    }

    msgBox.hide();
    msgBox.setWindowTitle("请稍候");
    msgBox.setText("正在处理中，请稍候...");
    ui->progressBar->setMaximum(100);
    ui->progressBar->setValue(100);
    QMessageBox::information(this, "提示", "🎉 操作已成功完成！", QMessageBox::Ok);
    ui->progressBar->hide();
    ui->pushButton->show();
    ui->progressBar->setValue(0);
    initialize_all();
}

void MainWindow::initialize_all() // bt_show不能改，因为选择按钮不变
{
    path_in_cor = 0;
    path_out_cor = 0;
    // 安全地清空字符串数组
    for (int i = 0; i < 256; i++) {
        zip->binary_coding[i].clear();
    }
    memset(zip->count_char, 0, sizeof(zip->count_char));
    zip->jud_binary_out_finished = 0;
    zip->vec_char_final.clear();
    zip->path_in.clear();
    zip->path_out.clear();
    zip->char_final = std::priority_queue<node *, std::vector<node *>, decltype(&hfutzip::compare)>();
    ui->lineEdit_2->setText(zip->path_out);
    ui->lineEdit->setText(zip->path_in);
}

MainWindow::~MainWindow()
{
    delete ui;
    // 调用quit()函数
    zip_thread->quit();
    // 调用wait()函数
    zip_thread->wait();
    delete zip_thread;
}

void MainWindow::on_pushButton_3_clicked()
{
    if (ui->lineEdit_3->bt_show == 1)
    {
        zip->path_in = QFileDialog::getOpenFileName(this, "打开文件", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
        ui->lineEdit->setText(zip->path_in);
        if (!zip->path_in.isEmpty())
        {
            QFileInfo temp(zip->path_in);
            // 获取文件的绝对路径
            QString path = temp.absoluteFilePath();
            // 获取文件的后缀
            QString suffix = temp.suffix();
            // 更改文件的后缀
            zip->path_out = path.replace(suffix, "hfut");
            ui->lineEdit_2->setText(zip->path_out);
            path_in_cor = 1;
            path_out_cor = 1;
        }
        else
        {
            initialize_all();
        }
    }
    else
    {
        zip->path_in = QFileDialog::getOpenFileName(this, "打开.hfut文件", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation), "HFUT Files (*.hfut)");
        ui->lineEdit->setText(zip->path_in);
        if (!zip->path_in.isEmpty())
        {
            QFileInfo temp(zip->path_in);
            // 获取文件的绝对路径
            QString path = temp.absoluteFilePath();
            // 获取文件的后缀
            QString suffix = temp.suffix();
            // 更改文件的后缀
            zip->path_out = path.replace(suffix, zip->get_suffix());
            ui->lineEdit_2->setText(zip->path_out);
            path_in_cor = 1;
            path_out_cor = 1;
        }
        else
        {
            initialize_all();
        }
    }
}

void MainWindow::on_pushButton_4_clicked()
{
    if (ui->lineEdit_3->bt_show == 1)
    {
        if (path_in_cor == 0)
        {
            QMessageBox::critical(this, "错误", "未选定输入的文件", QMessageBox::Ok);
            ui->lineEdit_2->setText("未选定输入的文件");
        }
        else
        {
            zip->path_out = QFileDialog::getExistingDirectory(this, "选择文件夹", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
            if (!zip->path_out.isEmpty())
            {
                zip->path_out += +"/" + QFileInfo(zip->path_in).baseName() + ".hfut";
                ui->lineEdit_2->setText(zip->path_out);
            }
            else
            {
                initialize_all();
            }
        }
    }

    else if (ui->lineEdit_3->bt_show == 0)
    {
        if (path_in_cor == 0)
        {
            QMessageBox::critical(this, "错误", "未选定输入的文件", QMessageBox::Ok);
            ui->lineEdit_2->setText("未选定输入的文件");
        }
        else
        {
            // zip->path_out = QFileDialog::getExistingDirectory(this, "选择文件夹", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation))+"/"+QFileInfo(zip->path_in).baseName()+"."+zip->get_suffix();
            zip->path_out = QFileDialog::getExistingDirectory(this, "选择文件夹", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
            if (!zip->path_out.isEmpty())
            {
                zip->path_out += "/" + QFileInfo(zip->path_in).baseName() + "." + zip->get_suffix();
                ui->lineEdit_2->setText(zip->path_out);
            }
            else
            {
                initialize_all();
            }
        }
    }
}

void MainWindow::on_pushButton_clicked()
{

    if (path_in_cor == 0 || path_out_cor == 0)
    {
        QMessageBox::critical(this, "错误", "未选定输入或者输出的路径", QMessageBox::Ok);
        initialize_all();
        return;
    }
    if (!QFile(zip->path_in).open(QIODevice::ReadOnly)) // 当QFile匿名对象销毁的时候，qfile会自动关闭
    {
        QMessageBox::critical(this, "错误", "文件不可读", QMessageBox::Ok);
        initialize_all();
        return;
    }

    if (QFile(zip->path_in).size() > INT_MAX - 1)
    {
        QMessageBox::warning(this, "警告", "文件太大了，文件大小应小于2147483KB（2.1GB）！！", QMessageBox::Ok);
        initialize_all();
        return;
    }

    // 注意：这是三选
    // 如果用户取消了选择或者发生了错误，则返回QMessageBox::Cancel。因此，如果用户直接点击右上角的关闭按钮，也相当于取消了选择，所以也会返回QMessageBox::Cancel。
    if (QFile(zip->path_out).exists()) // exists()函数只能检查文件是否存在，而不能检查文件是否可写。文件是否可写取决于文件的权限和属性，以及操作,比如文件已经打开，不可写，但是存在
    {
        QMessageBox::StandardButton result = QMessageBox::warning(this, "警告", "文件已存在，是否覆盖", QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
        if (result == QMessageBox::Cancel)
        {
            initialize_all();
            return;
        }
    }

    // 在这里文件已经创建，如果放在上面的上面，每次都会"警告", "文件已存在，是否覆盖"，但是上面的程序检测了if(!QFile(zip->path_out).open(QIODevice::WriteOnly))吗
    if (!QFile(zip->path_out).open(QIODevice::WriteOnly)) // 文件不存在会返回true吗，就是可写？但是open不了？ReadOnly这种情况一定返回false？
    {
        QMessageBox::critical(this, "错误", "无法写入文件", QMessageBox::Ok);
        initialize_all();
        return;
    }

    if (ui->lineEdit_3->bt_show == 1)
    {
        msgBox.show();

        //    下面这些没必要，msgBox阻塞了其他窗口
        //    zip->process_file_makezip();
        //    ui->pushButton_4->setEnabled(false);
        //    ui->pushButton->setEnabled(false);
        //    ui->pushButton_3->setEnabled(false);
        //    ui->lineEdit_3->setDragEnabled(false);

        // 进度条
        ui->progressBar->setMaximum(0);
        ui->pushButton->hide();
        ui->progressBar->show();
        emit make_zip();
    }
    else if (ui->lineEdit_3->bt_show == 0)
    {
        msgBox.show();
        ui->progressBar->setMaximum(0);
        ui->pushButton->hide();
        ui->progressBar->show();
        emit process_zip();
    }
}

void MainWindow::on_radioButton_clicked()
{
    ui->lineEdit_3->bt_show = 1;
    ui->pushButton->setText("开始压缩");
    initialize_all();
}

void MainWindow::on_radioButton_2_clicked()
{
    ui->lineEdit_3->bt_show = 0;
    ui->pushButton->setText("开始解压缩");
    initialize_all();
}

// 压缩级别选择槽函数
void MainWindow::on_compressionLevelComboBox_currentIndexChanged(int index)
{
    switch (index) {
        case 0: // 快速压缩
            zip->compression_level = LEVEL_FAST;
            ui->compressionLevelDescLabel->setText("快速压缩速度最快，适合临时文件");
            qDebug() << "用户选择快速压缩模式";
            break;
        case 1: // 智能压缩
            zip->compression_level = LEVEL_STANDARD;
            ui->compressionLevelDescLabel->setText("智能压缩自动选择最佳策略，推荐日常使用");
            qDebug() << "用户选择智能压缩模式";
            break;
        case 2: // 最大压缩
            zip->compression_level = LEVEL_MAXIMUM;
            ui->compressionLevelDescLabel->setText("最大压缩压缩比最高，适合长期存储");
            qDebug() << "用户选择最大压缩模式";
            break;
        default:
            zip->compression_level = LEVEL_STANDARD;
            break;
    }
}

// 计算压缩统计信息
void MainWindow::calculateCompressionStats()
{
    lastCompressionStats.originalSize = QFile(zip->path_in).size();
    lastCompressionStats.compressedSize = QFile(zip->path_out).size();
    lastCompressionStats.compressionRatio = (double)lastCompressionStats.compressedSize / lastCompressionStats.originalSize;
    
    // 获取实际使用的格式
    char format_type = zip->detect_file_format_type();
    switch (format_type) {
        case 'O':
            lastCompressionStats.formatUsed = "优化格式(O)";
            break;
        case 'U':
            lastCompressionStats.formatUsed = "超级优化格式(U)";
            break;
        default:
            lastCompressionStats.formatUsed = "原始格式";
            break;
    }
    
    // 计算节省的空间
    qint64 savedBytes = lastCompressionStats.originalSize - lastCompressionStats.compressedSize;
    if (savedBytes > 0) {
        lastCompressionStats.spaceSaved = QString("节省空间: %1").arg(formatFileSize(savedBytes));
    } else {
        lastCompressionStats.spaceSaved = QString("文件增大: %1").arg(formatFileSize(-savedBytes));
    }
}

// 显示压缩统计信息
void MainWindow::showCompressionStats()
{
    QString compressionPercent = QString::number(lastCompressionStats.compressionRatio * 100, 'f', 2) + "%";
    
    QMessageBox messageBox(this);
    messageBox.setWindowTitle("压缩完成");
    
    QString resultText = QString(
        "📊 压缩统计信息\n\n"
        "原始大小: %1\n"
        "压缩后大小: %2\n"
        "压缩率: %3\n"
        "使用格式: %4\n"
        "%5\n\n"
        "✅ 压缩成功完成！"
    ).arg(formatFileSize(lastCompressionStats.originalSize))
     .arg(formatFileSize(lastCompressionStats.compressedSize))
     .arg(compressionPercent)
     .arg(lastCompressionStats.formatUsed)
     .arg(lastCompressionStats.spaceSaved);
     
    messageBox.setText(resultText);
    messageBox.setIcon(QMessageBox::Information);
    messageBox.setStandardButtons(QMessageBox::Ok);
    messageBox.exec();
}

// 格式化文件大小
QString MainWindow::formatFileSize(qint64 bytes)
{
    if (bytes < 1024) {
        return QString("%1 字节").arg(bytes);
    } else if (bytes < 1024 * 1024) {
        return QString("%1 KB").arg(QString::number((double)bytes / 1024, 'f', 2));
    } else if (bytes < 1024 * 1024 * 1024) {
        return QString("%1 MB").arg(QString::number((double)bytes / (1024 * 1024), 'f', 2));
    } else {
        return QString("%1 GB").arg(QString::number((double)bytes / (1024 * 1024 * 1024), 'f', 2));
    }
}
