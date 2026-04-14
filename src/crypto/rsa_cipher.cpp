#include "crypto/rsa_cipher.h"
#include <QByteArray>
#include <QRandomGenerator>
#include <QStringList>
#include <algorithm>
#include <cstdint>

namespace sds {

namespace {

static bool parseUInt64(const QString& text, uint64_t* value) {
    bool ok = false;
    *value = text.trimmed().toULongLong(&ok);
    return ok;
}

static bool parseKeyPair(const QString& key, uint64_t* a, uint64_t* n, QString* error) {
    const QStringList parts = key.split(',', Qt::SkipEmptyParts);
    if (parts.size() != 2) {
        if (error) {
            *error = "Key must be in the format 'a,n'.";
        }
        return false;
    }

    if (!parseUInt64(parts.at(0), a) || !parseUInt64(parts.at(1), n)) {
        if (error) {
            *error = "Key contains invalid numbers.";
        }
        return false;
    }

    if (*a == 0 || *n == 0) {
        if (error) {
            *error = "Key values must be positive.";
        }
        return false;
    }

    return true;
}

static uint64_t gcd(uint64_t a, uint64_t b) {
    while (b != 0) {
        uint64_t t = b;
        b = a % b;
        a = t;
    }
    return a;
}

static uint64_t mulmod(uint64_t a, uint64_t b, uint64_t mod) {
    uint64_t result = 0;
    a %= mod;
    while (b > 0) {
        if (b & 1) {
            result = (result + a) % mod;
        }
        a = (a << 1) % mod;
        b >>= 1;
    }
    return result;
}

static uint64_t modPow(uint64_t base, uint64_t exp, uint64_t mod) {
    uint64_t result = 1;
    base %= mod;
    while (exp > 0) {
        if (exp & 1) {
            result = mulmod(result, base, mod);
        }
        exp >>= 1;
        base = mulmod(base, base, mod);
    }
    return result;
}

static bool isProbablePrime(uint64_t n) {
    if (n < 4) {
        return n == 2 || n == 3;
    }
    if ((n % 2) == 0) {
        return false;
    }

    uint64_t d = n - 1;
    int r = 0;
    while ((d & 1) == 0) {
        d >>= 1;
        r++;
    }

    const uint64_t bases[] = {2, 3, 5, 7, 11, 13, 17};
    for (uint64_t a : bases) {
        if (a >= n) {
            continue;
        }
        uint64_t x = modPow(a, d, n);
        if (x == 1 || x == n - 1) {
            continue;
        }
        bool cont = false;
        for (int i = 0; i < r - 1; ++i) {
            x = modPow(x, 2, n);
            if (x == n - 1) {
                cont = true;
                break;
            }
        }
        if (cont) {
            continue;
        }
        return false;
    }

    return true;
}

static uint64_t generatePrime(int bits) {
    if (bits < 16) {
        bits = 16;
    }
    const uint64_t minValue = 1ULL << (bits - 1);
    uint64_t candidate = minValue | (static_cast<uint64_t>(QRandomGenerator::global()->generate64()) & (minValue - 1));
    candidate |= 1ULL;

    while (!isProbablePrime(candidate)) {
        candidate += 2;
        if (candidate < minValue) {
            candidate |= minValue;
        }
    }

    return candidate;
}

static uint64_t modInverse(uint64_t a, uint64_t m) {
    int64_t m0 = static_cast<int64_t>(m);
    int64_t x0 = 0;
    int64_t x1 = 1;

    if (m == 1) {
        return 0;
    }

    int64_t aa = static_cast<int64_t>(a);
    int64_t mm = static_cast<int64_t>(m);
    while (aa > 1) {
        const int64_t q = aa / mm;
        int64_t t = mm;
        mm = aa % mm;
        aa = t;
        t = x0;
        x0 = x1 - q * x0;
        x1 = t;
    }

    if (x1 < 0) {
        x1 += m0;
    }

    return static_cast<uint64_t>(x1);
}

static int bitLength(uint64_t value) {
    int bits = 0;
    while (value > 0) {
        value >>= 1;
        bits++;
    }
    return bits;
}

static QByteArray toFixedBytes(uint64_t value, int size) {
    QByteArray out(size, 0);
    for (int i = size - 1; i >= 0; --i) {
        out[i] = static_cast<char>(value & 0xFF);
        value >>= 8;
    }
    return out;
}

static uint64_t fromBytes(const QByteArray& data) {
    uint64_t value = 0;
    for (unsigned char b : data) {
        value = (value << 8) | b;
    }
    return value;
}

} // namespace

QString RsaCipher::encryptText(const QString& text, const QString& key, QString* error) const {
    uint64_t e = 0;
    uint64_t n = 0;
    if (!parseKeyPair(key, &e, &n, error)) {
        return {};
    }

    const int nBits = bitLength(n);
    if (nBits < 32) {
        if (error) {
            *error = "RSA modulus is too small. Use at least 32 bits.";
        }
        return {};
    }

    const int modulusBytes = (nBits + 7) / 8;
    const int blockSize = modulusBytes - 1;
    if (blockSize <= 0) {
        if (error) {
            *error = "RSA modulus is too small for block encryption.";
        }
        return {};
    }

    QByteArray input = text.toUtf8();
    QByteArray lengthPrefix(4, 0);
    const quint32 len = static_cast<quint32>(input.size());
    lengthPrefix[0] = static_cast<char>((len >> 24) & 0xFF);
    lengthPrefix[1] = static_cast<char>((len >> 16) & 0xFF);
    lengthPrefix[2] = static_cast<char>((len >> 8) & 0xFF);
    lengthPrefix[3] = static_cast<char>(len & 0xFF);
    input = lengthPrefix + input;

    QByteArray output;
    for (int offset = 0; offset < input.size(); offset += blockSize) {
        const int remaining = static_cast<int>(input.size() - offset);
        const int chunkSize = std::min(blockSize, remaining);
        QByteArray chunk = input.mid(offset, chunkSize);
        uint64_t m = fromBytes(chunk);
        if (m >= n) {
            if (error) {
                *error = "Block is larger than modulus.";
            }
            return {};
        }
        uint64_t c = modPow(m, e, n);
        output.append(toFixedBytes(c, modulusBytes));
    }

    return QString::fromLatin1(output.toBase64());
}

QString RsaCipher::decryptText(const QString& text, const QString& key, QString* error) const {
    uint64_t d = 0;
    uint64_t n = 0;
    if (!parseKeyPair(key, &d, &n, error)) {
        return {};
    }

    const QByteArray input = QByteArray::fromBase64(text.toLatin1());
    if (input.isEmpty()) {
        if (!text.trimmed().isEmpty() && error) {
            *error = "Input is not valid Base64.";
        }
        return {};
    }

    const int nBits = bitLength(n);
    const int modulusBytes = (nBits + 7) / 8;
    const int blockSize = modulusBytes - 1;
    if (blockSize <= 0 || input.size() % modulusBytes != 0) {
        if (error) {
            *error = "Ciphertext length is invalid for this key.";
        }
        return {};
    }

    QByteArray output;
    for (int offset = 0; offset < input.size(); offset += modulusBytes) {
        QByteArray block = input.mid(offset, modulusBytes);
        uint64_t c = fromBytes(block);
        uint64_t m = modPow(c, d, n);
        QByteArray plainBlock = toFixedBytes(m, blockSize);
        output.append(plainBlock);
    }

    if (output.size() < 4) {
        if (error) {
            *error = "Decrypted data is too short.";
        }
        return {};
    }

    const quint32 len =
        (static_cast<unsigned char>(output[0]) << 24) |
        (static_cast<unsigned char>(output[1]) << 16) |
        (static_cast<unsigned char>(output[2]) << 8) |
        static_cast<unsigned char>(output[3]);

    if (output.size() - 4 < static_cast<int>(len)) {
        if (error) {
            *error = "Length prefix is invalid.";
        }
        return {};
    }

    return QString::fromUtf8(output.mid(4, len));
}

RsaKeyPair RsaCipher::generateKeyPair(int bits, QString* error) const {
    if (bits < 32) {
        bits = 32;
    }
    if (bits > 56) {
        bits = 56;
    }

    const uint64_t e = 65537;
    uint64_t p = 0;
    uint64_t q = 0;
    uint64_t phi = 0;

    for (int attempts = 0; attempts < 20; ++attempts) {
        p = generatePrime(bits / 2);
        q = generatePrime(bits / 2);
        if (p == q) {
            continue;
        }
        phi = (p - 1) * (q - 1);
        if (gcd(e, phi) == 1) {
            break;
        }
    }

    if (gcd(e, phi) != 1) {
        if (error) {
            *error = "Failed to generate RSA keys.";
        }
        return {};
    }

    const uint64_t n = p * q;
    const uint64_t d = modInverse(e, phi);

    RsaKeyPair result;
    result.publicKey = QString::number(e) + "," + QString::number(n);
    result.privateKey = QString::number(d) + "," + QString::number(n);
    return result;
}

} // namespace sds
