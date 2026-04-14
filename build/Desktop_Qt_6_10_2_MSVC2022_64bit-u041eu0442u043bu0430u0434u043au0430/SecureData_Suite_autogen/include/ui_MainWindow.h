/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created by: Qt User Interface Compiler version 6.10.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QTabWidget *tabWidget;
    QWidget *tabCrypto;
    QGridLayout *gridCrypto;
    QLabel *labelInput;
    QPushButton *btnSwap;
    QPlainTextEdit *textInput;
    QLabel *labelAlgorithm;
    QPushButton *btnEncrypt;
    QLabel *labelOutput;
    QLineEdit *lineKey;
    QPlainTextEdit *textOutput;
    QGroupBox *groupRsa;
    QGridLayout *gridRsa;
    QLabel *labelRsaPublic;
    QLineEdit *lineRsaPublic;
    QLabel *labelRsaPrivate;
    QLineEdit *lineRsaPrivate;
    QLabel *labelRsaBits;
    QSpinBox *spinRsaBits;
    QPushButton *btnRsaGenerate;
    QComboBox *comboCipher;
    QLabel *labelKey;
    QPushButton *btnDecrypt;
    QWidget *tabStego;
    QGridLayout *gridStego;
    QLabel *labelStegoInput;
    QLineEdit *lineStegoInput;
    QPushButton *btnStegoBrowseInput;
    QLabel *labelStegoOutput;
    QLineEdit *lineStegoOutput;
    QPushButton *btnStegoBrowseOutput;
    QLabel *labelStegoMessage;
    QPlainTextEdit *textStegoMessage;
    QPushButton *btnStegoEmbed;
    QPushButton *btnStegoExtract;
    QWidget *tabFiles;
    QGridLayout *gridFiles;
    QLabel *labelFileInput;
    QLineEdit *lineFileInput;
    QPushButton *btnFileBrowseInput;
    QLabel *labelFileOutput;
    QLineEdit *lineFileOutput;
    QPushButton *btnFileBrowseOutput;
    QCheckBox *checkCache;
    QLabel *labelCachePath;
    QPushButton *btnOpenCache;
    QPushButton *btnFileEncrypt;
    QPushButton *btnFileDecrypt;
    QWidget *tabAbout;
    QVBoxLayout *layoutAbout;
    QTextBrowser *textAbout;
    QHBoxLayout *layoutEasterButtons;
    QSpacerItem *horizontalSpacerAboutLeft;
    QPushButton *btnEggA;
    QPushButton *btnEggB;
    QSpacerItem *horizontalSpacerAboutRight;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(980, 720);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setObjectName("gridLayout");
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName("tabWidget");
        tabCrypto = new QWidget();
        tabCrypto->setObjectName("tabCrypto");
        gridCrypto = new QGridLayout(tabCrypto);
        gridCrypto->setObjectName("gridCrypto");
        labelInput = new QLabel(tabCrypto);
        labelInput->setObjectName("labelInput");

        gridCrypto->addWidget(labelInput, 3, 0, 1, 1);

        btnSwap = new QPushButton(tabCrypto);
        btnSwap->setObjectName("btnSwap");

        gridCrypto->addWidget(btnSwap, 9, 0, 1, 2);

        textInput = new QPlainTextEdit(tabCrypto);
        textInput->setObjectName("textInput");

        gridCrypto->addWidget(textInput, 4, 0, 1, 2);

        labelAlgorithm = new QLabel(tabCrypto);
        labelAlgorithm->setObjectName("labelAlgorithm");

        gridCrypto->addWidget(labelAlgorithm, 0, 0, 1, 1);

        btnEncrypt = new QPushButton(tabCrypto);
        btnEncrypt->setObjectName("btnEncrypt");

        gridCrypto->addWidget(btnEncrypt, 7, 0, 1, 2);

        labelOutput = new QLabel(tabCrypto);
        labelOutput->setObjectName("labelOutput");

        gridCrypto->addWidget(labelOutput, 5, 0, 1, 1);

        lineKey = new QLineEdit(tabCrypto);
        lineKey->setObjectName("lineKey");

        gridCrypto->addWidget(lineKey, 1, 1, 1, 1);

        textOutput = new QPlainTextEdit(tabCrypto);
        textOutput->setObjectName("textOutput");
        textOutput->setReadOnly(true);

        gridCrypto->addWidget(textOutput, 6, 0, 1, 2);

        groupRsa = new QGroupBox(tabCrypto);
        groupRsa->setObjectName("groupRsa");
        gridRsa = new QGridLayout(groupRsa);
        gridRsa->setObjectName("gridRsa");
        labelRsaPublic = new QLabel(groupRsa);
        labelRsaPublic->setObjectName("labelRsaPublic");

        gridRsa->addWidget(labelRsaPublic, 0, 0, 1, 1);

        lineRsaPublic = new QLineEdit(groupRsa);
        lineRsaPublic->setObjectName("lineRsaPublic");

        gridRsa->addWidget(lineRsaPublic, 0, 1, 1, 1);

        labelRsaPrivate = new QLabel(groupRsa);
        labelRsaPrivate->setObjectName("labelRsaPrivate");

        gridRsa->addWidget(labelRsaPrivate, 1, 0, 1, 1);

        lineRsaPrivate = new QLineEdit(groupRsa);
        lineRsaPrivate->setObjectName("lineRsaPrivate");

        gridRsa->addWidget(lineRsaPrivate, 1, 1, 1, 1);

        labelRsaBits = new QLabel(groupRsa);
        labelRsaBits->setObjectName("labelRsaBits");

        gridRsa->addWidget(labelRsaBits, 2, 0, 1, 1);

        spinRsaBits = new QSpinBox(groupRsa);
        spinRsaBits->setObjectName("spinRsaBits");
        spinRsaBits->setMinimum(32);
        spinRsaBits->setMaximum(56);
        spinRsaBits->setSingleStep(4);
        spinRsaBits->setValue(48);

        gridRsa->addWidget(spinRsaBits, 2, 1, 1, 1);

        btnRsaGenerate = new QPushButton(groupRsa);
        btnRsaGenerate->setObjectName("btnRsaGenerate");

        gridRsa->addWidget(btnRsaGenerate, 3, 1, 1, 1);


        gridCrypto->addWidget(groupRsa, 2, 0, 1, 2);

        comboCipher = new QComboBox(tabCrypto);
        comboCipher->setObjectName("comboCipher");

        gridCrypto->addWidget(comboCipher, 0, 1, 1, 1);

        labelKey = new QLabel(tabCrypto);
        labelKey->setObjectName("labelKey");

        gridCrypto->addWidget(labelKey, 1, 0, 1, 1);

        btnDecrypt = new QPushButton(tabCrypto);
        btnDecrypt->setObjectName("btnDecrypt");

        gridCrypto->addWidget(btnDecrypt, 8, 0, 1, 2);

        tabWidget->addTab(tabCrypto, QString());
        tabStego = new QWidget();
        tabStego->setObjectName("tabStego");
        gridStego = new QGridLayout(tabStego);
        gridStego->setObjectName("gridStego");
        labelStegoInput = new QLabel(tabStego);
        labelStegoInput->setObjectName("labelStegoInput");

        gridStego->addWidget(labelStegoInput, 0, 0, 1, 1);

        lineStegoInput = new QLineEdit(tabStego);
        lineStegoInput->setObjectName("lineStegoInput");

        gridStego->addWidget(lineStegoInput, 0, 1, 1, 1);

        btnStegoBrowseInput = new QPushButton(tabStego);
        btnStegoBrowseInput->setObjectName("btnStegoBrowseInput");

        gridStego->addWidget(btnStegoBrowseInput, 0, 2, 1, 1);

        labelStegoOutput = new QLabel(tabStego);
        labelStegoOutput->setObjectName("labelStegoOutput");

        gridStego->addWidget(labelStegoOutput, 1, 0, 1, 1);

        lineStegoOutput = new QLineEdit(tabStego);
        lineStegoOutput->setObjectName("lineStegoOutput");

        gridStego->addWidget(lineStegoOutput, 1, 1, 1, 1);

        btnStegoBrowseOutput = new QPushButton(tabStego);
        btnStegoBrowseOutput->setObjectName("btnStegoBrowseOutput");

        gridStego->addWidget(btnStegoBrowseOutput, 1, 2, 1, 1);

        labelStegoMessage = new QLabel(tabStego);
        labelStegoMessage->setObjectName("labelStegoMessage");

        gridStego->addWidget(labelStegoMessage, 2, 0, 1, 3);

        textStegoMessage = new QPlainTextEdit(tabStego);
        textStegoMessage->setObjectName("textStegoMessage");

        gridStego->addWidget(textStegoMessage, 3, 0, 1, 3);

        btnStegoEmbed = new QPushButton(tabStego);
        btnStegoEmbed->setObjectName("btnStegoEmbed");

        gridStego->addWidget(btnStegoEmbed, 4, 0, 1, 1);

        btnStegoExtract = new QPushButton(tabStego);
        btnStegoExtract->setObjectName("btnStegoExtract");

        gridStego->addWidget(btnStegoExtract, 4, 1, 1, 1);

        tabWidget->addTab(tabStego, QString());
        tabFiles = new QWidget();
        tabFiles->setObjectName("tabFiles");
        gridFiles = new QGridLayout(tabFiles);
        gridFiles->setObjectName("gridFiles");
        labelFileInput = new QLabel(tabFiles);
        labelFileInput->setObjectName("labelFileInput");

        gridFiles->addWidget(labelFileInput, 0, 0, 1, 1);

        lineFileInput = new QLineEdit(tabFiles);
        lineFileInput->setObjectName("lineFileInput");

        gridFiles->addWidget(lineFileInput, 0, 1, 1, 1);

        btnFileBrowseInput = new QPushButton(tabFiles);
        btnFileBrowseInput->setObjectName("btnFileBrowseInput");

        gridFiles->addWidget(btnFileBrowseInput, 0, 2, 1, 1);

        labelFileOutput = new QLabel(tabFiles);
        labelFileOutput->setObjectName("labelFileOutput");

        gridFiles->addWidget(labelFileOutput, 1, 0, 1, 1);

        lineFileOutput = new QLineEdit(tabFiles);
        lineFileOutput->setObjectName("lineFileOutput");

        gridFiles->addWidget(lineFileOutput, 1, 1, 1, 1);

        btnFileBrowseOutput = new QPushButton(tabFiles);
        btnFileBrowseOutput->setObjectName("btnFileBrowseOutput");

        gridFiles->addWidget(btnFileBrowseOutput, 1, 2, 1, 1);

        checkCache = new QCheckBox(tabFiles);
        checkCache->setObjectName("checkCache");
        checkCache->setChecked(true);

        gridFiles->addWidget(checkCache, 2, 0, 1, 1);

        labelCachePath = new QLabel(tabFiles);
        labelCachePath->setObjectName("labelCachePath");

        gridFiles->addWidget(labelCachePath, 2, 1, 1, 1);

        btnOpenCache = new QPushButton(tabFiles);
        btnOpenCache->setObjectName("btnOpenCache");

        gridFiles->addWidget(btnOpenCache, 2, 2, 1, 1);

        btnFileEncrypt = new QPushButton(tabFiles);
        btnFileEncrypt->setObjectName("btnFileEncrypt");

        gridFiles->addWidget(btnFileEncrypt, 3, 0, 1, 1);

        btnFileDecrypt = new QPushButton(tabFiles);
        btnFileDecrypt->setObjectName("btnFileDecrypt");

        gridFiles->addWidget(btnFileDecrypt, 3, 1, 1, 1);

        tabWidget->addTab(tabFiles, QString());
        tabAbout = new QWidget();
        tabAbout->setObjectName("tabAbout");
        layoutAbout = new QVBoxLayout(tabAbout);
        layoutAbout->setObjectName("layoutAbout");
        textAbout = new QTextBrowser(tabAbout);
        textAbout->setObjectName("textAbout");

        layoutAbout->addWidget(textAbout);

        layoutEasterButtons = new QHBoxLayout();
        layoutEasterButtons->setObjectName("layoutEasterButtons");
        horizontalSpacerAboutLeft = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutEasterButtons->addItem(horizontalSpacerAboutLeft);

        btnEggA = new QPushButton(tabAbout);
        btnEggA->setObjectName("btnEggA");

        layoutEasterButtons->addWidget(btnEggA);

        btnEggB = new QPushButton(tabAbout);
        btnEggB->setObjectName("btnEggB");

        layoutEasterButtons->addWidget(btnEggB);

        horizontalSpacerAboutRight = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutEasterButtons->addItem(horizontalSpacerAboutRight);


        layoutAbout->addLayout(layoutEasterButtons);

        tabWidget->addTab(tabAbout, QString());

        gridLayout->addWidget(tabWidget, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 980, 25));
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
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "SecureData Suite", nullptr));
        labelInput->setText(QCoreApplication::translate("MainWindow", "Input text", nullptr));
        btnSwap->setText(QCoreApplication::translate("MainWindow", "Swap input/output", nullptr));
        labelAlgorithm->setText(QCoreApplication::translate("MainWindow", "Algorithm", nullptr));
        btnEncrypt->setText(QCoreApplication::translate("MainWindow", "Encrypt", nullptr));
        labelOutput->setText(QCoreApplication::translate("MainWindow", "Result", nullptr));
        groupRsa->setTitle(QCoreApplication::translate("MainWindow", "RSA keys", nullptr));
        labelRsaPublic->setText(QCoreApplication::translate("MainWindow", "Public (e,n)", nullptr));
        labelRsaPrivate->setText(QCoreApplication::translate("MainWindow", "Private (d,n)", nullptr));
        labelRsaBits->setText(QCoreApplication::translate("MainWindow", "Bits", nullptr));
        btnRsaGenerate->setText(QCoreApplication::translate("MainWindow", "Generate", nullptr));
        labelKey->setText(QCoreApplication::translate("MainWindow", "Key", nullptr));
        btnDecrypt->setText(QCoreApplication::translate("MainWindow", "Decrypt", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabCrypto), QCoreApplication::translate("MainWindow", "Cryptography", nullptr));
        labelStegoInput->setText(QCoreApplication::translate("MainWindow", "Input image", nullptr));
        btnStegoBrowseInput->setText(QCoreApplication::translate("MainWindow", "Browse", nullptr));
        labelStegoOutput->setText(QCoreApplication::translate("MainWindow", "Output image", nullptr));
        btnStegoBrowseOutput->setText(QCoreApplication::translate("MainWindow", "Save as", nullptr));
        labelStegoMessage->setText(QCoreApplication::translate("MainWindow", "Message", nullptr));
        btnStegoEmbed->setText(QCoreApplication::translate("MainWindow", "Embed", nullptr));
        btnStegoExtract->setText(QCoreApplication::translate("MainWindow", "Extract", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabStego), QCoreApplication::translate("MainWindow", "Steganography", nullptr));
        labelFileInput->setText(QCoreApplication::translate("MainWindow", "Input file", nullptr));
        btnFileBrowseInput->setText(QCoreApplication::translate("MainWindow", "Browse", nullptr));
        labelFileOutput->setText(QCoreApplication::translate("MainWindow", "Output file", nullptr));
        btnFileBrowseOutput->setText(QCoreApplication::translate("MainWindow", "Save as", nullptr));
        checkCache->setText(QCoreApplication::translate("MainWindow", "Save encrypted copy to cache", nullptr));
        labelCachePath->setText(QCoreApplication::translate("MainWindow", "cache", nullptr));
        btnOpenCache->setText(QCoreApplication::translate("MainWindow", "Open cache", nullptr));
        btnFileEncrypt->setText(QCoreApplication::translate("MainWindow", "Encrypt file", nullptr));
        btnFileDecrypt->setText(QCoreApplication::translate("MainWindow", "Decrypt file", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabFiles), QCoreApplication::translate("MainWindow", "Files", nullptr));
        textAbout->setHtml(QCoreApplication::translate("MainWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><meta charset=\"utf-8\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Segoe UI'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<h2 style=\" margin:0 0 8px 0;\"><span style=\" font-size:20pt; font-weight:700; color:#93C5FD;\">SecureData Suite</span></h2>\n"
"<p style=\" margin:0 0 10px 0;\">Educational toolkit for cryptography and steganography experiments.</p>\n"
"<ul style=\"margin-top: 0px; margin-bottom: 0px;\">\n"
"<li>8 cryptographic algorithms</li>\n"
"<li>LSB steganography for PNG/BMP</li>\n"
"<li>File encryption with automatic cache</li>\n"
"</ul>\n"
"<p style=\" margin:10px 0 0 0; color:#94A3B8;\">Hint: there are two hidden utilities below.</p>\n"
"</body></html>", nullptr));
        btnEggA->setText(QCoreApplication::translate("MainWindow", "Sponsor A", nullptr));
#if QT_CONFIG(tooltip)
        btnEggA->setToolTip(QCoreApplication::translate("MainWindow", "Launch external utility A", nullptr));
#endif // QT_CONFIG(tooltip)
        btnEggB->setText(QCoreApplication::translate("MainWindow", "Sponsor B", nullptr));
#if QT_CONFIG(tooltip)
        btnEggB->setToolTip(QCoreApplication::translate("MainWindow", "Launch external utility B", nullptr));
#endif // QT_CONFIG(tooltip)
        tabWidget->setTabText(tabWidget->indexOf(tabAbout), QCoreApplication::translate("MainWindow", "About", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
