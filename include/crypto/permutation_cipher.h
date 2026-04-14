#ifndef SDS_PERMUTATION_CIPHER_H
#define SDS_PERMUTATION_CIPHER_H

#include "crypto/cipher_base.h"

namespace sds {

class PermutationCipher final : public CipherBase {
public:
    QString id() const override { return "permutation"; }
    QString name() const override { return "Permutation"; }
    QString description() const override { return "Columnar transposition based on a keyword."; }
    QString keyHint() const override { return "Keyword (letters or digits)"; }

    QString encryptText(const QString& text, const QString& key, QString* error) const override;
    QString decryptText(const QString& text, const QString& key, QString* error) const override;
};

} // namespace sds

#endif // SDS_PERMUTATION_CIPHER_H
