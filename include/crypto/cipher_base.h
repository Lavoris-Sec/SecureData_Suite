#ifndef SDS_CIPHER_BASE_H
#define SDS_CIPHER_BASE_H

#include <QString>
#include <QByteArray>

namespace sds {

class CipherBase {
public:
    virtual ~CipherBase() = default;

    virtual QString id() const = 0;
    virtual QString name() const = 0;
    virtual QString description() const = 0;

    virtual bool supportsBinary() const { return false; }
    virtual bool requiresKey() const { return true; }
    virtual QString keyHint() const { return QString(); }

    virtual QString encryptText(const QString& text, const QString& key, QString* error) const = 0;
    virtual QString decryptText(const QString& text, const QString& key, QString* error) const = 0;

    virtual QByteArray encryptBytes(const QByteArray& data, const QString& key, QString* error) const;
    virtual QByteArray decryptBytes(const QByteArray& data, const QString& key, QString* error) const;
};

} // namespace sds

#endif // SDS_CIPHER_BASE_H
