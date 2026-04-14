#ifndef SDS_CAESAR_CIPHER_H
#define SDS_CAESAR_CIPHER_H

#include "crypto/cipher_base.h"

namespace sds {

class CaesarCipher final : public CipherBase {
public:
    QString id() const override { return "caesar"; }
    QString name() const override { return "Caesar"; }
    QString description() const override { return "Shift letters by a fixed number of positions."; }
    QString keyHint() const override { return "Integer shift (e.g., 3)"; }

    QString encryptText(const QString& text, const QString& key, QString* error) const override;
    QString decryptText(const QString& text, const QString& key, QString* error) const override;
};

} // namespace sds

#endif // SDS_CAESAR_CIPHER_H
