#include "crypto/cipher_base.h"

namespace sds {

QByteArray CipherBase::encryptBytes(const QByteArray& data, const QString& key, QString* error) const {
    Q_UNUSED(data)
    Q_UNUSED(key)
    if (error) {
        *error = "Binary mode is not supported by this algorithm.";
    }
    return {};
}

QByteArray CipherBase::decryptBytes(const QByteArray& data, const QString& key, QString* error) const {
    Q_UNUSED(data)
    Q_UNUSED(key)
    if (error) {
        *error = "Binary mode is not supported by this algorithm.";
    }
    return {};
}

} // namespace sds
