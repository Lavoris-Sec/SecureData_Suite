/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.10.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QPushButton *surprise;
    QTabWidget *tabWidget;
    QWidget *tab;
    QGridLayout *gridLayout;
    QTextEdit *teInput;
    QLabel *label_Input;
    QLineEdit *leKey;
    QComboBox *comboMethods;
    QPushButton *btnEncrypt;
    QPushButton *btnDecrypt;
    QLabel *label_key;
    QTextBrowser *teOutput;
    QLabel *label;
    QWidget *tab_2;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        surprise = new QPushButton(centralwidget);
        surprise->setObjectName("surprise");
        surprise->setGeometry(QRect(770, 550, 20, 20));
        surprise->setFlat(true);
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName("tabWidget");
        tabWidget->setGeometry(QRect(0, 0, 801, 541));
        tab = new QWidget();
        tab->setObjectName("tab");
        gridLayout = new QGridLayout(tab);
        gridLayout->setObjectName("gridLayout");
        teInput = new QTextEdit(tab);
        teInput->setObjectName("teInput");

        gridLayout->addWidget(teInput, 2, 0, 1, 2);

        label_Input = new QLabel(tab);
        label_Input->setObjectName("label_Input");

        gridLayout->addWidget(label_Input, 0, 0, 1, 1);

        leKey = new QLineEdit(tab);
        leKey->setObjectName("leKey");

        gridLayout->addWidget(leKey, 2, 3, 1, 1);

        comboMethods = new QComboBox(tab);
        comboMethods->addItem(QString());
        comboMethods->addItem(QString());
        comboMethods->addItem(QString());
        comboMethods->addItem(QString());
        comboMethods->addItem(QString());
        comboMethods->addItem(QString());
        comboMethods->addItem(QString());
        comboMethods->addItem(QString());
        comboMethods->setObjectName("comboMethods");

        gridLayout->addWidget(comboMethods, 3, 4, 1, 1);

        btnEncrypt = new QPushButton(tab);
        btnEncrypt->setObjectName("btnEncrypt");

        gridLayout->addWidget(btnEncrypt, 4, 0, 1, 1);

        btnDecrypt = new QPushButton(tab);
        btnDecrypt->setObjectName("btnDecrypt");

        gridLayout->addWidget(btnDecrypt, 4, 3, 1, 1);

        label_key = new QLabel(tab);
        label_key->setObjectName("label_key");

        gridLayout->addWidget(label_key, 1, 3, 1, 1);

        teOutput = new QTextBrowser(tab);
        teOutput->setObjectName("teOutput");

        gridLayout->addWidget(teOutput, 2, 2, 1, 1);

        label = new QLabel(tab);
        label->setObjectName("label");

        gridLayout->addWidget(label, 1, 2, 1, 1);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName("tab_2");
        tabWidget->addTab(tab_2, QString());
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 25));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        surprise->setText(QString());
        label_Input->setText(QCoreApplication::translate("MainWindow", "Input text", nullptr));
        comboMethods->setItemText(0, QCoreApplication::translate("MainWindow", "Caesar", nullptr));
        comboMethods->setItemText(1, QCoreApplication::translate("MainWindow", "Vigenere", nullptr));
        comboMethods->setItemText(2, QCoreApplication::translate("MainWindow", "Atbash", nullptr));
        comboMethods->setItemText(3, QCoreApplication::translate("MainWindow", "XOR", nullptr));
        comboMethods->setItemText(4, QCoreApplication::translate("MainWindow", "Permutation", nullptr));
        comboMethods->setItemText(5, QCoreApplication::translate("MainWindow", "Simple AES", nullptr));
        comboMethods->setItemText(6, QCoreApplication::translate("MainWindow", "Huffman", nullptr));
        comboMethods->setItemText(7, QCoreApplication::translate("MainWindow", "RSA", nullptr));

        btnEncrypt->setText(QCoreApplication::translate("MainWindow", "ENCRYPT", nullptr));
        btnDecrypt->setText(QCoreApplication::translate("MainWindow", "DECRYPT", nullptr));
        label_key->setText(QCoreApplication::translate("MainWindow", "Key", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Result", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab), QCoreApplication::translate("MainWindow", "Tab 1", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QCoreApplication::translate("MainWindow", "Tab 2", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
