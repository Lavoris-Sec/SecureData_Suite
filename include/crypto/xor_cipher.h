#ifndef SDS_XOR_CIPHER_H
#define SDS_XOR_CIPHER_H

#include "crypto/cipher_base.h"

namespace sds {

class XorCipher final : public CipherBase {
public:
    QString id() const override { return "xor"; }
    QString name() const override { return "XOR"; }
    QString description() const override { return "Byte-wise XOR with a repeating key."; }
    bool supportsBinary() const override { return true; }
    QString keyHint() const override { return "Text key (bytes repeat)"; }

    QString encryptText(const QString& text, const QString& key, QString* error) const override;
    QString decryptText(const QString& text, const QString& key, QString* error) const override;
    QByteArray encryptBytes(const QByteArray& data, const QString& key, QString* error) const override;
    QByteArray decryptBytes(const QByteArray& data, const QString& key, QString* error) const override;
};

} // namespace sds

#endif // SDS_XOR_CIPHER_H
