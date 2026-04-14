#ifndef SDS_VIGENERE_CIPHER_H
#define SDS_VIGENERE_CIPHER_H

#include "crypto/cipher_base.h"

namespace sds {

class VigenereCipher final : public CipherBase {
public:
    QString id() const override { return "vigenere"; }
    QString name() const override { return "Vigenere"; }
    QString description() const override { return "Polyalphabetic substitution using a keyword."; }
    QString keyHint() const override { return "Keyword (letters only)"; }

    QString encryptText(const QString& text, const QString& key, QString* error) const override;
    QString decryptText(const QString& text, const QString& key, QString* error) const override;
};

} // namespace sds

#endif // SDS_VIGENERE_CIPHER_H
