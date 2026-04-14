#ifndef SDS_ATBASH_CIPHER_H
#define SDS_ATBASH_CIPHER_H

#include "crypto/cipher_base.h"

namespace sds {

class AtbashCipher final : public CipherBase {
public:
    QString id() const override { return "atbash"; }
    QString name() const override { return "Atbash"; }
    QString description() const override { return "Mirror alphabet substitution."; }
    bool requiresKey() const override { return false; }

    QString encryptText(const QString& text, const QString& key, QString* error) const override;
    QString decryptText(const QString& text, const QString& key, QString* error) const override;
};

} // namespace sds

#endif // SDS_ATBASH_CIPHER_H
