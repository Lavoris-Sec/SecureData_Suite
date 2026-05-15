#include "ui/MainWindow.h"
#include "ui_MainWindow.h"

#include "stego/lsb_steganography.h"

#include <QCoreApplication>
#include <QApplication>
#include <QDesktopServices>
#include <QDialog>
#include <QDir>
#include <QDateTime>
#include <QEvent>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QIcon>
#include <QInputDialog>
#include <QKeyEvent>
#include <QCheckBox>
#include <QClipboard>
#include <QLabel>
#include <QLayout>
#include <QMenu>
#include <QMessageBox>
#include <QMovie>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QProcess>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QRandomGenerator>
#include <QResizeEvent>
#include <QSettings>
#include <QShortcut>
#include <QStatusBar>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QTimer>
#include <QToolButton>
#include <QUrl>
#include <QVBoxLayout>
#include <QEasingCurve>
#include <algorithm>

namespace sds {

namespace {

QIcon createMainAppIcon() {
    QPixmap pixmap(64, 64);
    pixmap.fill(Qt::black);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPen pen(QColor("#39ff5a"));
    pen.setWidth(4);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);

    painter.drawRoundedRect(QRectF(16, 28, 32, 24), 5, 5);
    painter.drawArc(QRectF(18, 8, 28, 28), 20 * 16, 140 * 16);

    painter.setBrush(QColor("#39ff5a"));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(QPointF(32, 38), 3.2, 3.2);
    painter.drawRoundedRect(QRectF(30.2, 38, 3.6, 8.5), 1.5, 1.5);

    painter.setPen(QColor(0, 0, 0, 90));
    QFont glyphFont("Consolas", 6, QFont::Bold);
    painter.setFont(glyphFont);
    const QString glyphs = "01X$";
    for (int row = 0; row < 5; ++row) {
        for (int col = 0; col < 5; ++col) {
            painter.drawText(QRectF(17 + col * 6, 29 + row * 4.5, 6, 6),
                             Qt::AlignCenter,
                             QString(glyphs.at((row + col) % glyphs.size())));
        }
    }

    return QIcon(pixmap);
}

QIcon createSponsorGifIcon() {
    QPixmap pixmap(64, 64);
    pixmap.fill(QColor("#6fa8ff"));

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);
    painter.drawEllipse(QRectF(9, 9, 15, 9));
    painter.drawEllipse(QRectF(18, 7, 18, 11));
    painter.drawEllipse(QRectF(31, 10, 14, 8));
    painter.drawEllipse(QRectF(40, 8, 13, 8));

    QPainterPath hill;
    hill.moveTo(0, 46);
    hill.quadTo(14, 36, 28, 40);
    hill.quadTo(44, 44, 64, 28);
    hill.lineTo(64, 64);
    hill.lineTo(0, 64);
    hill.closeSubpath();
    painter.setBrush(QColor("#68b323"));
    painter.drawPath(hill);

    painter.setBrush(QColor("#d8cfbf"));
    painter.drawRoundedRect(QRectF(25, 22, 17, 16), 5, 5);

    QPolygonF leftEar;
    leftEar << QPointF(27, 24) << QPointF(30, 18) << QPointF(32, 24);
    painter.drawPolygon(leftEar);
    QPolygonF rightEar;
    rightEar << QPointF(35, 24) << QPointF(38, 18) << QPointF(40, 24);
    painter.drawPolygon(rightEar);

    painter.setBrush(QColor("#2d2d2d"));
    painter.drawEllipse(QRectF(29, 28, 2.5, 2.5));
    painter.drawEllipse(QRectF(35, 28, 2.5, 2.5));

    QPen facePen(QColor("#2d2d2d"));
    facePen.setWidth(1);
    painter.setPen(facePen);
    painter.drawArc(QRectF(31, 30, 4, 4), 200 * 16, 140 * 16);

    painter.setPen(QPen(QColor(0, 0, 0, 35), 2));
    painter.drawLine(QPointF(38, 37), QPointF(45, 42));

