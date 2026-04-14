#include "crypto/vigenere_cipher.h"

namespace sds {

static bool normalizeKey(const QString& key, QString* out, QString* error) {
    QString cleaned;
    cleaned.reserve(key.size());
    for (const QChar& c : key) {
        if (c.isLetter()) {
            cleaned.append(c.toLower());
        }
    }

    if (cleaned.isEmpty()) {
        if (error) {
            *error = "Key must contain at least one letter.";
        }
        return false;
    }

    *out = cleaned;
    return true;
}

static QString vigenereTransform(const QString& text, const QString& key, bool encrypt) {
    QString result;
    result.reserve(text.size());

    int keyIndex = 0;
    const int keyLen = key.size();

    for (const QChar& c : text) {
        if (c.isLetter()) {
            const bool upper = c.isUpper();
            const int base = upper ? 'A' : 'a';
            const int code = c.toLatin1() - base;

            const int keyShift = key.at(keyIndex % keyLen).toLatin1() - 'a';
            const int shift = encrypt ? keyShift : -keyShift;

            const int shifted = (code + shift + 26) % 26;
            result.append(QChar(base + shifted));

            keyIndex++;
        } else {
            result.append(c);
        }
    }

    return result;
}

QString VigenereCipher::encryptText(const QString& text, const QString& key, QString* error) const {
    QString normalized;
    if (!normalizeKey(key, &normalized, error)) {
        return {};
    }
    return vigenereTransform(text, normalized, true);
}

QString VigenereCipher::decryptText(const QString& text, const QString& key, QString* error) const {
    QString normalized;
    if (!normalizeKey(key, &normalized, error)) {
        return {};
    }
    return vigenereTransform(text, normalized, false);
}

} // namespace sds
