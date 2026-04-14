#ifndef SDS_RSA_CIPHER_H
#define SDS_RSA_CIPHER_H

#include "crypto/cipher_base.h"
#include <QString>

namespace sds {

struct RsaKeyPair {
    QString publicKey;
    QString privateKey;
};

class RsaCipher final : public CipherBase {
public:
    QString id() const override { return "rsa"; }
    QString name() const override { return "RSA"; }
    QString description() const override { return "Educational RSA with generated key pairs."; }
    QString keyHint() const override { return "Public: e,n  |  Private: d,n"; }

    QString encryptText(const QString& text, const QString& key, QString* error) const override;
    QString decryptText(const QString& text, const QString& key, QString* error) const override;

    RsaKeyPair generateKeyPair(int bits, QString* error) const;
};

} // namespace sds

#endif // SDS_RSA_CIPHER_H
