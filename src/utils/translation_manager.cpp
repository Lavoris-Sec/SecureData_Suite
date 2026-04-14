#include "utils/translation_manager.h"
#include <QApplication>
#include <QDir>

namespace sds {

TranslationManager::TranslationManager(QObject* parent)
    : QObject(parent) {}

bool TranslationManager::loadLanguage(const QString& locale) {
    if (locale == currentLocale_) {
        return true;
    }

    qApp->removeTranslator(&translator_);

    const QString fileName = QString("SecureData_Suite_%1.qm").arg(locale);
    const QString path = QDir(qApp->applicationDirPath()).filePath(fileName);
    if (!translator_.load(path)) {
        return false;
    }

    qApp->installTranslator(&translator_);
    currentLocale_ = locale;
    emit languageChanged();
    return true;
}

void TranslationManager::clear() {
    qApp->removeTranslator(&translator_);
    currentLocale_ = "en";
    emit languageChanged();
}

} // namespace sds
