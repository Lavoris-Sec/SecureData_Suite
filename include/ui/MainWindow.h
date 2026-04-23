#ifndef SDS_MAINWINDOW_H
#define SDS_MAINWINDOW_H

#include <QMainWindow>
#include <QPointer>
#include <QJsonObject>
#include "crypto/cipher_manager.h"
#include "crypto/rsa_cipher.h"
#include "utils/file_cache.h"
#include "utils/translation_manager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QPushButton;
class QToolButton;
class QLabel;
class QResizeEvent;
class QEvent;
class QKeyEvent;
class QShortcut;
class QUrl;

namespace sds {

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    void changeEvent(QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

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
    void onOpenAbout();
    void onAboutLinkClicked(const QUrl& url);
    void onToggleSponsorShuffle();
    void onMiniGame();
    void onAchievementSelected(int index);
    void onResetAchievements();
    void onRefreshAchievements();

    void onAboutSecretA();
    void onAboutSecretB();
    void onAboutSecretC();

private:
    const sds::CipherBase* currentCipher() const;
    void updateCipherUi();
    void showError(const QString& message);
    void setupLanguageMenu();
    void setupSecretButton();
    void updateSecretButtonPosition();
    void updateLanguageButtonPosition();
    void updateLanguageButtonVisuals();
    void applyManualTranslations();
    QString l10n(const QString& en, const QString& ru) const;
    void applySponsorButtonModes();
    void applyButtonLinkStyle(QPushButton* button, bool asLink);
    void runSponsorAction(const QString& sponsorKey, const QString& defaultMode,
                          const QString& defaultExe, const QString& defaultGif,
                          const QString& defaultUrl, bool linkModeActive);
    void showSponsorGifPopup(const QString& gifPath, const QString& title);
    void showRandomFortune();
    void triggerGlitchEffect();
    void logStatus(const QString& message);
    void checkAchievements(const QString& eventKey);
    QString achievementFilePath() const;
    void loadAchievements();
    void saveAchievements() const;
    void refreshAchievementsUi();
    void incrementAchievementCounter(const QString& key);
    int achievementCounter(const QString& key) const;
    bool isAchievementUnlocked(const QString& key) const;
    void unlockAchievement(const QString& key, const QString& titleEn, const QString& titleRu);
    void setupHotkeys();
    void maybeShowOnboarding();
    bool isSafeModeEnabled() const;
    void applySafetyMode();
    void applyProfileMode();
    void applyTheme();
    void refreshAboutText();
    void launchEasterExecutable(const QString& settingKey, const QString& fallbackExe);

    Ui::MainWindow* ui;
    sds::CipherManager cipherManager_;
    sds::FileCache fileCache_;
    sds::TranslationManager translationManager_;
    QPointer<QPushButton> secretButton_;
    QPointer<QToolButton> languageButton_;
    QPointer<QLabel> languageFlagLabel_;
    QPointer<QShortcut> aboutShortcut_;
    QString uiLocale_ = "en";
    bool sponsorALinkMode_ = false; // kept for compatibility with runSponsorAction
    bool sponsorBLinkMode_ = false; // kept for compatibility with runSponsorAction
    bool sponsorShuffleEnabled_ = true;
    int konamiProgress_ = 0;
    bool safeModeEnabled_ = false;
    QJsonObject achievementsState_;
};

} // namespace sds

#endif // SDS_MAINWINDOW_H
