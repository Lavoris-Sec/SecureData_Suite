#ifndef SDS_SIMPLE_AES_CIPHER_H
#define SDS_SIMPLE_AES_CIPHER_H

#include "crypto/cipher_base.h"

namespace sds {

class SimpleAesCipher final : public CipherBase {
public:
    QString id() const override { return "simple_aes"; }
    QString name() const override { return "Simple AES"; }
    QString description() const override { return "Educational AES-128 (ECB, PKCS#7)."; }
    bool supportsBinary() const override { return true; }
    QString keyHint() const override { return "Key (16+ chars, UTF-8)"; }

    QString encryptText(const QString& text, const QString& key, QString* error) const override;
    QString decryptText(const QString& text, const QString& key, QString* error) const override;
    QByteArray encryptBytes(const QByteArray& data, const QString& key, QString* error) const override;
    QByteArray decryptBytes(const QByteArray& data, const QString& key, QString* error) const override;
};

} // namespace sds

#endif // SDS_SIMPLE_AES_CIPHER_H
