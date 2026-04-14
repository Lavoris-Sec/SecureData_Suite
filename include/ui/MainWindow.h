#ifndef SDS_MAINWINDOW_H
#define SDS_MAINWINDOW_H

#include <QMainWindow>
#include <QPointer>
#include "crypto/cipher_manager.h"
#include "crypto/rsa_cipher.h"
#include "utils/file_cache.h"
#include "utils/translation_manager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QPushButton;
class QResizeEvent;
class QEvent;

namespace sds {

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    void changeEvent(QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onEncryptClicked();
    void onDecryptClicked();
    void onSwapClicked();
    void onCipherChanged(int index);
    void onRsaGenerateClicked();

    void onStegoBrowseInput();
    void onStegoBrowseOutput();
    void onStegoEmbed();
    void onStegoExtract();

    void onFileBrowseInput();
    void onFileBrowseOutput();
    void onFileEncrypt();
    void onFileDecrypt();
    void onOpenCache();

    void onLanguageEnglish();
    void onLanguageRussian();

    void onAboutSecretA();
    void onAboutSecretB();

private:
    const sds::CipherBase* currentCipher() const;
    void updateCipherUi();
    void showError(const QString& message);
    void setupLanguageMenu();
    void setupSecretButton();
    void updateSecretButtonPosition();
    void applyTheme();
    void launchEasterExecutable(const QString& settingKey, const QString& fallbackExe);

    Ui::MainWindow* ui;
    sds::CipherManager cipherManager_;
    sds::FileCache fileCache_;
    sds::TranslationManager translationManager_;
    QPointer<QPushButton> secretButton_;
};

} // namespace sds

#endif // SDS_MAINWINDOW_H
