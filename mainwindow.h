#include "fileedit.h"
#include "hfutzip.h"
#include <QThread>
#include <QMessageBox>
#include <QLabel>
#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

// 压缩统计信息结构体
struct CompressionStats {
    qint64 originalSize;
    qint64 compressedSize;
    double compressionRatio;
    QString formatUsed;
    QString spaceSaved;
};

class MainWindow : public QMainWindow

{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QMessageBox msgBox; // 阻塞？
    hfutzip *zip;
    bool path_in_cor;
    bool path_out_cor;
    void initialize_all();
    QThread *zip_thread; // 在这里使用了多线程
    void zip_finished();

private:
    // 压缩统计信息
    CompressionStats lastCompressionStats;
    
    // 压缩统计相关函数
    void calculateCompressionStats();
    void showCompressionStats();
    QString formatFileSize(qint64 bytes);

private slots:
    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_clicked();

    void on_radioButton_clicked();

    void on_radioButton_2_clicked();

    // 压缩级别选择槽函数
    void on_compressionLevelComboBox_currentIndexChanged(int index);

signals:
    void make_zip();
    void process_zip();

private:
    Ui::MainWindow *ui;
};

#endif
