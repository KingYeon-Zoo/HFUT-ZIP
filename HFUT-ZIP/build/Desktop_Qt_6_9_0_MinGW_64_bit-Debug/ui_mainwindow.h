/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <fileedit.h>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *mainLayout;
    QLabel *titleLabel;
    QFrame *dragDropFrame;
    QVBoxLayout *dragDropLayout;
    FileEdit *lineEdit_3;
    QFrame *pathFrame;
    QVBoxLayout *pathLayout;
    QHBoxLayout *inputLayout;
    QLabel *inputLabel;
    QLineEdit *lineEdit;
    QPushButton *pushButton_3;
    QHBoxLayout *outputLayout;
    QLabel *outputLabel;
    QLineEdit *lineEdit_2;
    QPushButton *pushButton_4;
    QHBoxLayout *controlLayout;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QRadioButton *radioButton;
    QRadioButton *radioButton_2;
    QGroupBox *compressionLevelGroupBox;
    QVBoxLayout *compressionLevelLayout;
    QComboBox *compressionLevelComboBox;
    QLabel *compressionLevelDescLabel;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton;
    QProgressBar *progressBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 600);
        MainWindow->setMinimumSize(QSize(800, 600));
        MainWindow->setMaximumSize(QSize(800, 600));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        mainLayout = new QVBoxLayout(centralwidget);
        mainLayout->setSpacing(30);
        mainLayout->setObjectName("mainLayout");
        mainLayout->setContentsMargins(50, 50, 50, 50);
        titleLabel = new QLabel(centralwidget);
        titleLabel->setObjectName("titleLabel");
        titleLabel->setAlignment(Qt::AlignCenter);

        mainLayout->addWidget(titleLabel);

        dragDropFrame = new QFrame(centralwidget);
        dragDropFrame->setObjectName("dragDropFrame");
        dragDropFrame->setFrameShape(QFrame::StyledPanel);
        dragDropFrame->setFrameShadow(QFrame::Raised);
        dragDropLayout = new QVBoxLayout(dragDropFrame);
        dragDropLayout->setSpacing(20);
        dragDropLayout->setObjectName("dragDropLayout");
        dragDropLayout->setContentsMargins(40, 40, 40, 40);
        lineEdit_3 = new FileEdit(dragDropFrame);
        lineEdit_3->setObjectName("lineEdit_3");
        lineEdit_3->setReadOnly(true);
        lineEdit_3->setAlignment(Qt::AlignCenter);

        dragDropLayout->addWidget(lineEdit_3);


        mainLayout->addWidget(dragDropFrame);

        pathFrame = new QFrame(centralwidget);
        pathFrame->setObjectName("pathFrame");
        pathFrame->setFrameShape(QFrame::StyledPanel);
        pathFrame->setFrameShadow(QFrame::Raised);
        pathLayout = new QVBoxLayout(pathFrame);
        pathLayout->setSpacing(15);
        pathLayout->setObjectName("pathLayout");
        pathLayout->setContentsMargins(30, 30, 30, 30);
        inputLayout = new QHBoxLayout();
        inputLayout->setObjectName("inputLayout");
        inputLabel = new QLabel(pathFrame);
        inputLabel->setObjectName("inputLabel");

        inputLayout->addWidget(inputLabel);

        lineEdit = new QLineEdit(pathFrame);
        lineEdit->setObjectName("lineEdit");
        lineEdit->setReadOnly(true);

        inputLayout->addWidget(lineEdit);

        pushButton_3 = new QPushButton(pathFrame);
        pushButton_3->setObjectName("pushButton_3");

        inputLayout->addWidget(pushButton_3);


        pathLayout->addLayout(inputLayout);

        outputLayout = new QHBoxLayout();
        outputLayout->setObjectName("outputLayout");
        outputLabel = new QLabel(pathFrame);
        outputLabel->setObjectName("outputLabel");

        outputLayout->addWidget(outputLabel);

        lineEdit_2 = new QLineEdit(pathFrame);
        lineEdit_2->setObjectName("lineEdit_2");
        lineEdit_2->setReadOnly(true);

        outputLayout->addWidget(lineEdit_2);

        pushButton_4 = new QPushButton(pathFrame);
        pushButton_4->setObjectName("pushButton_4");

        outputLayout->addWidget(pushButton_4);


        pathLayout->addLayout(outputLayout);


        mainLayout->addWidget(pathFrame);

        controlLayout = new QHBoxLayout();
        controlLayout->setObjectName("controlLayout");
        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName("groupBox");
        verticalLayout = new QVBoxLayout(groupBox);
        verticalLayout->setObjectName("verticalLayout");
        radioButton = new QRadioButton(groupBox);
        radioButton->setObjectName("radioButton");
        radioButton->setAcceptDrops(false);
        radioButton->setChecked(true);

        verticalLayout->addWidget(radioButton);

        radioButton_2 = new QRadioButton(groupBox);
        radioButton_2->setObjectName("radioButton_2");

        verticalLayout->addWidget(radioButton_2);


        controlLayout->addWidget(groupBox);

        compressionLevelGroupBox = new QGroupBox(centralwidget);
        compressionLevelGroupBox->setObjectName("compressionLevelGroupBox");
        compressionLevelLayout = new QVBoxLayout(compressionLevelGroupBox);
        compressionLevelLayout->setObjectName("compressionLevelLayout");
        compressionLevelComboBox = new QComboBox(compressionLevelGroupBox);
        compressionLevelComboBox->addItem(QString());
        compressionLevelComboBox->addItem(QString());
        compressionLevelComboBox->addItem(QString());
        compressionLevelComboBox->setObjectName("compressionLevelComboBox");

        compressionLevelLayout->addWidget(compressionLevelComboBox);

        compressionLevelDescLabel = new QLabel(compressionLevelGroupBox);
        compressionLevelDescLabel->setObjectName("compressionLevelDescLabel");
        compressionLevelDescLabel->setWordWrap(true);

        compressionLevelLayout->addWidget(compressionLevelDescLabel);


        controlLayout->addWidget(compressionLevelGroupBox);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        controlLayout->addItem(horizontalSpacer);

        pushButton = new QPushButton(centralwidget);
        pushButton->setObjectName("pushButton");

        controlLayout->addWidget(pushButton);


        mainLayout->addLayout(controlLayout);

        progressBar = new QProgressBar(centralwidget);
        progressBar->setObjectName("progressBar");
        progressBar->setValue(0);

        mainLayout->addWidget(progressBar);

        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "HFUT \345\223\210\345\244\253\346\233\274\345\216\213\347\274\251\345\267\245\345\205\267", nullptr));
        titleLabel->setText(QCoreApplication::translate("MainWindow", "HFUT \345\223\210\345\244\253\346\233\274\345\216\213\347\274\251\345\267\245\345\205\267", nullptr));
        titleLabel->setStyleSheet(QCoreApplication::translate("MainWindow", "font-size: 28px; font-weight: bold; color: #333; margin-bottom: 20px;", nullptr));
        dragDropFrame->setStyleSheet(QCoreApplication::translate("MainWindow", "QFrame { border: 2px dashed #ccc; border-radius: 15px; background-color: #f8f9fa; }", nullptr));
        lineEdit_3->setText(QCoreApplication::translate("MainWindow", "\346\213\226\346\213\275\346\226\207\344\273\266\345\210\260\346\255\244\345\244\204\346\210\226\347\202\271\345\207\273\351\200\211\346\213\251\346\226\207\344\273\266", nullptr));
        lineEdit_3->setStyleSheet(QCoreApplication::translate("MainWindow", "border: none; background: transparent; font-size: 16px; color: #666;", nullptr));
        pathFrame->setStyleSheet(QCoreApplication::translate("MainWindow", "QFrame { border: 1px solid #ddd; border-radius: 10px; background-color: white; }", nullptr));
        inputLabel->setText(QCoreApplication::translate("MainWindow", "\350\276\223\345\205\245\346\226\207\344\273\266\357\274\232", nullptr));
        inputLabel->setStyleSheet(QCoreApplication::translate("MainWindow", "font-size: 14px; font-weight: bold; color: #333;", nullptr));
        lineEdit->setStyleSheet(QCoreApplication::translate("MainWindow", "border: 1px solid #ccc; border-radius: 5px; padding: 8px; font-size: 12px;", nullptr));
        pushButton_3->setText(QCoreApplication::translate("MainWindow", "\351\200\211\346\213\251\346\226\207\344\273\266", nullptr));
        pushButton_3->setStyleSheet(QCoreApplication::translate("MainWindow", "QPushButton { border: 1px solid #007AFF; border-radius: 8px; background-color: #007AFF; color: white; padding: 8px 16px; font-size: 12px; } QPushButton:hover { background-color: #0056CC; }", nullptr));
        outputLabel->setText(QCoreApplication::translate("MainWindow", "\350\276\223\345\207\272\350\267\257\345\276\204\357\274\232", nullptr));
        outputLabel->setStyleSheet(QCoreApplication::translate("MainWindow", "font-size: 14px; font-weight: bold; color: #333;", nullptr));
        lineEdit_2->setStyleSheet(QCoreApplication::translate("MainWindow", "border: 1px solid #ccc; border-radius: 5px; padding: 8px; font-size: 12px;", nullptr));
        pushButton_4->setText(QCoreApplication::translate("MainWindow", "\351\200\211\346\213\251\350\267\257\345\276\204", nullptr));
        pushButton_4->setStyleSheet(QCoreApplication::translate("MainWindow", "QPushButton { border: 1px solid #007AFF; border-radius: 8px; background-color: #007AFF; color: white; padding: 8px 16px; font-size: 12px; } QPushButton:hover { background-color: #0056CC; }", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "\346\223\215\344\275\234\346\250\241\345\274\217", nullptr));
        groupBox->setStyleSheet(QCoreApplication::translate("MainWindow", "QGroupBox { font-size: 14px; font-weight: bold; color: #333; border: 1px solid #ddd; border-radius: 8px; margin-top: 10px; padding-top: 10px; } QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }", nullptr));
        radioButton->setText(QCoreApplication::translate("MainWindow", "\345\216\213\347\274\251", nullptr));
        radioButton->setStyleSheet(QCoreApplication::translate("MainWindow", "font-size: 12px; color: #333;", nullptr));
        radioButton_2->setText(QCoreApplication::translate("MainWindow", "\350\247\243\345\216\213\347\274\251", nullptr));
        radioButton_2->setStyleSheet(QCoreApplication::translate("MainWindow", "font-size: 12px; color: #333;", nullptr));
        compressionLevelGroupBox->setTitle(QCoreApplication::translate("MainWindow", "\345\216\213\347\274\251\347\272\247\345\210\253", nullptr));
        compressionLevelGroupBox->setStyleSheet(QCoreApplication::translate("MainWindow", "QGroupBox { font-size: 14px; font-weight: bold; color: #333; border: 1px solid #ddd; border-radius: 8px; margin-top: 10px; padding-top: 10px; } QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }", nullptr));
        compressionLevelComboBox->setItemText(0, QCoreApplication::translate("MainWindow", "\345\277\253\351\200\237\345\216\213\347\274\251", nullptr));
        compressionLevelComboBox->setItemText(1, QCoreApplication::translate("MainWindow", "\346\231\272\350\203\275\345\216\213\347\274\251 (\346\216\250\350\215\220)", nullptr));
        compressionLevelComboBox->setItemText(2, QCoreApplication::translate("MainWindow", "\346\234\200\345\244\247\345\216\213\347\274\251", nullptr));

        compressionLevelComboBox->setStyleSheet(QCoreApplication::translate("MainWindow", "QComboBox { border: 1px solid #ccc; border-radius: 5px; padding: 5px; font-size: 12px; background-color: white; } QComboBox::drop-down { border: none; } QComboBox::down-arrow { image: none; border: none; }", nullptr));
        compressionLevelDescLabel->setText(QCoreApplication::translate("MainWindow", "\346\231\272\350\203\275\345\216\213\347\274\251\344\274\232\346\240\271\346\215\256\346\226\207\344\273\266\347\211\271\345\276\201\350\207\252\345\212\250\351\200\211\346\213\251\346\234\200\344\275\263\346\240\274\345\274\217", nullptr));
        compressionLevelDescLabel->setStyleSheet(QCoreApplication::translate("MainWindow", "font-size: 10px; color: #666; padding: 2px;", nullptr));
        pushButton->setText(QCoreApplication::translate("MainWindow", "\345\274\200\345\247\213\345\216\213\347\274\251", nullptr));
        pushButton->setStyleSheet(QCoreApplication::translate("MainWindow", "QPushButton { border: none; border-radius: 12px; background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #34C759, stop:1 #30A14E); color: white; padding: 12px 24px; font-size: 16px; font-weight: bold; } QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #30A14E, stop:1 #28A745); } QPushButton:pressed { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #28A745, stop:1 #1E7E34); }", nullptr));
        progressBar->setStyleSheet(QCoreApplication::translate("MainWindow", "QProgressBar { border: 1px solid #ddd; border-radius: 8px; background-color: #f0f0f0; text-align: center; } QProgressBar::chunk { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #007AFF, stop:1 #0056CC); border-radius: 7px; }", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
