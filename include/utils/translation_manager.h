#ifndef SDS_TRANSLATION_MANAGER_H
#define SDS_TRANSLATION_MANAGER_H

#include <QObject>
#include <QTranslator>

namespace sds {

class TranslationManager : public QObject {
    Q_OBJECT

public:
    explicit TranslationManager(QObject* parent = nullptr);

    bool loadLanguage(const QString& locale);
    void clear();
    QString currentLocale() const { return currentLocale_; }

signals:
    void languageChanged();

private:
    QString currentLocale_ = "en";
    QTranslator translator_;
};

} // namespace sds

#endif // SDS_TRANSLATION_MANAGER_H
