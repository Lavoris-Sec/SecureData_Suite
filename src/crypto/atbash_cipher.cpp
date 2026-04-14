#include "crypto/atbash_cipher.h"

namespace sds {

static QString atbashTransform(const QString& text) {
    QString result;
    result.reserve(text.size());

    for (const QChar& c : text) {
        if (c.isLetter()) {
            const bool upper = c.isUpper();
            const int base = upper ? 'A' : 'a';
            const int code = c.toLatin1() - base;
            const int mirrored = 25 - code;
            result.append(QChar(base + mirrored));
        } else {
            result.append(c);
        }
    }

    return result;
}

QString AtbashCipher::encryptText(const QString& text, const QString& key, QString* error) const {
    Q_UNUSED(key)
    Q_UNUSED(error)
    return atbashTransform(text);
}

QString AtbashCipher::decryptText(const QString& text, const QString& key, QString* error) const {
    Q_UNUSED(key)
    Q_UNUSED(error)
    return atbashTransform(text);
}

} // namespace sds
