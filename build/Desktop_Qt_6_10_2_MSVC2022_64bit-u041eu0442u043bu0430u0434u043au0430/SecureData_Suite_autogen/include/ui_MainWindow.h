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
#include <QtWidgets/QProgressBar>
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
    QVBoxLayout *layoutInputArea;
    QPlainTextEdit *textInput;
    QHBoxLayout *layoutInputActions;
    QSpacerItem *spacerInputActions;
    QPushButton *btnCopyInput;
    QLabel *labelAlgorithm;
    QPushButton *btnEncrypt;
    QLabel *labelOutput;
    QLineEdit *lineKey;
    QVBoxLayout *layoutOutputArea;
    QPlainTextEdit *textOutput;
    QHBoxLayout *layoutOutputActions;
    QSpacerItem *spacerOutputActions;
    QPushButton *btnCopyOutput;
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
    QVBoxLayout *layoutFiles;
    QGroupBox *groupFilePaths;
    QGridLayout *gridFilePaths;
    QLabel *labelFileInput;
    QLineEdit *lineFileInput;
    QPushButton *btnFileBrowseInput;
    QLabel *labelFileOutput;
    QLineEdit *lineFileOutput;
    QPushButton *btnFileBrowseOutput;
    QGroupBox *groupFileCache;
    QGridLayout *gridFileCache;
    QCheckBox *checkCache;
    QLabel *labelCachePath;
    QPushButton *btnOpenCache;
    QHBoxLayout *layoutFileActions;
    QPushButton *btnFileEncrypt;
    QPushButton *btnFileDecrypt;
    QSpacerItem *verticalSpacerFilesBottom;
    QWidget *tabSettings;
    QVBoxLayout *layoutSettings;
    QGroupBox *groupAchievements;
    QVBoxLayout *layoutAchievementsCard;
    QLabel *labelSettingsHint;
    QComboBox *comboAchievements;
    QProgressBar *progressAchievement;
    QLabel *labelAchievementStatus;
    QHBoxLayout *layoutAchievementButtons;
    QPushButton *btnRefreshAchievements;
    QPushButton *btnResetAchievements;
    QGroupBox *groupSecurityMode;
    QHBoxLayout *layoutSecurityModeCard;
    QSpacerItem *horizontalSpacerModeLeft;
    QLabel *labelModeEducation;
    QCheckBox *switchSecurityMode;
    QLabel *labelModeStrong;
    QSpacerItem *horizontalSpacerModeRight;
    QGroupBox *groupPreferences;
    QVBoxLayout *layoutPreferences;
    QCheckBox *checkDefaultCacheEnabled;
    QCheckBox *checkConfirmResetAchievements;
    QCheckBox *checkCompactMode;
    QCheckBox *checkReducedAnimations;
    QSpacerItem *verticalSpacerBottomSettings;
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
        MainWindow->resize(980, 735);
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

        layoutInputArea = new QVBoxLayout();
        layoutInputArea->setSpacing(6);
        layoutInputArea->setObjectName("layoutInputArea");
        textInput = new QPlainTextEdit(tabCrypto);
        textInput->setObjectName("textInput");

        layoutInputArea->addWidget(textInput);

        layoutInputActions = new QHBoxLayout();
        layoutInputActions->setObjectName("layoutInputActions");
        spacerInputActions = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutInputActions->addItem(spacerInputActions);

        btnCopyInput = new QPushButton(tabCrypto);
        btnCopyInput->setObjectName("btnCopyInput");

        layoutInputActions->addWidget(btnCopyInput);


        layoutInputArea->addLayout(layoutInputActions);


        gridCrypto->addLayout(layoutInputArea, 4, 0, 1, 2);

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

        layoutOutputArea = new QVBoxLayout();
        layoutOutputArea->setSpacing(6);
        layoutOutputArea->setObjectName("layoutOutputArea");
        textOutput = new QPlainTextEdit(tabCrypto);
        textOutput->setObjectName("textOutput");
        textOutput->setReadOnly(true);

        layoutOutputArea->addWidget(textOutput);

        layoutOutputActions = new QHBoxLayout();
        layoutOutputActions->setObjectName("layoutOutputActions");
        spacerOutputActions = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutOutputActions->addItem(spacerOutputActions);

        btnCopyOutput = new QPushButton(tabCrypto);
        btnCopyOutput->setObjectName("btnCopyOutput");

        layoutOutputActions->addWidget(btnCopyOutput);


        layoutOutputArea->addLayout(layoutOutputActions);


        gridCrypto->addLayout(layoutOutputArea, 6, 0, 1, 2);

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

        gridStego->addWidget(btnStegoExtract, 4, 1, 1, 2);

        tabWidget->addTab(tabStego, QString());
        tabFiles = new QWidget();
        tabFiles->setObjectName("tabFiles");
        layoutFiles = new QVBoxLayout(tabFiles);
        layoutFiles->setSpacing(8);
        layoutFiles->setObjectName("layoutFiles");
        layoutFiles->setContentsMargins(10, 10, 10, 8);
        groupFilePaths = new QGroupBox(tabFiles);
        groupFilePaths->setObjectName("groupFilePaths");
        gridFilePaths = new QGridLayout(groupFilePaths);
        gridFilePaths->setObjectName("gridFilePaths");
        gridFilePaths->setHorizontalSpacing(8);
        gridFilePaths->setVerticalSpacing(8);
        labelFileInput = new QLabel(groupFilePaths);
        labelFileInput->setObjectName("labelFileInput");

        gridFilePaths->addWidget(labelFileInput, 0, 0, 1, 1);

        lineFileInput = new QLineEdit(groupFilePaths);
        lineFileInput->setObjectName("lineFileInput");

        gridFilePaths->addWidget(lineFileInput, 0, 1, 1, 1);

        btnFileBrowseInput = new QPushButton(groupFilePaths);
        btnFileBrowseInput->setObjectName("btnFileBrowseInput");
        btnFileBrowseInput->setMinimumSize(QSize(0, 34));

        gridFilePaths->addWidget(btnFileBrowseInput, 0, 2, 1, 1);

        labelFileOutput = new QLabel(groupFilePaths);
        labelFileOutput->setObjectName("labelFileOutput");

        gridFilePaths->addWidget(labelFileOutput, 1, 0, 1, 1);

        lineFileOutput = new QLineEdit(groupFilePaths);
        lineFileOutput->setObjectName("lineFileOutput");

        gridFilePaths->addWidget(lineFileOutput, 1, 1, 1, 1);

        btnFileBrowseOutput = new QPushButton(groupFilePaths);
        btnFileBrowseOutput->setObjectName("btnFileBrowseOutput");
        btnFileBrowseOutput->setMinimumSize(QSize(0, 34));

        gridFilePaths->addWidget(btnFileBrowseOutput, 1, 2, 1, 1);


        layoutFiles->addWidget(groupFilePaths);

        groupFileCache = new QGroupBox(tabFiles);
        groupFileCache->setObjectName("groupFileCache");
        gridFileCache = new QGridLayout(groupFileCache);
        gridFileCache->setObjectName("gridFileCache");
        gridFileCache->setHorizontalSpacing(8);
        gridFileCache->setVerticalSpacing(8);
        checkCache = new QCheckBox(groupFileCache);
        checkCache->setObjectName("checkCache");
        checkCache->setChecked(true);

        gridFileCache->addWidget(checkCache, 0, 0, 1, 1);

        labelCachePath = new QLabel(groupFileCache);
        labelCachePath->setObjectName("labelCachePath");

        gridFileCache->addWidget(labelCachePath, 0, 1, 1, 1);

        btnOpenCache = new QPushButton(groupFileCache);
        btnOpenCache->setObjectName("btnOpenCache");
        btnOpenCache->setMinimumSize(QSize(0, 34));

        gridFileCache->addWidget(btnOpenCache, 0, 2, 1, 1);


        layoutFiles->addWidget(groupFileCache);

        layoutFileActions = new QHBoxLayout();
        layoutFileActions->setSpacing(8);
        layoutFileActions->setObjectName("layoutFileActions");
        btnFileEncrypt = new QPushButton(tabFiles);
        btnFileEncrypt->setObjectName("btnFileEncrypt");
        btnFileEncrypt->setMinimumSize(QSize(0, 36));

        layoutFileActions->addWidget(btnFileEncrypt);

        btnFileDecrypt = new QPushButton(tabFiles);
        btnFileDecrypt->setObjectName("btnFileDecrypt");
        btnFileDecrypt->setMinimumSize(QSize(0, 36));

        layoutFileActions->addWidget(btnFileDecrypt);


        layoutFiles->addLayout(layoutFileActions);

        verticalSpacerFilesBottom = new QSpacerItem(20, 1, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        layoutFiles->addItem(verticalSpacerFilesBottom);

        tabWidget->addTab(tabFiles, QString());
        tabSettings = new QWidget();
        tabSettings->setObjectName("tabSettings");
        layoutSettings = new QVBoxLayout(tabSettings);
        layoutSettings->setSpacing(8);
        layoutSettings->setObjectName("layoutSettings");
        layoutSettings->setContentsMargins(10, 10, 10, 8);
        groupAchievements = new QGroupBox(tabSettings);
        groupAchievements->setObjectName("groupAchievements");
        layoutAchievementsCard = new QVBoxLayout(groupAchievements);
        layoutAchievementsCard->setSpacing(10);
        layoutAchievementsCard->setObjectName("layoutAchievementsCard");
        layoutAchievementsCard->setContentsMargins(12, 14, 12, 12);
        labelSettingsHint = new QLabel(groupAchievements);
        labelSettingsHint->setObjectName("labelSettingsHint");

        layoutAchievementsCard->addWidget(labelSettingsHint);

        comboAchievements = new QComboBox(groupAchievements);
        comboAchievements->setObjectName("comboAchievements");

        layoutAchievementsCard->addWidget(comboAchievements);

        progressAchievement = new QProgressBar(groupAchievements);
        progressAchievement->setObjectName("progressAchievement");
        progressAchievement->setMinimum(0);
        progressAchievement->setMaximum(100);
        progressAchievement->setValue(0);

        layoutAchievementsCard->addWidget(progressAchievement);

        labelAchievementStatus = new QLabel(groupAchievements);
        labelAchievementStatus->setObjectName("labelAchievementStatus");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(labelAchievementStatus->sizePolicy().hasHeightForWidth());
        labelAchievementStatus->setSizePolicy(sizePolicy);
        labelAchievementStatus->setMinimumSize(QSize(0, 38));
        labelAchievementStatus->setWordWrap(true);

        layoutAchievementsCard->addWidget(labelAchievementStatus);

        layoutAchievementButtons = new QHBoxLayout();
        layoutAchievementButtons->setSpacing(8);
        layoutAchievementButtons->setObjectName("layoutAchievementButtons");
        btnRefreshAchievements = new QPushButton(groupAchievements);
        btnRefreshAchievements->setObjectName("btnRefreshAchievements");
        btnRefreshAchievements->setMinimumSize(QSize(0, 36));

        layoutAchievementButtons->addWidget(btnRefreshAchievements);

        btnResetAchievements = new QPushButton(groupAchievements);
        btnResetAchievements->setObjectName("btnResetAchievements");
        btnResetAchievements->setMinimumSize(QSize(0, 36));

        layoutAchievementButtons->addWidget(btnResetAchievements);


        layoutAchievementsCard->addLayout(layoutAchievementButtons);


        layoutSettings->addWidget(groupAchievements);

        groupSecurityMode = new QGroupBox(tabSettings);
        groupSecurityMode->setObjectName("groupSecurityMode");
        layoutSecurityModeCard = new QHBoxLayout(groupSecurityMode);
        layoutSecurityModeCard->setObjectName("layoutSecurityModeCard");
        layoutSecurityModeCard->setContentsMargins(12, 12, 12, 12);
        horizontalSpacerModeLeft = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutSecurityModeCard->addItem(horizontalSpacerModeLeft);

        labelModeEducation = new QLabel(groupSecurityMode);
        labelModeEducation->setObjectName("labelModeEducation");
        labelModeEducation->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        layoutSecurityModeCard->addWidget(labelModeEducation);

        switchSecurityMode = new QCheckBox(groupSecurityMode);
        switchSecurityMode->setObjectName("switchSecurityMode");

        layoutSecurityModeCard->addWidget(switchSecurityMode);

        labelModeStrong = new QLabel(groupSecurityMode);
        labelModeStrong->setObjectName("labelModeStrong");
        labelModeStrong->setAlignment(Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);

        layoutSecurityModeCard->addWidget(labelModeStrong);

        horizontalSpacerModeRight = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutSecurityModeCard->addItem(horizontalSpacerModeRight);


        layoutSettings->addWidget(groupSecurityMode);

        groupPreferences = new QGroupBox(tabSettings);
        groupPreferences->setObjectName("groupPreferences");
        layoutPreferences = new QVBoxLayout(groupPreferences);
        layoutPreferences->setSpacing(8);
        layoutPreferences->setObjectName("layoutPreferences");
        layoutPreferences->setContentsMargins(12, 12, 12, 12);
        checkDefaultCacheEnabled = new QCheckBox(groupPreferences);
        checkDefaultCacheEnabled->setObjectName("checkDefaultCacheEnabled");

        layoutPreferences->addWidget(checkDefaultCacheEnabled);

        checkConfirmResetAchievements = new QCheckBox(groupPreferences);
        checkConfirmResetAchievements->setObjectName("checkConfirmResetAchievements");

        layoutPreferences->addWidget(checkConfirmResetAchievements);

        checkCompactMode = new QCheckBox(groupPreferences);
        checkCompactMode->setObjectName("checkCompactMode");

        layoutPreferences->addWidget(checkCompactMode);

        checkReducedAnimations = new QCheckBox(groupPreferences);
        checkReducedAnimations->setObjectName("checkReducedAnimations");

        layoutPreferences->addWidget(checkReducedAnimations);


        layoutSettings->addWidget(groupPreferences);

        verticalSpacerBottomSettings = new QSpacerItem(20, 1, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        layoutSettings->addItem(verticalSpacerBottomSettings);

        tabWidget->addTab(tabSettings, QString());
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

        tabWidget->setCurrentIndex(3);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "SecureData Suite", nullptr));
        labelInput->setText(QCoreApplication::translate("MainWindow", "Input text", nullptr));
        btnSwap->setText(QCoreApplication::translate("MainWindow", "Swap input/output", nullptr));
        btnCopyInput->setText(QCoreApplication::translate("MainWindow", "Copy", nullptr));
        labelAlgorithm->setText(QCoreApplication::translate("MainWindow", "Algorithm", nullptr));
        btnEncrypt->setText(QCoreApplication::translate("MainWindow", "Encrypt", nullptr));
        labelOutput->setText(QCoreApplication::translate("MainWindow", "Result", nullptr));
        btnCopyOutput->setText(QCoreApplication::translate("MainWindow", "Copy", nullptr));
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
        groupFilePaths->setTitle(QCoreApplication::translate("MainWindow", "File paths", nullptr));
        labelFileInput->setText(QCoreApplication::translate("MainWindow", "Input file", nullptr));
        btnFileBrowseInput->setText(QCoreApplication::translate("MainWindow", "Browse", nullptr));
        labelFileOutput->setText(QCoreApplication::translate("MainWindow", "Output file", nullptr));
        btnFileBrowseOutput->setText(QCoreApplication::translate("MainWindow", "Save as", nullptr));
        groupFileCache->setTitle(QCoreApplication::translate("MainWindow", "Cache", nullptr));
        checkCache->setText(QCoreApplication::translate("MainWindow", "Save encrypted copy to cache", nullptr));
        labelCachePath->setText(QCoreApplication::translate("MainWindow", "cache", nullptr));
        btnOpenCache->setText(QCoreApplication::translate("MainWindow", "Open cache", nullptr));
        btnFileEncrypt->setText(QCoreApplication::translate("MainWindow", "Encrypt file", nullptr));
        btnFileDecrypt->setText(QCoreApplication::translate("MainWindow", "Decrypt file", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabFiles), QCoreApplication::translate("MainWindow", "Files", nullptr));
        groupAchievements->setTitle(QCoreApplication::translate("MainWindow", "Achievements", nullptr));
        labelSettingsHint->setText(QCoreApplication::translate("MainWindow", "Achievements", nullptr));
        labelAchievementStatus->setText(QCoreApplication::translate("MainWindow", "Locked", nullptr));
        btnRefreshAchievements->setText(QCoreApplication::translate("MainWindow", "Refresh", nullptr));
        btnResetAchievements->setText(QCoreApplication::translate("MainWindow", "Clear achievements", nullptr));
        groupSecurityMode->setTitle(QCoreApplication::translate("MainWindow", "Security mode", nullptr));
        labelModeEducation->setText(QCoreApplication::translate("MainWindow", "Education mode", nullptr));
        switchSecurityMode->setText(QString());
        labelModeStrong->setText(QCoreApplication::translate("MainWindow", "Strong mode", nullptr));
        groupPreferences->setTitle(QCoreApplication::translate("MainWindow", "Preferences", nullptr));
        checkDefaultCacheEnabled->setText(QCoreApplication::translate("MainWindow", "Enable file cache by default", nullptr));
        checkConfirmResetAchievements->setText(QCoreApplication::translate("MainWindow", "Ask confirmation before clearing achievements", nullptr));
        checkCompactMode->setText(QCoreApplication::translate("MainWindow", "Compact mode", nullptr));
        checkReducedAnimations->setText(QCoreApplication::translate("MainWindow", "Reduced animations", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabSettings), QCoreApplication::translate("MainWindow", "Settings", nullptr));
        textAbout->setHtml(QCoreApplication::translate("MainWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><meta charset=\"utf-8\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"hr { height: 1px; border-width: 0; }\n"
"li.unchecked::marker { content: \"\\2610\"; }\n"
"li.checked::marker { content: \"\\2612\"; }\n"
"</style></head><body style=\" font-family:'Segoe UI'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<h2 style=\" margin-top:0px; margin-bottom:8px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:20pt; font-weight:700; color:#93c5fd;\">SecureData Suite</span></h2>\n"
"<p style=\" margin-top:0px; margin-bottom:10px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">Educational toolkit for cryptography and steganography experiments.</span></p>\n"
"<ul style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; "
                        "margin-right: 0px; -qt-list-indent: 1;\">\n"
"<li style=\" font-size:10pt;\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">8 cryptographic algorithms</li>\n"
"<li style=\" font-size:10pt;\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">LSB steganography for PNG/BMP</li>\n"
"<li style=\" font-size:10pt;\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">File encryption with automatic cache</li></ul>\n"
"<p style=\" margin-top:10px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; color:#94a3b8;\">Hint: there are two hidden utilities below.</span></p></body></html>", nullptr));
#if QT_CONFIG(tooltip)
        btnEggA->setToolTip(QCoreApplication::translate("MainWindow", "Launch external utility A", nullptr));
#endif // QT_CONFIG(tooltip)
        btnEggA->setText(QCoreApplication::translate("MainWindow", "Sponsor A", nullptr));
#if QT_CONFIG(tooltip)
        btnEggB->setToolTip(QCoreApplication::translate("MainWindow", "Launch external utility B", nullptr));
#endif // QT_CONFIG(tooltip)
        btnEggB->setText(QCoreApplication::translate("MainWindow", "Sponsor B", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabAbout), QCoreApplication::translate("MainWindow", "About", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
