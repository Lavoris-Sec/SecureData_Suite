#include "crypto/xor_cipher.h"
#include <QByteArray>

namespace sds {

static bool getKeyBytes(const QString& key, QByteArray* out, QString* error) {
    const QByteArray bytes = key.toUtf8();
    if (bytes.isEmpty()) {
        if (error) {
            *error = "Key must not be empty.";
        }
        return false;
    }
    *out = bytes;
    return true;
}

static QByteArray xorTransform(const QByteArray& data, const QByteArray& keyBytes) {
    QByteArray result = data;
    for (int i = 0; i < result.size(); ++i) {
        result[i] = result[i] ^ keyBytes.at(i % keyBytes.size());
    }
    return result;
}

QString XorCipher::encryptText(const QString& text, const QString& key, QString* error) const {
    QByteArray keyBytes;
    if (!getKeyBytes(key, &keyBytes, error)) {
        return {};
    }

    const QByteArray input = text.toUtf8();
    const QByteArray encrypted = xorTransform(input, keyBytes);
    return QString::fromLatin1(encrypted.toBase64());
}

QString XorCipher::decryptText(const QString& text, const QString& key, QString* error) const {
    QByteArray keyBytes;
    if (!getKeyBytes(key, &keyBytes, error)) {
        return {};
    }

    const QByteArray decoded = QByteArray::fromBase64(text.toLatin1());
    if (decoded.isEmpty() && !text.trimmed().isEmpty()) {
        if (error) {
            *error = "Input is not valid Base64.";
        }
        return {};
    }

    const QByteArray decrypted = xorTransform(decoded, keyBytes);
    return QString::fromUtf8(decrypted);
}

QByteArray XorCipher::encryptBytes(const QByteArray& data, const QString& key, QString* error) const {
    QByteArray keyBytes;
    if (!getKeyBytes(key, &keyBytes, error)) {
        return {};
    }
    return xorTransform(data, keyBytes);
}

QByteArray XorCipher::decryptBytes(const QByteArray& data, const QString& key, QString* error) const {
    return encryptBytes(data, key, error);
}

} // namespace sds