    return QIcon(pixmap);
}

} // namespace

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , cipherManager_()
    , fileCache_()
    , translationManager_(this) {
    ui->setupUi(this);
    setWindowIcon(createMainAppIcon());
    applyTheme();

    rebuildCipherList();

    connect(ui->btnEncrypt, &QPushButton::clicked, this, &MainWindow::onEncryptClicked);
    connect(ui->btnDecrypt, &QPushButton::clicked, this, &MainWindow::onDecryptClicked);
    connect(ui->btnSwap, &QPushButton::clicked, this, &MainWindow::onSwapClicked);
    connect(ui->btnCopyInput, &QPushButton::clicked, this, [this]() {
        const QString text = ui->textInput->toPlainText();
        QApplication::clipboard()->setText(text);
        logStatus(text.isEmpty()
                  ? l10n("Input field copied (empty)", "Поле ввода скопировано (пусто)")
                  : l10n("Input field copied", "Поле ввода скопировано"));
    });
    connect(ui->btnCopyOutput, &QPushButton::clicked, this, [this]() {
        const QString text = ui->textOutput->toPlainText();
        QApplication::clipboard()->setText(text);
        logStatus(text.isEmpty()
                  ? l10n("Output field copied (empty)", "Поле вывода скопировано (пусто)")
                  : l10n("Output field copied", "Поле вывода скопировано"));
    });
    connect(ui->comboCipher, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onCipherChanged);
    connect(ui->btnRsaGenerate, &QPushButton::clicked, this, &MainWindow::onRsaGenerateClicked);

    connect(ui->btnStegoBrowseInput, &QPushButton::clicked, this, &MainWindow::onStegoBrowseInput);
    connect(ui->btnStegoBrowseOutput, &QPushButton::clicked, this, &MainWindow::onStegoBrowseOutput);
    connect(ui->btnStegoEmbed, &QPushButton::clicked, this, &MainWindow::onStegoEmbed);
    connect(ui->btnStegoExtract, &QPushButton::clicked, this, &MainWindow::onStegoExtract);

    connect(ui->btnFileBrowseInput, &QPushButton::clicked, this, &MainWindow::onFileBrowseInput);
    connect(ui->btnFileBrowseOutput, &QPushButton::clicked, this, &MainWindow::onFileBrowseOutput);
    connect(ui->btnFileEncrypt, &QPushButton::clicked, this, &MainWindow::onFileEncrypt);
    connect(ui->btnFileDecrypt, &QPushButton::clicked, this, &MainWindow::onFileDecrypt);
    connect(ui->btnOpenCache, &QPushButton::clicked, this, &MainWindow::onOpenCache);

    connect(ui->btnEggA, &QPushButton::clicked, this, &MainWindow::onAboutSecretA);
    connect(ui->btnEggB, &QPushButton::clicked, this, &MainWindow::onAboutSecretB);
    connect(ui->textAbout, &QTextBrowser::anchorClicked, this, &MainWindow::onAboutLinkClicked);
    connect(ui->comboAchievements, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onAchievementSelected);
    connect(ui->btnResetAchievements, &QPushButton::clicked, this, &MainWindow::onResetAchievements);
    connect(ui->btnRefreshAchievements, &QPushButton::clicked, this, &MainWindow::onRefreshAchievements);
    connect(ui->switchSecurityMode, &QCheckBox::stateChanged, this, &MainWindow::onSecurityModeChanged);
    connect(ui->checkDefaultCacheEnabled, &QCheckBox::stateChanged, this, &MainWindow::onUiPreferenceChanged);
    connect(ui->checkConfirmResetAchievements, &QCheckBox::stateChanged, this, &MainWindow::onUiPreferenceChanged);
    connect(ui->checkCompactMode, &QCheckBox::stateChanged, this, &MainWindow::onUiPreferenceChanged);
    connect(ui->checkReducedAnimations, &QCheckBox::stateChanged, this, &MainWindow::onUiPreferenceChanged);

    ui->labelCachePath->setText(fileCache_.cacheDir());

    setupLanguageMenu();
    setupSecretButton();
    setupHotkeys();

    aboutShortcut_ = new QShortcut(QKeySequence(Qt::Key_F1), this);
    connect(aboutShortcut_, &QShortcut::activated, this, &MainWindow::onOpenAbout);

    QSettings startupSettings(QDir(QCoreApplication::applicationDirPath()).filePath("settings.ini"), QSettings::IniFormat);
    sponsorShuffleEnabled_ = startupSettings.value("Easter/SponsorShuffleEnabled", true).toBool();
    const bool cacheByDefault = startupSettings.value("General/CacheByDefault", true).toBool();
    confirmResetAchievementsEnabled_ = startupSettings.value("General/ConfirmResetAchievements", true).toBool();
    compactModeEnabled_ = startupSettings.value("General/CompactMode", false).toBool();
    reducedAnimationsEnabled_ = startupSettings.value("General/ReducedAnimations", false).toBool();
    ui->checkCache->setChecked(cacheByDefault);
    ui->checkDefaultCacheEnabled->setChecked(cacheByDefault);
    ui->checkConfirmResetAchievements->setChecked(confirmResetAchievementsEnabled_);
    ui->checkCompactMode->setChecked(compactModeEnabled_);
    ui->checkReducedAnimations->setChecked(reducedAnimationsEnabled_);
    const QString startupLocale = startupSettings.value("Language/Locale", "en").toString().trimmed();
    if (startupLocale == "ru_BY") {
        uiLocale_ = "ru_BY";
        translationManager_.loadLanguage("ru_BY");
        applyManualTranslations();
    } else {
        uiLocale_ = "en";
        translationManager_.clear();
        applyManualTranslations();
    }

    applySafetyMode();
    applyProfileMode();
    applySecurityModeUi();
    maybeShowOnboarding();
    loadAchievements();
    refreshAchievementsUi();
    logStatus(l10n("Ready", "Готово"));
    updateCipherUi();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::changeEvent(QEvent* event) {
    if (event->type() == QEvent::LanguageChange) {
        applyManualTranslations();
    }
    QMainWindow::changeEvent(event);
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    updateSecretButtonPosition();
    updateLanguageButtonPosition();
}

const CipherBase* MainWindow::currentCipher() const {
    const QVariant id = ui->comboCipher->currentData();
    return cipherManager_.findById(id.toString());
}

void MainWindow::rebuildCipherList() {
    const QString previousId = ui->comboCipher->currentData().toString();
    ui->comboCipher->blockSignals(true);
    ui->comboCipher->clear();

    for (const auto& cipher : cipherManager_.ciphers()) {
        const QString id = cipher->id();
        QString displayName = cipher->name();
        if (!strongModeEnabled_) {
            if (id == "simple_aes") {
                displayName = "AES (edu)";
            } else if (id == "rsa") {
                displayName = "RSA (edu)";
            } else if (id == "xor") {
                displayName = "XOR (edu)";
            }
        } else {
            if (id == "simple_aes") {
                displayName = "AES";
            }
            if (id == "rsa") {
                displayName = "RSA";
            }
            if (id == "xor") {
                displayName = "XOR";
            }
        }
        ui->comboCipher->addItem(displayName, id);
    }

    int idx = ui->comboCipher->findData(previousId);
    if (idx < 0) {
        idx = 0;
    }
    if (ui->comboCipher->count() > 0) {
        ui->comboCipher->setCurrentIndex(idx);
    }
    ui->comboCipher->blockSignals(false);
    updateCipherUi();
}

void MainWindow::updateCipherUi() {
    const CipherBase* cipher = currentCipher();
    if (!cipher) {
        return;
    }

    ui->lineKey->setEnabled(cipher->requiresKey() && cipher->id() != "rsa");
    ui->lineKey->setPlaceholderText(localizedCipherKeyHint(cipher));
    ui->groupRsa->setVisible(cipher->id() == "rsa");
}

QString MainWindow::localizedCipherKeyHint(const CipherBase* cipher) const {
    if (!cipher) {
        return QString();
    }

    const QString id = cipher->id();
    if (id == "caesar") {
        return l10n("Integer shift (e.g., 3)", "Целочисленный сдвиг (например, 3)");
    }
    if (id == "vigenere") {
        return l10n("Keyword (letters only)", "Ключевое слово (только буквы)");
    }
    if (id == "xor") {
        return l10n("Text key (bytes repeat)", "Текстовый ключ (байты повторяются)");
    }
    if (id == "permutation") {
        return l10n("Keyword (letters or digits)", "Ключевое слово (буквы или цифры)");
    }
    if (id == "simple_aes") {
        return l10n("Key (16+ chars, UTF-8)", "Ключ (16+ символов, UTF-8)");
    }
    if (id == "rsa") {
        return l10n("Public: CNGPUB:...  |  Private: CNGPRIV:...",
                    "Публичный: CNGPUB:...  |  Приватный: CNGPRIV:...");
    }

    return cipher->keyHint();
}

void MainWindow::showError(const QString& message) {
    logStatus(l10n("Error: ", "Ошибка: ") + message);
    checkAchievements("error_event");
    showToast(message, true, 3200);
}

void MainWindow::onCipherChanged(int index) {
    Q_UNUSED(index)
    updateCipherUi();
}

void MainWindow::onEncryptClicked() {
    const CipherBase* cipher = currentCipher();
    if (!cipher) {
        return;
    }

    QString error;
    const QString key = cipher->id() == "rsa" ? ui->lineRsaPublic->text().trimmed() : ui->lineKey->text();
    const QString input = ui->textInput->toPlainText();
    const QString output = cipher->encryptText(input, key, &error);

    if (!error.isEmpty()) {
        showError(error);
        return;
    }

    ui->textOutput->setPlainText(output);
    logStatus(l10n("Text encrypted", "Текст зашифрован"));
    checkAchievements("encrypt_text");
}

void MainWindow::onDecryptClicked() {
    const CipherBase* cipher = currentCipher();
    if (!cipher) {
        return;
    }

    QString error;
    const QString key = cipher->id() == "rsa" ? ui->lineRsaPrivate->text().trimmed() : ui->lineKey->text();
    const QString input = ui->textInput->toPlainText();
    const QString output = cipher->decryptText(input, key, &error);

    if (!error.isEmpty()) {
        showError(error);
        return;
    }

    ui->textOutput->setPlainText(output);
    logStatus(l10n("Text decrypted", "Текст расшифрован"));
    checkAchievements("decrypt_text");
}

void MainWindow::onSwapClicked() {
    const QString input = ui->textInput->toPlainText();
    const QString output = ui->textOutput->toPlainText();
    ui->textInput->setPlainText(output);
    ui->textOutput->setPlainText(input);
}

void MainWindow::onRsaGenerateClicked() {
    const RsaCipher rsa;
    QString error;
    const int bits = ui->spinRsaBits->value();
    if (bits > RsaCipher::maxSupportedBits()) {
        showError(l10n("Strong RSA backend is not available in this build. Install Boost/OpenSSL backend to use 1024+ bits.",
                       "Strong RSA backend недоступен в этой сборке. Установите Boost/OpenSSL backend для 1024+ бит."));
        return;
    }

    const int prevMin = ui->progressAchievement->minimum();
    const int prevMax = ui->progressAchievement->maximum();
    const int prevVal = ui->progressAchievement->value();
    const QString prevFmt = ui->progressAchievement->format();
    ui->progressAchievement->setRange(0, 0);
    ui->progressAchievement->setFormat(l10n("Generating RSA...", "Генерация RSA..."));
    qApp->processEvents();

    const RsaKeyPair pair = rsa.generateKeyPair(bits, &error);
    ui->progressAchievement->setRange(prevMin, prevMax);
    ui->progressAchievement->setValue(prevVal);
    ui->progressAchievement->setFormat(prevFmt);
    if (!error.isEmpty()) {
        showError(error);
        return;
    }
    ui->lineRsaPublic->setText(pair.publicKey);
    ui->lineRsaPrivate->setText(pair.privateKey);
    logStatus(l10n("RSA keys generated", "Ключи RSA сгенерированы"));
    checkAchievements("rsa_generate");
}

void MainWindow::onStegoBrowseInput() {
    const QString file = QFileDialog::getOpenFileName(this, tr("Select image"), QString(), tr("Images (*.png *.bmp)"));
    if (!file.isEmpty()) {
        ui->lineStegoInput->setText(file);
    }
}

void MainWindow::onStegoBrowseOutput() {
    const QString file = QFileDialog::getSaveFileName(this, tr("Save image"), QString(), tr("Images (*.png *.bmp)"));
    if (!file.isEmpty()) {
        ui->lineStegoOutput->setText(file);
    }
}

void MainWindow::onStegoEmbed() {
    const QString input = ui->lineStegoInput->text().trimmed();
    const QString output = ui->lineStegoOutput->text().trimmed();
    const QString message = ui->textStegoMessage->toPlainText();

    if (input.isEmpty() || output.isEmpty()) {
        showError(tr("Please choose both input and output images."));
        return;
    }

    QString error;
    if (!LsbSteganography::embedText(input, output, message, &error)) {
        showError(error);
        return;
    }

    showToast(tr("Message embedded into the image."));
    logStatus(l10n("Message embedded", "Сообщение встроено"));
    checkAchievements("stego_embed");
}

void MainWindow::onStegoExtract() {
    const QString input = ui->lineStegoInput->text().trimmed();
    if (input.isEmpty()) {
        showError(tr("Please choose an image to extract from."));
        return;
    }

    QString error;
    const QString message = LsbSteganography::extractText(input, &error);
    if (!error.isEmpty()) {
        showError(error);
        return;
    }

    ui->textStegoMessage->setPlainText(message);
    logStatus(l10n("Message extracted", "Сообщение извлечено"));
    checkAchievements("stego_extract");
}

void MainWindow::onFileBrowseInput() {
    const QString file = QFileDialog::getOpenFileName(this, tr("Select file"), QString(), tr("All files (*.*)"));
    if (!file.isEmpty()) {
        ui->lineFileInput->setText(file);
    }
}

void MainWindow::onFileBrowseOutput() {
    const QString file = QFileDialog::getSaveFileName(this, tr("Save file"), QString(), tr("All files (*.*)"));
    if (!file.isEmpty()) {
        ui->lineFileOutput->setText(file);
    }
}

void MainWindow::onFileEncrypt() {
    const CipherBase* cipher = currentCipher();
    if (!cipher) {
        return;
    }
    if (!cipher->supportsBinary()) {
        showError(tr("This algorithm does not support files. Use XOR or Simple AES."));
        return;
    }

    const QString inputPath = ui->lineFileInput->text().trimmed();
    const QString outputPath = ui->lineFileOutput->text().trimmed();
    if (inputPath.isEmpty() || outputPath.isEmpty()) {
        showError(tr("Please choose both input and output files."));
        return;
    }

    QFile inputFile(inputPath);
    if (!inputFile.open(QIODevice::ReadOnly)) {
        showError(tr("Failed to open input file."));
        return;
    }
    const QByteArray data = inputFile.readAll();
    inputFile.close();

    QString error;
    const QByteArray encrypted = cipher->encryptBytes(data, ui->lineKey->text(), &error);
    if (!error.isEmpty()) {
        showError(error);
        return;
    }

    QFile outputFile(outputPath);
    if (!outputFile.open(QIODevice::WriteOnly)) {
        showError(tr("Failed to write output file."));
        return;
    }
    outputFile.write(encrypted);
    outputFile.close();

    if (ui->checkCache->isChecked()) {
        fileCache_.saveEncryptedCopy(inputPath, encrypted, cipher->id(), nullptr, nullptr);
    }

    showToast(tr("File encrypted."));
    logStatus(l10n("File encrypted", "Файл зашифрован"));
    checkAchievements("file_encrypt");
}

void MainWindow::onFileDecrypt() {
    const CipherBase* cipher = currentCipher();
    if (!cipher) {
        return;
    }
    if (!cipher->supportsBinary()) {
        showError(tr("This algorithm does not support files. Use XOR or Simple AES."));
        return;
    }

    const QString inputPath = ui->lineFileInput->text().trimmed();
    const QString outputPath = ui->lineFileOutput->text().trimmed();
    if (inputPath.isEmpty() || outputPath.isEmpty()) {
        showError(tr("Please choose both input and output files."));
        return;
    }

    QFile inputFile(inputPath);
    if (!inputFile.open(QIODevice::ReadOnly)) {
        showError(tr("Failed to open input file."));
        return;
    }
    const QByteArray data = inputFile.readAll();
    inputFile.close();

    QString error;
    const QByteArray decrypted = cipher->decryptBytes(data, ui->lineKey->text(), &error);
    if (!error.isEmpty()) {
        showError(error);
        return;
    }

    QFile outputFile(outputPath);
    if (!outputFile.open(QIODevice::WriteOnly)) {
        showError(tr("Failed to write output file."));
        return;
    }
    outputFile.write(decrypted);
    outputFile.close();

    showToast(tr("File decrypted."));
    logStatus(l10n("File decrypted", "Файл расшифрован"));
    checkAchievements("file_decrypt");
}

void MainWindow::onOpenCache() {
    QDesktopServices::openUrl(QUrl::fromLocalFile(fileCache_.cacheDir()));
    logStatus(l10n("Opened cache folder", "Открыта папка кеша"));
}

void MainWindow::setupLanguageMenu() {
    QMenu* langMenu = new QMenu(this);
    QAction* actionEn = langMenu->addAction(tr("English"));
    QAction* actionRu = langMenu->addAction(tr("Russian"));
    connect(actionEn, &QAction::triggered, this, &MainWindow::onLanguageEnglish);
    connect(actionRu, &QAction::triggered, this, &MainWindow::onLanguageRussian);

    languageButton_ = new QToolButton(this);
    languageButton_->setObjectName("languageButton");
    languageButton_->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    languageButton_->setText(tr("Language"));
    languageButton_->setPopupMode(QToolButton::InstantPopup);
    languageButton_->setMenu(langMenu);
    languageButton_->setFixedHeight(28);
    languageButton_->setMinimumWidth(150);
    languageButton_->setCursor(Qt::PointingHandCursor);
    languageButton_->raise();
    ui->textInput->setPlaceholderText(l10n("Enter source text here", "Введите исходный текст"));
    ui->textOutput->setPlaceholderText(l10n("Result will appear here", "Здесь появится результат"));
    ui->lineKey->setPlaceholderText(localizedCipherKeyHint(currentCipher()));
    ui->lineRsaPublic->setPlaceholderText(l10n("Public key will appear here", "Здесь появится публичный ключ"));
    ui->lineRsaPrivate->setPlaceholderText(l10n("Private key will appear here", "Здесь появится приватный ключ"));
    ui->lineStegoInput->setPlaceholderText(l10n("Choose input image", "Выберите входное изображение"));
    ui->lineStegoOutput->setPlaceholderText(l10n("Choose output image path", "Выберите путь для выходного изображения"));
    ui->textStegoMessage->setPlaceholderText(l10n("Enter hidden message", "Введите скрытое сообщение"));

    updateLanguageButtonVisuals();
    updateLanguageButtonPosition();
}

void MainWindow::onLanguageEnglish() {
    uiLocale_ = "en";
    translationManager_.clear();
    applyManualTranslations();
    QSettings settings(QDir(QCoreApplication::applicationDirPath()).filePath("settings.ini"), QSettings::IniFormat);
    settings.setValue("Language/Locale", "en");
    showToast(l10n("English language applied.", "Английский язык применён."));
}

void MainWindow::onLanguageRussian() {
    uiLocale_ = "ru_BY";
    if (!translationManager_.loadLanguage("ru_BY")) {
        // Continue with manual translation fallback even if qm is missing.
    }
    applyManualTranslations();
    QSettings settings(QDir(QCoreApplication::applicationDirPath()).filePath("settings.ini"), QSettings::IniFormat);
    settings.setValue("Language/Locale", "ru_BY");
    showToast(l10n("Russian language applied.", "Русский язык применён."));
}

void MainWindow::setupSecretButton() {
    secretButton_ = new QPushButton(this);
    secretButton_->setObjectName("secretButton");
    secretButton_->setText("");
    secretButton_->setFlat(true);
    secretButton_->setFixedSize(18, 18);
    secretButton_->setStyleSheet("background: transparent; border: none;");
    secretButton_->raise();
    connect(secretButton_, &QPushButton::clicked, this, &MainWindow::onAboutSecretC);

    auto* pulseTimer = new QTimer(this);
    pulseTimer->setInterval(7000);
    connect(pulseTimer, &QTimer::timeout, this, [this, pulseTimer]() {
        if (!secretButton_) {
            return;
        }
        const int nextMs = QRandomGenerator::global()->bounded(4500, 9500);
        pulseTimer->setInterval(nextMs);

        if (QRandomGenerator::global()->bounded(100) < 30) {
            secretButton_->setStyleSheet(
                "background: rgba(96, 165, 250, 0.18);"
                "border: 1px solid rgba(147, 197, 253, 0.55);"
                "border-radius: 9px;");
            QTimer::singleShot(850, this, [this]() {
                if (secretButton_) {
                    secretButton_->setStyleSheet("background: transparent; border: none;");
                }
            });
        }
    });
    pulseTimer->start();
    updateSecretButtonPosition();
}

void MainWindow::updateSecretButtonPosition() {
    if (!secretButton_) {
        return;
    }
    const int x = width() - secretButton_->width() - 6;
    const int y = height() - secretButton_->height() - 6;
    secretButton_->move(x, y);
}

void MainWindow::updateLanguageButtonPosition() {
    if (!languageButton_) {
        return;
    }
    const int x = width() - languageButton_->width() - 14;
    const int y = menuBar()->isVisible() ? menuBar()->height() + 10 : 10;
    languageButton_->move(x, y);

    if (languageFlagLabel_) {
        const int flagX = x - languageFlagLabel_->width() - 8;
        const int flagY = y + (languageButton_->height() - languageFlagLabel_->height()) / 2;
        languageFlagLabel_->move(flagX, flagY);
    }
}

void MainWindow::updateLanguageButtonVisuals() {
    if (!languageButton_) {
        return;
    }

    const bool isRussian = (uiLocale_ == "ru_BY");
    languageButton_->setText(isRussian ? l10n("Russian", "Русский") : l10n("English", "Английский"));
    languageButton_->setIcon(QIcon(isRussian ? ":/flags/ru.xpm" : ":/flags/us.xpm"));
    languageButton_->setIconSize(QSize(20, 14));

    if (!languageFlagLabel_) {
        languageFlagLabel_ = new QLabel(this);
        languageFlagLabel_->setObjectName("languageFlagLabel");
        languageFlagLabel_->setFixedSize(24, 16);
        languageFlagLabel_->setStyleSheet("background: transparent;");
        languageFlagLabel_->raise();
    }

    QPixmap pix(isRussian ? ":/flags/ru.xpm" : ":/flags/us.xpm");
    if (!pix.isNull()) {
        languageFlagLabel_->setPixmap(pix.scaled(languageFlagLabel_->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        languageFlagLabel_->setText("");
    } else {
        languageFlagLabel_->setPixmap(QPixmap());
        languageFlagLabel_->setText(isRussian ? "RU" : "EN");
        languageFlagLabel_->setStyleSheet("color:#E2E8F0; background:transparent; font-weight:700;");
    }
    languageFlagLabel_->show();
    updateLanguageButtonPosition();
}

QString MainWindow::l10n(const QString& en, const QString& ru) const {
    return uiLocale_ == "ru_BY" ? ru : en;
}

void MainWindow::applyManualTranslations() {
    setWindowTitle("SecureData Suite");

    ui->tabWidget->setTabText(ui->tabWidget->indexOf(ui->tabCrypto), l10n("Cryptography", "Криптография"));
    ui->tabWidget->setTabText(ui->tabWidget->indexOf(ui->tabStego), l10n("Steganography", "Стеганография"));
    ui->tabWidget->setTabText(ui->tabWidget->indexOf(ui->tabFiles), l10n("Files", "Файлы"));
    ui->tabWidget->setTabText(ui->tabWidget->indexOf(ui->tabSettings), l10n("Settings (F5)", "Настройки (F5)"));
    ui->tabWidget->setTabText(ui->tabWidget->indexOf(ui->tabAbout), l10n("About (F1)", "О программе (F1)"));

    ui->labelAlgorithm->setText(l10n("Algorithm", "Алгоритм"));
    ui->labelKey->setText(l10n("Key", "Ключ"));
    ui->groupRsa->setTitle("RSA keys");
    ui->labelRsaPublic->setText(l10n("Public (e,n)", "Публичный (e,n)"));
    ui->labelRsaPrivate->setText(l10n("Private (d,n)", "Приватный (d,n)"));
    ui->labelRsaBits->setText(l10n("Bits", "Биты"));
    ui->btnRsaGenerate->setText(l10n("Generate", "Сгенерировать"));
    ui->labelInput->setText(l10n("Input text", "Входной текст"));
    ui->labelOutput->setText(l10n("Result", "Результат"));
    ui->btnEncrypt->setText(l10n("Encrypt", "Зашифровать"));
    ui->btnDecrypt->setText(l10n("Decrypt", "Расшифровать"));
    ui->btnSwap->setText(l10n("Swap input/output", "Поменять местами"));
    ui->btnCopyInput->setText(l10n("Copy", "Копировать"));
    ui->btnCopyOutput->setText(l10n("Copy", "Копировать"));
    ui->btnCopyInput->setToolTip(l10n("Copy input text to clipboard", "Скопировать текст из поля ввода"));
    ui->btnCopyOutput->setToolTip(l10n("Copy result text to clipboard", "Скопировать текст из поля результата"));

    ui->labelStegoInput->setText(l10n("Input image", "Входное изображение"));
    ui->labelStegoOutput->setText(l10n("Output image", "Выходное изображение"));
    ui->labelStegoMessage->setText(l10n("Message", "Сообщение"));
    ui->btnStegoBrowseInput->setText(l10n("Browse", "Обзор"));
    ui->btnStegoBrowseOutput->setText(l10n("Save as", "Сохранить как"));
    ui->btnStegoEmbed->setText(l10n("Embed", "Встроить"));
    ui->btnStegoExtract->setText(l10n("Extract", "Извлечь"));

    ui->labelFileInput->setText(l10n("Input file", "Входной файл"));
    ui->labelFileOutput->setText(l10n("Output file", "Выходной файл"));
    ui->btnFileBrowseInput->setText(l10n("Browse", "Обзор"));
    ui->btnFileBrowseOutput->setText(l10n("Save as", "Сохранить как"));
    ui->checkCache->setText(l10n("Save encrypted copy to cache", "Сохранять зашифрованную копию в кеш"));
    ui->btnOpenCache->setText(l10n("Open cache", "Открыть кеш"));
    ui->btnFileEncrypt->setText(l10n("Encrypt file", "Зашифровать файл"));
    ui->btnFileDecrypt->setText(l10n("Decrypt file", "Расшифровать файл"));
    ui->lineFileInput->setPlaceholderText(l10n("Choose source file to process", "Выберите исходный файл для обработки"));
    ui->lineFileOutput->setPlaceholderText(l10n("Choose path for processed result", "Выберите путь для сохранения результата"));
    ui->lineFileInput->setToolTip(l10n("Source file: this file will be read.",
                                       "Исходный файл: этот файл будет считан."));
    ui->lineFileOutput->setToolTip(l10n("Destination file: encrypted/decrypted result will be saved here.",
                                        "Файл назначения: сюда будет сохранён результат шифрования/расшифрования."));
    ui->btnFileEncrypt->setToolTip(l10n("Encrypt input file into output file (XOR/Simple AES).",
                                        "Зашифровать входной файл в выходной (XOR/Simple AES)."));
    ui->btnFileDecrypt->setToolTip(l10n("Decrypt input file into output file (XOR/Simple AES).",
                                        "Расшифровать входной файл в выходной (XOR/Simple AES)."));
    ui->checkCache->setToolTip(l10n("Also store encrypted copy in app cache folder.",
                                    "Дополнительно сохранить зашифрованную копию в папке кеша."));
    ui->btnOpenCache->setToolTip(l10n("Open folder with cached encrypted copies.",
                                      "Открыть папку с кешированными зашифрованными копиями."));

    ui->groupFilePaths->setTitle(l10n("File paths", "Пути к файлам"));
    ui->groupFileCache->setTitle(l10n("Cache", "Кеш"));
    ui->groupAchievements->setTitle(l10n("Achievements", "Достижения"));
    ui->groupSecurityMode->setTitle(l10n("Security mode", "Режим безопасности"));
    ui->groupPreferences->setTitle(l10n("Preferences", "Предпочтения"));
    ui->labelSettingsHint->setText(l10n("Achievements", "Достижения"));
    ui->labelModeEducation->setText(l10n("Education mode", "Учебный режим"));
    ui->labelModeStrong->setText(l10n("Strong mode", "Усиленный режим"));
    ui->btnRefreshAchievements->setText(l10n("Refresh", "Обновить"));
    ui->btnResetAchievements->setText(l10n("Clear achievements", "Очистить достижения"));
    ui->checkDefaultCacheEnabled->setText(l10n("Enable file cache by default", "Включать кеш файлов по умолчанию"));
    ui->checkConfirmResetAchievements->setText(l10n("Ask confirmation before clearing achievements",
                                                    "Запрашивать подтверждение перед очисткой достижений"));
    ui->checkCompactMode->setText(l10n("Compact mode", "Компактный режим"));
    ui->checkReducedAnimations->setText(l10n("Reduced animations", "Уменьшенные анимации"));

    ui->btnEggA->setText(l10n("Sponsor A", "Спонсор A"));
    ui->btnEggB->setText(l10n("Sponsor B", "Спонсор B"));
    ui->btnEggA->setToolTip(l10n("Launch sponsor action A (Ctrl+Q toggles random link mode)", "Запустить действие спонсора A (Ctrl+Q переключает режим случайных ссылок)"));
    ui->btnEggB->setToolTip(l10n("Launch external utility B", "Запустить внешнюю утилиту B"));

    if (languageButton_ && languageButton_->menu() && languageButton_->menu()->actions().size() >= 2) {
        languageButton_->menu()->actions().at(0)->setText(l10n("English", "Английский"));
        languageButton_->menu()->actions().at(1)->setText(l10n("Russian", "Русский"));
    }

    updateLanguageButtonVisuals();
    ui->textInput->setPlaceholderText(l10n("Enter source text here", "Введите исходный текст"));
    ui->textOutput->setPlaceholderText(l10n("Result will appear here", "Здесь появится результат"));
    ui->lineKey->setPlaceholderText(localizedCipherKeyHint(currentCipher()));
    ui->lineRsaPublic->setPlaceholderText(l10n("Public key will appear here", "Здесь появится публичный ключ"));
    ui->lineRsaPrivate->setPlaceholderText(l10n("Private key will appear here", "Здесь появится приватный ключ"));
    ui->lineStegoInput->setPlaceholderText(l10n("Choose input image", "Выберите входное изображение"));
    ui->lineStegoOutput->setPlaceholderText(l10n("Choose output image path", "Выберите путь для выходного изображения"));
    ui->textStegoMessage->setPlaceholderText(l10n("Enter hidden message", "Введите скрытое сообщение"));
    applySponsorButtonModes();
    refreshAboutText();
    refreshAchievementsUi();
    applySecurityModeUi();
}

void MainWindow::applyButtonLinkStyle(QPushButton* button, bool asLink) {
    if (!button) {
        return;
    }
    button->setCursor(Qt::PointingHandCursor);
    if (asLink) {
        button->setStyleSheet(
            "QPushButton {"
            " color: #60A5FA;"
            " background: transparent;"
            " border: none;"
            " text-decoration: underline;"
            " font-weight: 700;"
            " padding: 7px 12px;"
            "}"
            "QPushButton:hover { color: #93C5FD; }"
            "QPushButton:pressed { color: #BFDBFE; }"
        );
    } else {
        button->setStyleSheet("");
    }
}

void MainWindow::applySponsorButtonModes() {
    sponsorALinkMode_ = false;
    sponsorBLinkMode_ = false;
    ui->btnEggA->setText(l10n("Sponsor A", "Спонсор A"));
    ui->btnEggB->setText(l10n("Sponsor B", "Спонсор B"));
    applyButtonLinkStyle(ui->btnEggA, false);
    applyButtonLinkStyle(ui->btnEggB, false);
}

void MainWindow::logStatus(const QString& message) {
    const QString stamp = QDateTime::currentDateTime().toString("HH:mm:ss");
    statusBar()->showMessage(QString("[%1] %2").arg(stamp, message), 5000);
}

void MainWindow::showToast(const QString& message, bool isError, int durationMs) {
    logStatus(message);
    if (!toastLabel_) {
        toastLabel_ = new QLabel(this);
        toastLabel_->setObjectName("toastLabel");
        toastLabel_->setWordWrap(true);
        toastLabel_->setAlignment(Qt::AlignCenter);
        toastLabel_->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        toastLabel_->hide();
    }

    const QString bg = isError ? "#7F1D1D" : "#0F766E";
    const QString border = isError ? "#F87171" : "#5EEAD4";
    toastLabel_->setStyleSheet(QString(
        "QLabel#toastLabel {"
        " color: #F8FAFC;"
        " background: %1;"
        " border: 1px solid %2;"
        " border-radius: 10px;"
        " padding: 10px 14px;"
        " font-weight: 600;"
        "}").arg(bg, border));
    toastLabel_->setText(message);
    toastLabel_->adjustSize();

    const int maxWidth = qMax(320, width() - 40);
    if (toastLabel_->width() > maxWidth) {
        toastLabel_->setFixedWidth(maxWidth);
        toastLabel_->adjustSize();
    } else {
        toastLabel_->setFixedWidth(toastLabel_->width());
    }

    const int x = (width() - toastLabel_->width()) / 2;
    const int yEnd = height() - statusBar()->height() - toastLabel_->height() - 16;
    const int yStart = yEnd + 20;

    toastLabel_->move(x, yStart);
    toastLabel_->setWindowOpacity(0.0);
    toastLabel_->show();
    toastLabel_->raise();

    if (reducedAnimationsEnabled_) {
        toastLabel_->move(x, yEnd);
        toastLabel_->setWindowOpacity(1.0);
        QTimer::singleShot(qMax(900, durationMs), this, [this]() {
            if (toastLabel_) {
                toastLabel_->hide();
            }
        });
        return;
    }

    auto* in = new QPropertyAnimation(toastLabel_, "windowOpacity", toastLabel_);
    in->setDuration(180);
    in->setStartValue(0.0);
    in->setEndValue(1.0);
    in->setEasingCurve(QEasingCurve::OutCubic);
    in->start(QAbstractAnimation::DeleteWhenStopped);

    auto* slide = new QPropertyAnimation(toastLabel_, "pos", toastLabel_);
    slide->setDuration(180);
    slide->setStartValue(QPoint(x, yStart));
    slide->setEndValue(QPoint(x, yEnd));
    slide->setEasingCurve(QEasingCurve::OutCubic);
    slide->start(QAbstractAnimation::DeleteWhenStopped);

    QTimer::singleShot(durationMs, this, [this]() {
        if (!toastLabel_) {
            return;
        }
        auto* out = new QPropertyAnimation(toastLabel_, "windowOpacity", toastLabel_);
        out->setDuration(220);
        out->setStartValue(1.0);
        out->setEndValue(0.0);
        out->setEasingCurve(QEasingCurve::InCubic);
        connect(out, &QPropertyAnimation::finished, toastLabel_, &QLabel::hide);
        out->start(QAbstractAnimation::DeleteWhenStopped);
    });
}

QString MainWindow::achievementFilePath() const {
    const QString base = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(base);
    dir.mkpath(".");
    const QString hiddenDir = dir.filePath(".sds");
    QDir(hiddenDir).mkpath(".");
    return QDir(hiddenDir).filePath("achievements.json");
}

void MainWindow::loadAchievements() {
    QFile f(achievementFilePath());
    if (!f.exists()) {
        achievementsState_ = QJsonObject{
            {"stats", QJsonObject{}},
            {"unlocked", QJsonObject{}}
        };
        saveAchievements();
        return;
    }
    if (!f.open(QIODevice::ReadOnly)) {
        achievementsState_ = QJsonObject{{"stats", QJsonObject{}}, {"unlocked", QJsonObject{}}};
        return;
    }
    const QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    f.close();
    achievementsState_ = doc.isObject() ? doc.object() : QJsonObject{{"stats", QJsonObject{}}, {"unlocked", QJsonObject{}}};
}

void MainWindow::saveAchievements() const {
    QFile f(achievementFilePath());
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return;
    }
    f.write(QJsonDocument(achievementsState_).toJson(QJsonDocument::Indented));
    f.close();
}

void MainWindow::incrementAchievementCounter(const QString& key) {
    QJsonObject stats = achievementsState_.value("stats").toObject();
    const int old = stats.value(key).toInt(0);
    stats.insert(key, old + 1);
    achievementsState_.insert("stats", stats);
}

int MainWindow::achievementCounter(const QString& key) const {
    return achievementsState_.value("stats").toObject().value(key).toInt(0);
}

bool MainWindow::isAchievementUnlocked(const QString& key) const {
    return achievementsState_.value("unlocked").toObject().value(key).toBool(false);
}

void MainWindow::unlockAchievement(const QString& key, const QString& titleEn, const QString& titleRu) {
    if (isAchievementUnlocked(key)) {
        return;
    }
    QJsonObject unlocked = achievementsState_.value("unlocked").toObject();
    unlocked.insert(key, true);
    achievementsState_.insert("unlocked", unlocked);
    saveAchievements();
    const QString title = l10n("Achievement unlocked", "Достижение открыто");
    const QString text = l10n(titleEn, titleRu);
    logStatus(title + ": " + text);
    showToast(title + ": " + text);
}

void MainWindow::checkAchievements(const QString& eventKey) {
    incrementAchievementCounter(eventKey);

    if (eventKey == "encrypt_text" && achievementCounter("encrypt_text") >= 1) {
        unlockAchievement("first_encrypt", "First encryption completed", "Первое шифрование выполнено");
    }
    if (eventKey == "file_encrypt" && achievementCounter("file_encrypt") >= 3) {
        unlockAchievement("file_guardian", "File Guardian: encrypted 3 files", "Хранитель файлов: зашифровано 3 файла");
    }
    if (achievementCounter("stego_embed") + achievementCounter("stego_extract") >= 3) {
        unlockAchievement("stego_explorer", "Stego Explorer: completed 3 stego operations",
                          "Исследователь стего: выполнено 3 стего-операции");
    }
    if (achievementCounter("error_event") >= 20) {
        unlockAchievement("touch_grass", "Touch Grass: 20 errors survived", "Touch Grass: пережито 20 ошибок");
    }
    if (eventKey == "konami") {
        unlockAchievement("matrix", "Matrix unlocked", "Матрица разблокирована");
    }
    if (eventKey == "panic_mode") {
        unlockAchievement("panic", "Panic button certified", "Паник-кнопка сертифицирована");
    }
    if (achievementCounter("sponsor_shuffle") >= 5) {
        unlockAchievement("sponsor_rng", "Sponsor RNG addict", "Адепт рандома спонсоров");
    }
    if (eventKey == "mini_game_win") {
        unlockAchievement("cipher_sense", "Cipher sense awakened", "Крипто-чутье пробудилось");
    }

    saveAchievements();
    refreshAchievementsUi();
}

void MainWindow::refreshAchievementsUi() {
    struct ADef {
        QString id;
        QString titleEn;
        QString titleRu;
        QString descEn;
        QString descRu;
        QString statKey;
        int target;
    };
    const QList<ADef> defs = {
        {"first_encrypt", "First Encrypt", "Первое шифрование", "Complete first text encryption", "Выполнить первое шифрование текста", "encrypt_text", 1},
        {"file_guardian", "File Guardian", "Хранитель файлов", "Encrypt 3 files", "Зашифровать 3 файла", "file_encrypt", 3},
        {"stego_explorer", "Stego Explorer", "Исследователь стего", "Complete 3 stego operations", "Выполнить 3 стего-операции", "stego_total", 3},
        {"touch_grass", "Touch Grass", "Touch Grass", "Make 20 errors (yes, really)", "Сделать 20 ошибок (да, правда)", "error_event", 20},
        {"matrix", "Matrix", "Матрица", "Enter Konami code", "Ввести код Конами", "konami", 1},
        {"panic", "Panic Certified", "Паник сертифицирован", "Use panic shortcut once", "Использовать паник-шорткат 1 раз", "panic_mode", 1},
        {"sponsor_rng", "Sponsor RNG", "Рандом спонсоров", "Use sponsor shuffle 5 times", "Использовать рандом спонсоров 5 раз", "sponsor_shuffle", 5},
        {"cipher_sense", "Cipher Sense", "Крипто-чутье", "Win mini game once", "Победить в мини-игре 1 раз", "mini_game_win", 1}
    };

    const int prev = ui->comboAchievements->currentIndex();
    ui->comboAchievements->blockSignals(true);
    ui->comboAchievements->clear();

    int unlockedCount = 0;
    for (const auto& d : defs) {
        const bool unlocked = isAchievementUnlocked(d.id);
        if (unlocked) {
            unlockedCount++;
        }
        ui->comboAchievements->addItem(unlocked ? l10n(d.titleEn, d.titleRu) : l10n("Hidden achievement", "Скрытое достижение"), d.id);
    }
    ui->comboAchievements->blockSignals(false);
    ui->progressAchievement->setRange(0, defs.size());
    ui->progressAchievement->setValue(unlockedCount);
    ui->progressAchievement->setFormat(l10n("Unlocked %v/%m", "Открыто %v/%m"));

    if (ui->comboAchievements->count() > 0) {
        ui->comboAchievements->setCurrentIndex(qBound(0, prev, ui->comboAchievements->count() - 1));
        onAchievementSelected(ui->comboAchievements->currentIndex());
    } else {
        ui->labelAchievementStatus->setText(l10n("No achievements", "Нет достижений"));
    }
}

void MainWindow::onAchievementSelected(int index) {
    if (index < 0) {
        return;
    }
    const QString id = ui->comboAchievements->itemData(index).toString();
    struct ADef { QString id; QString titleEn; QString titleRu; QString descEn; QString descRu; QString statKey; int target; };
    const QList<ADef> defs = {
        {"first_encrypt", "First Encrypt", "Первое шифрование", "Complete first text encryption", "Выполнить первое шифрование текста", "encrypt_text", 1},
        {"file_guardian", "File Guardian", "Хранитель файлов", "Encrypt 3 files", "Зашифровать 3 файла", "file_encrypt", 3},
        {"stego_explorer", "Stego Explorer", "Исследователь стего", "Complete 3 stego operations", "Выполнить 3 стего-операции", "stego_total", 3},
        {"touch_grass", "Touch Grass", "Touch Grass", "Make 20 errors (yes, really)", "Сделать 20 ошибок (да, правда)", "error_event", 20},
        {"matrix", "Matrix", "Матрица", "Enter Konami code", "Ввести код Конами", "konami", 1},
        {"panic", "Panic Certified", "Паник сертифицирован", "Use panic shortcut once", "Использовать паник-шорткат 1 раз", "panic_mode", 1},
        {"sponsor_rng", "Sponsor RNG", "Рандом спонсоров", "Use sponsor shuffle 5 times", "Использовать рандом спонсоров 5 раз", "sponsor_shuffle", 5},
        {"cipher_sense", "Cipher Sense", "Крипто-чутье", "Win mini game once", "Победить в мини-игре 1 раз", "mini_game_win", 1}
    };
    for (const auto& d : defs) {
        if (d.id != id) continue;
        const bool unlocked = isAchievementUnlocked(d.id);
        int progress = 0;
        if (d.statKey == "stego_total") {
            progress = achievementCounter("stego_embed") + achievementCounter("stego_extract");
        } else {
            progress = achievementCounter(d.statKey);
        }
        progress = qBound(0, progress, d.target);
        ui->progressAchievement->setRange(0, d.target);
        ui->progressAchievement->setValue(progress);
        ui->progressAchievement->setFormat(unlocked ? l10n("Completed", "Выполнено")
                                                   : l10n("Progress %v/%m", "Прогресс %v/%m"));
        if (unlocked) {
            ui->labelAchievementStatus->setText(
                QString("<b>%1</b><br/>%2")
                    .arg(l10n(d.titleEn, d.titleRu),
                         l10n(d.descEn, d.descRu)));
        } else {
            ui->labelAchievementStatus->setText(
                QString("<b>%1</b><br/>%2")
                    .arg(l10n("Locked achievement", "Скрытое достижение"),
                         l10n("Description hidden until unlocked.",
                              "Описание откроется после получения.")));
        }
        return;
    }
}

void MainWindow::onResetAchievements() {
    if (confirmResetAchievementsEnabled_) {
        if (QMessageBox::question(this, l10n("Reset achievements", "Сброс достижений"),
                                  l10n("Clear all achievements and progress?", "Очистить все достижения и прогресс?"))
            != QMessageBox::Yes) {
            return;
        }
    }
    achievementsState_ = QJsonObject{{"stats", QJsonObject{}}, {"unlocked", QJsonObject{}}};
    saveAchievements();
    refreshAchievementsUi();
    logStatus(l10n("Achievements cleared", "Достижения очищены"));
}

void MainWindow::onRefreshAchievements() {
    loadAchievements();
    refreshAchievementsUi();
    logStatus(l10n("Achievements reloaded", "Достижения обновлены"));
}

void MainWindow::onUiPreferenceChanged(int value) {
    Q_UNUSED(value)
    const bool cacheByDefault = ui->checkDefaultCacheEnabled->isChecked();
    confirmResetAchievementsEnabled_ = ui->checkConfirmResetAchievements->isChecked();
    compactModeEnabled_ = ui->checkCompactMode->isChecked();
    reducedAnimationsEnabled_ = ui->checkReducedAnimations->isChecked();
    ui->checkCache->setChecked(cacheByDefault);

    QSettings settings(QDir(QCoreApplication::applicationDirPath()).filePath("settings.ini"), QSettings::IniFormat);
    settings.setValue("General/CacheByDefault", cacheByDefault);
    settings.setValue("General/ConfirmResetAchievements", confirmResetAchievementsEnabled_);
    settings.setValue("General/CompactMode", compactModeEnabled_);
    settings.setValue("General/ReducedAnimations", reducedAnimationsEnabled_);

    applyTheme();
}

void MainWindow::setupHotkeys() {
    auto* shEncrypt = new QShortcut(QKeySequence("Ctrl+E"), this);
    connect(shEncrypt, &QShortcut::activated, this, &MainWindow::onEncryptClicked);

    auto* shDecrypt = new QShortcut(QKeySequence("Ctrl+D"), this);
    connect(shDecrypt, &QShortcut::activated, this, &MainWindow::onDecryptClicked);

    auto* shSwap = new QShortcut(QKeySequence("Ctrl+W"), this);
    connect(shSwap, &QShortcut::activated, this, &MainWindow::onSwapClicked);

    auto* shLang = new QShortcut(QKeySequence("Ctrl+L"), this);
    connect(shLang, &QShortcut::activated, [this]() {
        if (languageButton_) {
            languageButton_->showMenu();
        }
    });

    auto* shTab1 = new QShortcut(QKeySequence("Ctrl+1"), this);
    connect(shTab1, &QShortcut::activated, [this]() { ui->tabWidget->setCurrentWidget(ui->tabCrypto); });

    auto* shTab2 = new QShortcut(QKeySequence("Ctrl+2"), this);
    connect(shTab2, &QShortcut::activated, [this]() { ui->tabWidget->setCurrentWidget(ui->tabStego); });

    auto* shTab3 = new QShortcut(QKeySequence("Ctrl+3"), this);
    connect(shTab3, &QShortcut::activated, [this]() { ui->tabWidget->setCurrentWidget(ui->tabFiles); });

    auto* shTab4 = new QShortcut(QKeySequence("Ctrl+4"), this);
    connect(shTab4, &QShortcut::activated, [this]() { ui->tabWidget->setCurrentWidget(ui->tabSettings); });

    auto* shTab5 = new QShortcut(QKeySequence("Ctrl+5"), this);
    connect(shTab5, &QShortcut::activated, [this]() { ui->tabWidget->setCurrentWidget(ui->tabAbout); });

    auto* shSettingsF5 = new QShortcut(QKeySequence(Qt::Key_F5), this);
    connect(shSettingsF5, &QShortcut::activated, [this]() { ui->tabWidget->setCurrentWidget(ui->tabSettings); });

    auto* shToggleShuffle = new QShortcut(QKeySequence("Ctrl+Q"), this);
    connect(shToggleShuffle, &QShortcut::activated, this, &MainWindow::onToggleSponsorShuffle);

    auto* shPanic = new QShortcut(QKeySequence("Ctrl+Shift+X"), this);
    connect(shPanic, &QShortcut::activated, [this]() {
        ui->textInput->clear();
        ui->textOutput->clear();
        ui->textStegoMessage->clear();
        ui->lineFileInput->clear();
        ui->lineFileOutput->clear();
        showMinimized();
        logStatus(l10n("Panic mode: fields cleared and app minimized", "Паник-режим: поля очищены, приложение свернуто"));
        checkAchievements("panic_mode");
    });

    auto* shGame = new QShortcut(QKeySequence("Ctrl+Shift+G"), this);
    connect(shGame, &QShortcut::activated, this, &MainWindow::onMiniGame);
}

void MainWindow::maybeShowOnboarding() {
    QSettings settings(QDir(QCoreApplication::applicationDirPath()).filePath("settings.ini"), QSettings::IniFormat);
    if (settings.value("General/OnboardingDone", false).toBool()) {
        return;
    }

    QString text = l10n(
        "Welcome to SecureData Suite!\n\n"
        "Quick start:\n"
        "1) Choose algorithm in Cryptography tab\n"
        "2) Enter key and input text\n"
        "3) Encrypt/Decrypt\n"
        "4) Use Files tab for file operations (XOR/Simple AES)\n"
        "5) Press F1 for About\n\n"
        "Hotkeys: Ctrl+E encrypt, Ctrl+D decrypt, Ctrl+L language menu, Ctrl+1..5 tabs.",
        "Добро пожаловать в SecureData Suite!\n\n"
        "Быстрый старт:\n"
        "1) Выберите алгоритм во вкладке Криптография\n"
        "2) Введите ключ и текст\n"
        "3) Нажмите Зашифровать/Расшифровать\n"
        "4) Вкладка Файлы для операций с файлами (XOR/Simple AES)\n"
        "5) Нажмите F1 для О программе\n\n"
        "Горячие клавиши: Ctrl+E шифровать, Ctrl+D расшифровать, Ctrl+L меню языка.");
    QMessageBox::information(this, l10n("Welcome", "Добро пожаловать"), text);
    settings.setValue("General/OnboardingDone", true);
}

bool MainWindow::isSafeModeEnabled() const {
    QSettings settings(QDir(QCoreApplication::applicationDirPath()).filePath("settings.ini"), QSettings::IniFormat);
    return settings.value("General/SafeMode", false).toBool();
}

void MainWindow::applySafetyMode() {
    safeModeEnabled_ = isSafeModeEnabled();
    if (safeModeEnabled_) {
        ui->btnEggA->setEnabled(false);
        ui->btnEggB->setEnabled(false);
        if (secretButton_) {
            secretButton_->setEnabled(false);
        }
        ui->btnEggA->setToolTip(l10n("Disabled in Safe Mode", "Отключено в безопасном режиме"));
        ui->btnEggB->setToolTip(l10n("Disabled in Safe Mode", "Отключено в безопасном режиме"));
        logStatus(l10n("Safe mode is ON", "Безопасный режим включен"));
    }
}

void MainWindow::applyProfileMode() {
    QSettings settings(QDir(QCoreApplication::applicationDirPath()).filePath("settings.ini"), QSettings::IniFormat);
    const QString profile = settings.value("General/Profile", "student").toString().trimmed().toLower();

    if (profile == "demo") {
        ui->tabWidget->setCurrentWidget(ui->tabAbout);
        logStatus(l10n("Profile: demo", "Профиль: демо"));
    } else if (profile == "teacher") {
        logStatus(l10n("Profile: teacher", "Профиль: преподаватель"));
    } else {
        logStatus(l10n("Profile: student", "Профиль: студент"));
    }
}

void MainWindow::applySecurityModeUi() {
    QSettings settings(QDir(QCoreApplication::applicationDirPath()).filePath("settings.ini"), QSettings::IniFormat);
    const QString mode = settings.value("General/SecurityMode", "education").toString().trimmed().toLower();
    strongModeEnabled_ = (mode == "strong");

    if (ui->switchSecurityMode) {
        ui->switchSecurityMode->blockSignals(true);
        ui->switchSecurityMode->setChecked(strongModeEnabled_);
        ui->switchSecurityMode->blockSignals(false);
    }

    auto applyLabelStyle = [](QLabel* label, bool active) {
        if (!label) return;
        label->setStyleSheet(active
            ? "color:#E2E8F0; font-weight:700; font-size:13px;"
            : "color:#64748B; font-weight:500; font-size:11px;");
    };

    applyLabelStyle(ui->labelModeEducation, !strongModeEnabled_);
    applyLabelStyle(ui->labelModeStrong, strongModeEnabled_);

    if (ui->spinRsaBits) {
        if (strongModeEnabled_) {
            ui->spinRsaBits->setRange(1024, 4096);
            ui->spinRsaBits->setSingleStep(256);
            if (ui->spinRsaBits->value() < 1024) {
                ui->spinRsaBits->setValue(2048);
            }
        } else {
            ui->spinRsaBits->setRange(32, 56);
            ui->spinRsaBits->setSingleStep(4);
            if (ui->spinRsaBits->value() > 56) {
                ui->spinRsaBits->setValue(48);
            }
        }
    }

    rebuildCipherList();
}

void MainWindow::onSecurityModeChanged(int value) {
    strongModeEnabled_ = (value >= 1);
    QSettings settings(QDir(QCoreApplication::applicationDirPath()).filePath("settings.ini"), QSettings::IniFormat);
    settings.setValue("General/SecurityMode", strongModeEnabled_ ? "strong" : "education");
    applySecurityModeUi();
    showToast(strongModeEnabled_
                  ? l10n("Strong mode enabled: AES/XOR/RSA labels switched to strong profile.",
                         "Усиленный режим включен: метки AES/XOR/RSA переключены на strong-профиль.")
                  : l10n("Education mode enabled: AES/XOR/RSA labels switched to edu profile.",
                         "Учебный режим включен: метки AES/XOR/RSA переключены на edu-профиль."));
}

void MainWindow::onToggleSponsorShuffle() {
    sponsorShuffleEnabled_ = !sponsorShuffleEnabled_;
    QSettings settings(QDir(QCoreApplication::applicationDirPath()).filePath("settings.ini"), QSettings::IniFormat);
    settings.setValue("Easter/SponsorShuffleEnabled", sponsorShuffleEnabled_);
    logStatus(sponsorShuffleEnabled_
              ? l10n("Sponsor shuffle mode: ON", "Режим рандома спонсоров: ВКЛ")
              : l10n("Sponsor shuffle mode: OFF", "Режим рандома спонсоров: ВЫКЛ"));
    checkAchievements("toggle_shuffle");
}

void MainWindow::onMiniGame() {
    const QStringList ciphers = {"Caesar", "Vigenere", "Atbash", "XOR", "Permutation", "RSA"};
    const QStringList hintsEn = {
        "Hint from About: shifts letters by a fixed key.",
        "Hint from About: uses a keyword for multiple Caesar shifts.",
        "Hint from About: mirrors alphabet (A<->Z).",
        "Hint from About: bitwise operation with key bytes.",
        "Hint from About: reorders symbol positions using key order.",
        "Hint from About: asymmetric algorithm with public/private keys."
    };
    const QStringList hintsRu = {
        "Подсказка из About: сдвигает буквы на фиксированный ключ.",
        "Подсказка из About: использует ключевое слово для серии сдвигов.",
        "Подсказка из About: зеркалит алфавит (A<->Z).",
        "Подсказка из About: побитовая операция с байтами ключа.",
        "Подсказка из About: переставляет позиции символов по ключу.",
        "Подсказка из About: асимметричный алгоритм с публичным/приватным ключом."
    };
    const int idx = QRandomGenerator::global()->bounded(ciphers.size());
    const QString answer = ciphers.at(idx);
    const QString hint = (uiLocale_ == "ru_BY") ? hintsRu.at(idx) : hintsEn.at(idx);

    QMessageBox::information(
        this,
        l10n("Mini game hint", "Подсказка мини-игры"),
        hint
    );

    const QString input = QInputDialog::getText(
        this,
        l10n("Mini game", "Мини-игра"),
        l10n("Guess an algorithm name (hint: classic list):", "Угадай название алгоритма (подсказка: классический список):")
    );

    if (input.trimmed().isEmpty()) {
        return;
    }
    if (input.trimmed().compare(answer, Qt::CaseInsensitive) == 0) {
        showToast(l10n("Correct guess. Crypto sense +1", "Верно. Крипто-чутье +1"));
        checkAchievements("mini_game_win");
    } else {
        showToast(l10n("Correct answer: %1", "Правильный ответ: %1").arg(answer), true);
    }
}

void MainWindow::showRandomFortune() {
    const QStringList fortunesEn = {
        "Cipher first, trust later.",
        "A weak key is just a polite confession.",
        "Stego hides whispers in pixels.",
        "Backup is the real superpower."
    };
    const QStringList fortunesRu = {
        "Сначала шифруй, потом доверяй.",
        "Слабый ключ — это вежливое признание.",
        "Стего прячет шёпот в пикселях.",
        "Резервная копия — настоящая суперсила."
    };
    const int idx = QRandomGenerator::global()->bounded(fortunesEn.size());
    logStatus(uiLocale_ == "ru_BY" ? fortunesRu.at(idx) : fortunesEn.at(idx));
}

void MainWindow::triggerGlitchEffect() {
    const QString original = styleSheet();
    setStyleSheet(original + " QMainWindow { border: 2px solid #F43F5E; }");
    QTimer::singleShot(120, this, [this, original]() { setStyleSheet(original); });
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    static const QList<int> konami = {
        Qt::Key_Up, Qt::Key_Up, Qt::Key_Down, Qt::Key_Down,
        Qt::Key_Left, Qt::Key_Right, Qt::Key_Left, Qt::Key_Right,
        Qt::Key_B, Qt::Key_A
    };

    if (event->key() == konami.at(konamiProgress_)) {
        konamiProgress_++;
        if (konamiProgress_ >= konami.size()) {
            konamiProgress_ = 0;
            triggerGlitchEffect();
            showToast(l10n("Konami accepted. Reality has been encrypted.",
                           "Конами принят. Реальность зашифрована."));
            checkAchievements("konami");
        }
    } else {
        konamiProgress_ = (event->key() == Qt::Key_Up) ? 1 : 0;
    }

    QMainWindow::keyPressEvent(event);
}

void MainWindow::onOpenAbout() {
    ui->tabWidget->setCurrentWidget(ui->tabAbout);
    ui->textAbout->setFocus();
    showRandomFortune();
}

void MainWindow::onAboutLinkClicked(const QUrl& url) {
    if (url.isEmpty()) {
        return;
    }
    QDesktopServices::openUrl(url);
}

void MainWindow::refreshAboutText() {
    QSettings settings(QDir(QCoreApplication::applicationDirPath()).filePath("settings.ini"), QSettings::IniFormat);
    const QString urlCaesar = settings.value("AboutLinks/Caesar", "https://en.wikipedia.org/wiki/Caesar_cipher").toString();
    const QString urlVigenere = settings.value("AboutLinks/Vigenere", "https://en.wikipedia.org/wiki/Vigen%C3%A8re_cipher").toString();
    const QString urlAtbash = settings.value("AboutLinks/Atbash", "https://en.wikipedia.org/wiki/Atbash").toString();
    const QString urlXor = settings.value("AboutLinks/XOR", "https://en.wikipedia.org/wiki/XOR_cipher").toString();
    const QString urlPermutation = settings.value("AboutLinks/Permutation", "https://en.wikipedia.org/wiki/Transposition_cipher").toString();
    const QString urlAes = settings.value("AboutLinks/SimpleAES", "https://en.wikipedia.org/wiki/Advanced_Encryption_Standard").toString();
    const QString urlHuffman = settings.value("AboutLinks/Huffman", "https://en.wikipedia.org/wiki/Huffman_coding").toString();
    const QString urlRsa = settings.value("AboutLinks/RSA", "https://en.wikipedia.org/wiki/RSA_(cryptosystem)").toString();
    const QString urlLsb = settings.value("AboutLinks/LSB", "https://en.wikipedia.org/wiki/Steganography").toString();

    const QString html =
        "<html><head><meta charset='utf-8'/>"
        "<style>"
        "body{font-family:'Segoe UI';font-size:10pt;color:#E2E8F0;}"
        "a{color:#60A5FA;text-decoration:none;}"
        "a:hover{text-decoration:underline;}"
        "h2{color:#93C5FD;margin:0 0 10px 0;}"
        "h3{color:#BFDBFE;margin:12px 0 4px 0;}"
        "p{margin:0 0 8px 0;}"
        "</style></head><body>"
        "<h2>SecureData Suite</h2>"
        "<p>" + l10n("Educational toolkit for cryptography and steganography.",
                     "Учебный набор инструментов для криптографии и стеганографии.") + "</p>"

        "<h3><a href='" + urlCaesar + "'>Caesar</a></h3>"
        "<p>" + l10n("Shifts letters by a fixed key. Fast and simple, suitable for demonstration.",
                     "Сдвигает буквы на фиксированное число позиций. Простой и быстрый учебный метод.") + "</p>"

        "<h3><a href='" + urlVigenere + "'>Vigenere</a></h3>"
        "<p>" + l10n("Uses a keyword to apply multiple Caesar shifts. Stronger than monoalphabetic substitution.",
                     "Использует ключевое слово для серии сдвигов Цезаря. Надёжнее простых моноалфавитных замен.") + "</p>"

        "<h3><a href='" + urlAtbash + "'>Atbash</a></h3>"
        "<p>" + l10n("Mirrors the alphabet (A<->Z). Keyless educational substitution method.",
                     "Зеркально отображает алфавит (A<->Z). Бесключевой учебный метод подстановки.") + "</p>"

        "<h3><a href='" + urlXor + "'>XOR</a></h3>"
        "<p>" + l10n("Applies bitwise XOR with key bytes. Reversible and suitable for text and files.",
                     "Побитовое XOR с байтами ключа. Обратим и подходит для текста и файлов.") + "</p>"

        "<h3><a href='" + urlPermutation + "'>Permutation</a></h3>"
        "<p>" + l10n("Reorders symbol positions using key order. Demonstrates transposition ciphers.",
                     "Переставляет символы по порядку ключа. Демонстрирует транспозиционные шифры.") + "</p>"

        "<h3><a href='" + urlAes + "'>Simple AES</a></h3>"
        "<p>" + l10n("Educational block-cipher style implementation with byte transformations.",
                     "Учебная блочная схема шифрования с байтовыми преобразованиями.") + "</p>"

        "<h3><a href='" + urlHuffman + "'>Huffman</a></h3>"
        "<p>" + l10n("Variable-length coding for compression; not encryption by itself.",
                     "Кодирование переменной длины для сжатия; само по себе не является шифрованием.") + "</p>"

        "<h3><a href='" + urlRsa + "'>RSA</a></h3>"
        "<p>" + l10n("Asymmetric algorithm with public/private keys for secure key exchange and encryption.",
                     "Асимметричный алгоритм с публичным/приватным ключами для безопасного обмена и шифрования.") + "</p>"

        "<h3><a href='" + urlLsb + "'>LSB steganography</a></h3>"
        "<p>" + l10n("Hides message bits in least significant bits of image pixels (PNG/BMP).",
                     "Скрывает биты сообщения в младших битах пикселей изображения (PNG/BMP).") + "</p>"

        "<h3>" + l10n("Extra modes", "Дополнительные режимы") + "</h3>"
        "<p>" + l10n("Mini-game: press Ctrl+Shift+G to run Cipher Challenge.",
                     "Мини-игра: нажмите Ctrl+Shift+G для запуска Cipher Challenge.") + "</p>"
        "<p>" + l10n("Panic mode: press Ctrl+Shift+X to quickly clear fields and minimize the window.",
                     "Паник-режим: нажмите Ctrl+Shift+X, чтобы быстро очистить поля и свернуть окно.") + "</p>"

        "<h3>" + l10n("Hotkeys", "Горячие клавиши") + "</h3>"
        "<p>" + l10n("Ctrl+E encrypt, Ctrl+D decrypt, Ctrl+W swap input/output, Ctrl+L language menu.",
                     "Ctrl+E шифровать, Ctrl+D расшифровать, Ctrl+W поменять вход/выход, Ctrl+L меню языка.") + "</p>"
        "<p>" + l10n("Ctrl+1..5 switch tabs, F5 opens Settings, F1 opens About.",
                     "Ctrl+1..5 переключение вкладок, F5 открывает Настройки, F1 открывает О программе.") + "</p>"
        "<p style='color:#94A3B8;'>" + l10n("Tip: press F1 to open this About tab quickly.",
                                            "Подсказка: нажмите F1, чтобы быстро открыть вкладку «О программе».") + "</p>"
        "</body></html>";

    ui->textAbout->setOpenExternalLinks(true);
    ui->textAbout->setOpenLinks(true);
    ui->textAbout->setHtml(html);
}

void MainWindow::applyTheme() {
    setStyleSheet(
        "QMainWindow {"
        " background: qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 #0B1221, stop:1 #1E293B);"
        " color: #E2E8F0;"
        "}"
        "QTabWidget::pane {"
        " border: 1px solid #334155;"
        " border-radius: 10px;"
        " background: rgba(15, 23, 42, 0.90);"
        "}"
        "QTabBar::tab {"
        " background: #111827;"
        " color: #CBD5E1;"
        " border: 1px solid #334155;"
        " border-bottom: none;"
        " border-top-left-radius: 8px;"
        " border-top-right-radius: 8px;"
        " padding: 8px 16px;"
        " margin-right: 4px;"
        "}"
        "QTabBar::tab:selected { background: #1D4ED8; color: white; }"
        "QGroupBox {"
        " border: 1px solid #334155;"
        " border-radius: 8px;"
        " margin-top: 10px;"
        " padding-top: 8px;"
        " font-weight: 600;"
        "}"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 6px; }"
        "QLineEdit, QPlainTextEdit, QTextBrowser, QComboBox, QSpinBox {"
        " background: #0F172A;"
        " color: #E2E8F0;"
        " border: 1px solid #334155;"
        " border-radius: 6px;"
        " padding: 6px;"
        "}"
        "QPushButton {"
        " background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #2563EB, stop:1 #1D4ED8);"
        " color: white;"
        " border: none;"
        " border-radius: 7px;"
        " padding: 7px 12px;"
        " font-weight: 600;"
        "}"
        "QPushButton:hover { background: #3B82F6; }"
        "QPushButton:pressed { background: #1E40AF; }"
        "QToolButton {"
        " background: #1E293B;"
        " color: #E2E8F0;"
        " border: 1px solid #334155;"
        " border-radius: 7px;"
        " padding: 6px 10px;"
        " font-weight: 600;"
        "}"
        "QToolButton:hover { background: #334155; }"
        "QCheckBox { color: #E2E8F0; }"
        "QCheckBox#switchSecurityMode::indicator {"
        " width: 46px;"
        " height: 24px;"
        " border-radius: 12px;"
        " background: #334155;"
        " border: 1px solid #475569;"
        "}"
        "QCheckBox#switchSecurityMode::indicator:checked {"
        " background: #1D4ED8;"
        " border: 1px solid #60A5FA;"
        "}"
        "QWidget#tabFiles QGroupBox, QWidget#tabSettings QGroupBox {"
        " border: 1px solid #2F4469;"
        " border-radius: 10px;"
        " background: rgba(15, 27, 48, 0.48);"
        " margin-top: 10px;"
        " padding-top: 8px;"
        "}"
        "QWidget#tabFiles QGroupBox::title, QWidget#tabSettings QGroupBox::title {"
        " color: #D7E7FF;"
        " left: 10px;"
        " padding: 0 6px;"
        "}"
        "QWidget#tabFiles QLineEdit, QWidget#tabSettings QComboBox {"
        " background: #10203A;"
        " border: 1px solid #37517C;"
        " color: #E8F0FF;"
        "}"
        "QWidget#tabFiles QPushButton, QWidget#tabSettings QPushButton {"
        " background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #2B59C3, stop:1 #274DB2);"
        " border-radius: 8px;"
        "}"
        "QWidget#tabFiles QPushButton:hover, QWidget#tabSettings QPushButton:hover { background: #3567D8; }"
        "QWidget#tabFiles QProgressBar, QWidget#tabSettings QProgressBar {"
        " border: 1px solid #35507A;"
        " border-radius: 6px;"
        " background: #0F1F37;"
        " color: #DCE8FF;"
        " text-align: center;"
        "}"
        "QWidget#tabFiles QProgressBar::chunk, QWidget#tabSettings QProgressBar::chunk {"
        " background: qlineargradient(x1:0,y1:0,x2:1,y2:0, stop:0 #3B82F6, stop:1 #60A5FA);"
        " border-radius: 5px;"
        "}"
        "QMenuBar, QMenu { background: #0F172A; color: #E2E8F0; }"
    );

    const int margin = compactModeEnabled_ ? 6 : 10;
    const int spacing = compactModeEnabled_ ? 6 : 8;
    const int buttonH = compactModeEnabled_ ? 32 : 36;
    const auto applyMargins = [margin](QLayout* layout) {
        if (!layout) return;
        layout->setContentsMargins(margin, margin, margin, qMax(6, margin - 1));
    };

    applyMargins(ui->layoutFiles);
    applyMargins(ui->layoutSettings);
    applyMargins(ui->layoutAchievementsCard);
    applyMargins(ui->layoutSecurityModeCard);
    applyMargins(ui->layoutPreferences);
    ui->layoutFiles->setSpacing(spacing);
    ui->layoutSettings->setSpacing(spacing);
    ui->layoutAchievementButtons->setSpacing(spacing);
    ui->layoutFileActions->setSpacing(spacing);

    const QList<QPushButton*> compactButtons = {
        ui->btnFileBrowseInput, ui->btnFileBrowseOutput, ui->btnOpenCache,
        ui->btnFileEncrypt, ui->btnFileDecrypt, ui->btnRefreshAchievements, ui->btnResetAchievements
    };
    for (QPushButton* b : compactButtons) {
        if (!b) continue;
        b->setMinimumHeight(buttonH);
    }
}

void MainWindow::launchEasterExecutable(const QString& settingKey, const QString& fallbackExe) {
    if (safeModeEnabled_) {
        showError(l10n("External launch blocked in Safe Mode.", "Запуск внешних файлов заблокирован в безопасном режиме."));
        return;
    }

    const QString appDir = QCoreApplication::applicationDirPath();
    QSettings settings(QDir(appDir).filePath("settings.ini"), QSettings::IniFormat);

    QString path = settings.value(settingKey, fallbackExe).toString().trimmed();
    if (path.isEmpty()) {
        path = fallbackExe;
    }

    QFileInfo info(path);
    if (info.isRelative()) {
        path = QDir(appDir).filePath(path);
    }

    if (!QFileInfo::exists(path)) {
        showError(tr("Easter executable not found: %1").arg(path));
        return;
    }

    if (!QProcess::startDetached(path)) {
        showError(tr("Failed to start executable: %1").arg(path));
    }
}

void MainWindow::onAboutSecretA() {
    if (false && sponsorShuffleEnabled_) {
        QSettings settings(QDir(QCoreApplication::applicationDirPath()).filePath("settings.ini"), QSettings::IniFormat);
        const QStringList links = {
            settings.value("Easter/SponsorA_Url", "https://example.com/a").toString(),
            settings.value("Easter/SponsorB_Url", "https://example.com/b").toString(),
            settings.value("Easter/SponsorC_Url", "https://example.com/c").toString()
        };
        const int idx = QRandomGenerator::global()->bounded(links.size());
        const QString target = links.at(idx).trimmed();
        if (!target.isEmpty()) {
            QDesktopServices::openUrl(QUrl(target));
            logStatus(l10n("Sponsor shuffle: opened random link", "Режим рандома: открыта случайная ссылка"));
            checkAchievements("sponsor_shuffle");
            return;
        }
    }

    runSponsorAction("SponsorA", "gif",
                     "sponsor_a.exe",
                     "cat-eating-chips-windows-xp-meme.gif",
                     "https://example.com",
                     sponsorALinkMode_);
}

void MainWindow::onAboutSecretB() {
    runSponsorAction("SponsorB", "url",
                     "sponsor_b.exe",
                     "cat-eating-chips-windows-xp-meme.gif",
                     "https://github.com/Lavoris-Sec",
                     sponsorBLinkMode_);
}

void MainWindow::onAboutSecretC() {
    QSettings settings(QDir(QCoreApplication::applicationDirPath()).filePath("settings.ini"), QSettings::IniFormat);
    const QString url = settings.value("Easter/SponsorC_Url", "https://example.com/c").toString().trimmed();
    if (!url.isEmpty()) {
        QDesktopServices::openUrl(QUrl(url));
        logStatus(l10n("Sponsor C link opened", "Ссылка Sponsor C открыта"));
        checkAchievements("sponsor_c_open");
    }
}

void MainWindow::runSponsorAction(const QString& sponsorKey, const QString& defaultMode,
                                  const QString& defaultExe, const QString& defaultGif,
                                  const QString& defaultUrl, bool linkModeActive) {
    if (safeModeEnabled_) {
        showError(l10n("Sponsor actions are disabled in Safe Mode.", "Действия спонсоров отключены в безопасном режиме."));
        return;
    }

    const QString appDir = QCoreApplication::applicationDirPath();
    QSettings settings(QDir(appDir).filePath("settings.ini"), QSettings::IniFormat);

    const QString keyPrefix = QString("Easter/%1_").arg(sponsorKey);
    const QString mode = settings.value(keyPrefix + "Mode", defaultMode).toString().trimmed().toLower();
    QString exePath = settings.value(keyPrefix + "Exe", defaultExe).toString().trimmed();
    QString gifPath = settings.value(keyPrefix + "Gif", defaultGif).toString().trimmed();
    const QString url = settings.value(keyPrefix + "Url", defaultUrl).toString().trimmed();

    if (linkModeActive && !url.isEmpty()) {
        QDesktopServices::openUrl(QUrl(url));
        return;
    }

    const auto resolveToAppDir = [&](const QString& path)->QString {
        if (path.isEmpty()) {
            return path;
        }
        QFileInfo info(path);
        return info.isRelative() ? QDir(appDir).filePath(path) : path;
    };

    exePath = resolveToAppDir(exePath);
    gifPath = resolveToAppDir(gifPath);

    if (mode == "none") {
        return;
    }
    if (mode == "exe") {
        if (!exePath.isEmpty() && QFileInfo::exists(exePath)) {
            if (!QProcess::startDetached(exePath)) {
                showError(l10n("Failed to start executable: %1", "Не удалось запустить исполняемый файл: %1").arg(exePath));
            }
        } else {
            showError(l10n("Executable not found: %1", "Исполняемый файл не найден: %1").arg(exePath));
        }
        return;
    }
    if (mode == "gif") {
        showSponsorGifPopup(gifPath, sponsorKey == "SponsorA" ? l10n("Sponsor A", "Спонсор A")
                                                               : l10n("Sponsor B", "Спонсор B"));
        return;
    }
    if (mode == "random_gif_or_none") {
        if (QRandomGenerator::global()->bounded(100) < 50) {
            showSponsorGifPopup(gifPath, sponsorKey == "SponsorA" ? l10n("Sponsor A", "Спонсор A")
                                                                   : l10n("Sponsor B", "Спонсор B"));
        }
        return;
    }
    if (mode == "random_exe_or_none") {
        if (QRandomGenerator::global()->bounded(100) < 50 && QFileInfo::exists(exePath)) {
            QProcess::startDetached(exePath);
        }
        return;
    }
    if (mode == "random_gif_or_exe") {
        if (QRandomGenerator::global()->bounded(100) < 50) {
            showSponsorGifPopup(gifPath, sponsorKey == "SponsorA" ? l10n("Sponsor A", "Спонсор A")
                                                                   : l10n("Sponsor B", "Спонсор B"));
        } else if (QFileInfo::exists(exePath)) {
            QProcess::startDetached(exePath);
        }
        return;
    }
    if (mode == "url") {
        if (!url.isEmpty()) {
            QDesktopServices::openUrl(QUrl(url));
        }
        return;
    }

    // Fallback behavior.
    if (!exePath.isEmpty() && QFileInfo::exists(exePath)) {
        QProcess::startDetached(exePath);
        return;
    }
    if (!gifPath.isEmpty()) {
        showSponsorGifPopup(gifPath, sponsorKey == "SponsorA" ? l10n("Sponsor A", "Спонсор A")
                                                               : l10n("Sponsor B", "Спонсор B"));
    }
}

void MainWindow::showSponsorGifPopup(const QString& gifPath, const QString& title) {
    QString source = gifPath;
    if (!QFileInfo::exists(source)) {
        const QString appDir = QCoreApplication::applicationDirPath();
        const QString altLocal = QDir(appDir).filePath("cat-eating-chips-windows-xp-meme.gif");
        const QString altLocalOld = QDir(appDir).filePath("cat eating chips Windows XP Meme.gif");
        const QString projectRootGif = QDir(appDir).filePath("../../cat-eating-chips-windows-xp-meme.gif");
        const QString projectRootGifOld = QDir(appDir).filePath("../../cat eating chips Windows XP Meme.gif");
        const QString resourcesGif = QDir(appDir).filePath("../../resources/cat-eating-chips-windows-xp-meme.gif");
        const QString resourcesGifOld = QDir(appDir).filePath("../../resources/cat eating chips Windows XP Meme.gif");
        if (QFileInfo::exists(altLocal)) {
            source = altLocal;
        } else if (QFileInfo::exists(altLocalOld)) {
            source = altLocalOld;
        } else if (QFileInfo::exists(projectRootGif)) {
            source = projectRootGif;
        } else if (QFileInfo::exists(projectRootGifOld)) {
            source = projectRootGifOld;
        } else if (QFileInfo::exists(resourcesGif)) {
            source = resourcesGif;
        } else if (QFileInfo::exists(resourcesGifOld)) {
            source = resourcesGifOld;
        } else {
            source = ":/cat-eating-chips-windows-xp-meme.gif";
        }
    }

    QDialog dialog(this);
    dialog.setWindowTitle(title);
    dialog.setWindowIcon(createSponsorGifIcon());
    dialog.setModal(false);
    dialog.setAttribute(Qt::WA_DeleteOnClose, false);

    auto* layout = new QVBoxLayout(&dialog);
    layout->setContentsMargins(10, 10, 10, 10);
    auto* label = new QLabel(&dialog);
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);

    QMovie movie(source);
    if (!movie.isValid()) {
        movie.setFileName(":/cat-eating-chips-windows-xp-meme.gif");
        if (!movie.isValid()) {
            label->setText(l10n("GIF not found or invalid.", "GIF не найден или поврежден."));
            dialog.resize(360, 120);
            dialog.exec();
            return;
        }
    }

    label->setMovie(&movie);
    movie.start();
    dialog.resize(520, 380);
    dialog.exec();
}

} // namespace sds
