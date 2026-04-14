#include "ui/MainWindow.h"
#include "ui_MainWindow.h"

#include "stego/lsb_steganography.h"

#include <QCoreApplication>
#include <QDesktopServices>
#include <QDir>
#include <QEvent>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QMenu>
#include <QMessageBox>
#include <QProcess>
#include <QPushButton>
#include <QResizeEvent>
#include <QSettings>
#include <QUrl>

namespace sds {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , cipherManager_()
    , fileCache_()
    , translationManager_(this) {
    ui->setupUi(this);
    applyTheme();

    for (const auto& cipher : cipherManager_.ciphers()) {
        ui->comboCipher->addItem(cipher->name(), cipher->id());
    }

    connect(ui->btnEncrypt, &QPushButton::clicked, this, &MainWindow::onEncryptClicked);
    connect(ui->btnDecrypt, &QPushButton::clicked, this, &MainWindow::onDecryptClicked);
    connect(ui->btnSwap, &QPushButton::clicked, this, &MainWindow::onSwapClicked);
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

    ui->labelCachePath->setText(fileCache_.cacheDir());

    setupLanguageMenu();
    setupSecretButton();
    updateCipherUi();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::changeEvent(QEvent* event) {
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    QMainWindow::changeEvent(event);
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    updateSecretButtonPosition();
}

const CipherBase* MainWindow::currentCipher() const {
    const QVariant id = ui->comboCipher->currentData();
    return cipherManager_.findById(id.toString());
}

void MainWindow::updateCipherUi() {
    const CipherBase* cipher = currentCipher();
    if (!cipher) {
        return;
    }

    ui->lineKey->setEnabled(cipher->requiresKey() && cipher->id() != "rsa");
    ui->lineKey->setPlaceholderText(cipher->keyHint());
    ui->groupRsa->setVisible(cipher->id() == "rsa");
}

void MainWindow::showError(const QString& message) {
    QMessageBox::warning(this, tr("Error"), message);
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
    const RsaKeyPair pair = rsa.generateKeyPair(bits, &error);
    if (!error.isEmpty()) {
        showError(error);
        return;
    }
    ui->lineRsaPublic->setText(pair.publicKey);
    ui->lineRsaPrivate->setText(pair.privateKey);
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

    QMessageBox::information(this, tr("Done"), tr("Message embedded into the image."));
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

    QMessageBox::information(this, tr("Done"), tr("File encrypted."));
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

    QMessageBox::information(this, tr("Done"), tr("File decrypted."));
}

void MainWindow::onOpenCache() {
    QDesktopServices::openUrl(QUrl::fromLocalFile(fileCache_.cacheDir()));
}

void MainWindow::setupLanguageMenu() {
    QMenu* langMenu = menuBar()->addMenu(tr("Language"));
    QAction* actionEn = langMenu->addAction(tr("English"));
    QAction* actionRu = langMenu->addAction(tr("Russian"));

    connect(actionEn, &QAction::triggered, this, &MainWindow::onLanguageEnglish);
    connect(actionRu, &QAction::triggered, this, &MainWindow::onLanguageRussian);
}

void MainWindow::onLanguageEnglish() {
    translationManager_.clear();
    QMessageBox::information(this, tr("Language"), tr("English language applied."));
}

void MainWindow::onLanguageRussian() {
    if (!translationManager_.loadLanguage("ru_BY")) {
        showError(tr("Failed to load Russian translation file."));
        return;
    }
    QMessageBox::information(this, tr("Language"), tr("Russian language applied."));
}

void MainWindow::setupSecretButton() {
    secretButton_ = new QPushButton(this);
    secretButton_->setObjectName("secretButton");
    secretButton_->setText("");
    secretButton_->setFlat(true);
    secretButton_->setFixedSize(18, 18);
    secretButton_->setStyleSheet("background: transparent; border: none;");
    secretButton_->raise();
    connect(secretButton_, &QPushButton::clicked, this, &MainWindow::onAboutSecretA);
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
        "QCheckBox { color: #E2E8F0; }"
        "QMenuBar, QMenu { background: #0F172A; color: #E2E8F0; }"
    );
}

void MainWindow::launchEasterExecutable(const QString& settingKey, const QString& fallbackExe) {
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
    launchEasterExecutable("Easter/ExeA", "easter_a.exe");
}

void MainWindow::onAboutSecretB() {
    launchEasterExecutable("Easter/ExeB", "easter_b.exe");
}

} // namespace sds
