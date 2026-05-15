#include "crypto/rsa_cipher.h"

#include <QByteArray>
#include <QRandomGenerator>
#include <QStringList>

#include <algorithm>
#include <cstdint>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <bcrypt.h>
#pragma comment(lib, "bcrypt.lib")
#endif

#if __has_include(<boost/multiprecision/cpp_int.hpp>)
#define SDS_RSA_BIGINT 1
#include <boost/multiprecision/cpp_int.hpp>
#else
#define SDS_RSA_BIGINT 0
#endif

namespace sds {

namespace {

static bool parseKeyPairParts(const QString& key, QString* a, QString* n, QString* error) {
    const QStringList parts = key.split(',', Qt::SkipEmptyParts);
    if (parts.size() != 2) {
        if (error) {
            *error = "Key must be in the format 'a,n'.";
        }
        return false;
    }
    *a = parts.at(0).trimmed();
    *n = parts.at(1).trimmed();
    if (a->isEmpty() || n->isEmpty()) {
        if (error) {
            *error = "Key contains empty values.";
        }
        return false;
    }
    return true;
}

#ifdef _WIN32
static const QString kCngPubPrefix = "CNGPUB:";
static const QString kCngPrivPrefix = "CNGPRIV:";

static bool ntSuccess(NTSTATUS st) {
    return st >= 0;
}

static bool importCngKey(const QString& key, bool requirePrivate, BCRYPT_ALG_HANDLE* alg, BCRYPT_KEY_HANDLE* hKey, QString* error) {
    *alg = nullptr;
    *hKey = nullptr;
    const bool isPub = key.startsWith(kCngPubPrefix);
    const bool isPriv = key.startsWith(kCngPrivPrefix);
    if (!isPub && !isPriv) {
        if (error) *error = "Key format is not CNG.";
        return false;
    }
    if (requirePrivate && !isPriv) {
        if (error) *error = "Private key required.";
        return false;
    }

    QByteArray blob = QByteArray::fromBase64(key.mid((isPub ? kCngPubPrefix : kCngPrivPrefix).size()).toLatin1());
    if (blob.isEmpty()) {
        if (error) *error = "Invalid CNG key blob.";
        return false;
    }

    NTSTATUS st = BCryptOpenAlgorithmProvider(alg, BCRYPT_RSA_ALGORITHM, nullptr, 0);
    if (!ntSuccess(st)) {
        if (error) *error = "Failed to open RSA provider.";
        return false;
    }

    st = BCryptImportKeyPair(*alg, nullptr,
                             isPriv ? BCRYPT_RSAFULLPRIVATE_BLOB : BCRYPT_RSAPUBLIC_BLOB,
                             hKey,
                             reinterpret_cast<PUCHAR>(blob.data()),
                             static_cast<ULONG>(blob.size()),
                             0);
    if (!ntSuccess(st)) {
        if (error) *error = "Failed to import CNG key.";
        BCryptCloseAlgorithmProvider(*alg, 0);
        *alg = nullptr;
        *hKey = nullptr;
        return false;
    }
    return true;
}
#endif

#if SDS_RSA_BIGINT
using boost::multiprecision::cpp_int;

static cpp_int gcdBig(cpp_int a, cpp_int b) {
    while (b != 0) {
        cpp_int t = b;
        b = a % b;
        a = t;
    }
    return a;
}

static cpp_int modPowBig(cpp_int base, cpp_int exp, const cpp_int& mod) {
    cpp_int result = 1 % mod;
    base %= mod;
    while (exp > 0) {
        if ((exp & 1) != 0) {
            result = (result * base) % mod;
        }
        exp >>= 1;
        base = (base * base) % mod;
    }
    return result;
}

static cpp_int modInverseBig(const cpp_int& a, const cpp_int& m) {
    cpp_int t = 0;
    cpp_int newT = 1;
    cpp_int r = m;
    cpp_int newR = a % m;

    while (newR != 0) {
        const cpp_int q = r / newR;
        const cpp_int nextT = t - q * newT;
        t = newT;
        newT = nextT;
        const cpp_int nextR = r - q * newR;
        r = newR;
        newR = nextR;
    }
    if (r != 1) {
        return 0;
    }
    if (t < 0) {
        t += m;
    }
    return t;
}

static int bitLengthBig(cpp_int value) {
    int bits = 0;
    while (value > 0) {
        value >>= 1;
        ++bits;
    }
    return bits;
}

static QByteArray toFixedBytesBig(cpp_int value, int size) {
    QByteArray out(size, 0);
    for (int i = size - 1; i >= 0; --i) {
        out[i] = static_cast<char>(static_cast<unsigned>(value & 0xFF));
        value >>= 8;
    }
    return out;
}

static cpp_int fromBytesBig(const QByteArray& data) {
    cpp_int value = 0;
    for (unsigned char b : data) {
        value <<= 8;
        value += b;
    }
    return value;
}

static cpp_int randomBits(int bits) {
    cpp_int out = 0;
    int remaining = bits;
    while (remaining > 0) {
        const quint64 chunk = QRandomGenerator::global()->generate64();
        const int take = std::min(remaining, 64);
        out <<= take;
        if (take == 64) {
            out += static_cast<cpp_int>(chunk);
        } else {
            const quint64 mask = (1ULL << take) - 1ULL;
            out += static_cast<cpp_int>(chunk & mask);
        }
        remaining -= take;
    }
    return out;
}

static cpp_int randomInRange(const cpp_int& low, const cpp_int& high) {
    const cpp_int span = high - low + 1;
    const int bits = bitLengthBig(span);
    while (true) {
        cpp_int x = randomBits(bits);
        if (x < span) {
            return low + x;
        }
    }
}

static bool isProbablePrimeBig(const cpp_int& n, int rounds) {
    if (n < 4) {
        return n == 2 || n == 3;
    }
    if ((n & 1) == 0) {
        return false;
    }

    cpp_int d = n - 1;
    int r = 0;
    while ((d & 1) == 0) {
        d >>= 1;
        ++r;
    }

    for (int i = 0; i < rounds; ++i) {
        const cpp_int a = randomInRange(2, n - 2);
        cpp_int x = modPowBig(a, d, n);
        if (x == 1 || x == n - 1) {
            continue;
        }
        bool witness = true;
        for (int j = 1; j < r; ++j) {
            x = modPowBig(x, 2, n);
            if (x == n - 1) {
                witness = false;
                break;
            }
        }
        if (witness) {
            return false;
        }
    }
    return true;
}

static cpp_int generatePrimeBig(int bits) {
    if (bits < 32) {
        bits = 32;
    }
    while (true) {
        cpp_int candidate = randomBits(bits);
        candidate |= (cpp_int(1) << (bits - 1));
        candidate |= 1;
        if (isProbablePrimeBig(candidate, 10)) {
            return candidate;
        }
    }
}

static bool parseBigInt(const QString& text, cpp_int* out) {
    try {
        *out = cpp_int(text.toStdString());
        return true;
    } catch (...) {
        return false;
    }
}
#endif

static bool parseUInt64(const QString& text, uint64_t* value) {
    bool ok = false;
    *value = text.trimmed().toULongLong(&ok);
    return ok;
}

static uint64_t gcd64(uint64_t a, uint64_t b) {
    while (b != 0) {
        const uint64_t t = b;
        b = a % b;
        a = t;
    }
    return a;
}

static uint64_t mulmod64(uint64_t a, uint64_t b, uint64_t mod) {
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

static uint64_t modPow64(uint64_t base, uint64_t exp, uint64_t mod) {
    uint64_t result = 1;
    base %= mod;
    while (exp > 0) {
        if (exp & 1) {
            result = mulmod64(result, base, mod);
        }
        exp >>= 1;
        base = mulmod64(base, base, mod);
    }
    return result;
}

static bool isProbablePrime64(uint64_t n) {
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
        ++r;
    }

    const uint64_t bases[] = {2, 3, 5, 7, 11, 13, 17};
    for (uint64_t a : bases) {
        if (a >= n) {
            continue;
        }
        uint64_t x = modPow64(a, d, n);
        if (x == 1 || x == n - 1) {
            continue;
        }
        bool cont = false;
        for (int i = 0; i < r - 1; ++i) {
            x = modPow64(x, 2, n);
            if (x == n - 1) {
                cont = true;
                break;
            }
        }
        if (!cont) {
            return false;
        }
    }
    return true;
}

static uint64_t generatePrime64(int bits) {
    if (bits < 16) {
        bits = 16;
    }
    const uint64_t minValue = 1ULL << (bits - 1);
    uint64_t candidate = minValue | (static_cast<uint64_t>(QRandomGenerator::global()->generate64()) & (minValue - 1));
    candidate |= 1ULL;

    while (!isProbablePrime64(candidate)) {
        candidate += 2;
        if (candidate < minValue) {
            candidate |= minValue;
        }
    }
    return candidate;
}

static uint64_t modInverse64(uint64_t a, uint64_t m) {
    int64_t t = 0;
    int64_t newT = 1;
    int64_t r = static_cast<int64_t>(m);
    int64_t newR = static_cast<int64_t>(a % m);

    while (newR != 0) {
        const int64_t q = r / newR;
        const int64_t nextT = t - q * newT;
        t = newT;
        newT = nextT;
        const int64_t nextR = r - q * newR;
        r = newR;
        newR = nextR;
    }
    if (r != 1) {
        return 0;
    }
    if (t < 0) {
        t += static_cast<int64_t>(m);
    }
    return static_cast<uint64_t>(t);
}

static int bitLength64(uint64_t value) {
    int bits = 0;
    while (value > 0) {
        value >>= 1;
        ++bits;
    }
    return bits;
}

static QByteArray toFixedBytes64(uint64_t value, int size) {
    QByteArray out(size, 0);
    for (int i = size - 1; i >= 0; --i) {
        out[i] = static_cast<char>(value & 0xFF);
        value >>= 8;
    }
    return out;
}

static uint64_t fromBytes64(const QByteArray& data) {
    uint64_t value = 0;
    for (unsigned char b : data) {
        value = (value << 8) | b;
    }
    return value;
}

} // namespace

bool RsaCipher::supportsStrongRsa() {
#ifdef _WIN32
    return true;
#else
    return SDS_RSA_BIGINT == 1;
#endif
}

int RsaCipher::maxSupportedBits() {
    return supportsStrongRsa() ? 4096 : 62;
}

QString RsaCipher::encryptText(const QString& text, const QString& key, QString* error) const {
#ifdef _WIN32
    BCRYPT_ALG_HANDLE alg = nullptr;
    BCRYPT_KEY_HANDLE hKey = nullptr;
    if (importCngKey(key, false, &alg, &hKey, error)) {
        DWORD bits = 0;
        ULONG cbRes = 0;
        NTSTATUS st = BCryptGetProperty(hKey, BCRYPT_KEY_LENGTH, reinterpret_cast<PUCHAR>(&bits), sizeof(bits), &cbRes, 0);
        if (!ntSuccess(st) || bits < 512) {
            if (error) *error = "Invalid RSA key length.";
            BCryptDestroyKey(hKey);
            BCryptCloseAlgorithmProvider(alg, 0);
            return {};
        }
        const int keyBytes = static_cast<int>(bits / 8);
        const int maxPlain = keyBytes - 11;
        QByteArray input = text.toUtf8();
        QByteArray output;
        for (int off = 0; off < input.size(); off += maxPlain) {
            QByteArray chunk = input.mid(off, (std::min)(maxPlain, static_cast<int>(input.size() - off)));
            ULONG outLen = 0;
            st = BCryptEncrypt(hKey,
                               reinterpret_cast<PUCHAR>(chunk.data()),
                               static_cast<ULONG>(chunk.size()),
                               nullptr, nullptr, 0,
                               nullptr, 0, &outLen,
                               BCRYPT_PAD_PKCS1);
            if (!ntSuccess(st) || outLen == 0) {
                if (error) *error = "RSA encryption failed.";
                BCryptDestroyKey(hKey);
                BCryptCloseAlgorithmProvider(alg, 0);
                return {};
            }
            QByteArray block(static_cast<int>(outLen), 0);
            st = BCryptEncrypt(hKey,
                               reinterpret_cast<PUCHAR>(chunk.data()),
                               static_cast<ULONG>(chunk.size()),
                               nullptr, nullptr, 0,
                               reinterpret_cast<PUCHAR>(block.data()),
                               outLen, &outLen,
                               BCRYPT_PAD_PKCS1);
            if (!ntSuccess(st)) {
                if (error) *error = "RSA encryption failed.";
                BCryptDestroyKey(hKey);
                BCryptCloseAlgorithmProvider(alg, 0);
                return {};
            }
            block.resize(static_cast<int>(outLen));
            output.append(block);
        }
        BCryptDestroyKey(hKey);
        BCryptCloseAlgorithmProvider(alg, 0);
        return QString::fromLatin1(output.toBase64());
    }
#endif

    QString aText;
    QString nText;
    if (!parseKeyPairParts(key, &aText, &nText, error)) {
        return {};
    }

#if SDS_RSA_BIGINT
    cpp_int e;
    cpp_int n;
    if (!parseBigInt(aText, &e) || !parseBigInt(nText, &n)) {
        if (error) {
            *error = "Key contains invalid numbers.";
        }
        return {};
    }
    if (e <= 0 || n <= 0) {
        if (error) {
            *error = "Key values must be positive.";
        }
        return {};
    }

    const int nBits = bitLengthBig(n);
    if (nBits < 40) {
        if (error) {
            *error = "RSA modulus is too small. Use at least 40 bits.";
        }
        return {};
    }

    const int modulusBytes = (nBits + 7) / 8;
    const int blockSize = modulusBytes - 2;
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
        const int chunkSize = (std::min)(blockSize, static_cast<int>(input.size() - offset));
        QByteArray chunk = input.mid(offset, chunkSize);
        QByteArray padded;
        padded.reserve(modulusBytes - 1);
        padded.append(static_cast<char>(chunkSize));
        padded.append(chunk);
        while (padded.size() < modulusBytes - 1) {
            padded.append(static_cast<char>(QRandomGenerator::global()->bounded(1, 255)));
        }
        const cpp_int m = fromBytesBig(padded);
        if (m >= n) {
            if (error) {
                *error = "Block is larger than modulus.";
            }
            return {};
        }
        const cpp_int c = modPowBig(m, e, n);
        output.append(toFixedBytesBig(c, modulusBytes));
    }
    return QString::fromLatin1(output.toBase64());
#else
    uint64_t e = 0;
    uint64_t n = 0;
    if (!parseUInt64(aText, &e) || !parseUInt64(nText, &n)) {
        if (error) {
            *error = "Key contains invalid numbers.";
        }
        return {};
    }
    if (e == 0 || n == 0) {
        if (error) {
            *error = "Key values must be positive.";
        }
        return {};
    }

    const int nBits = bitLength64(n);
    if (nBits < 40) {
        if (error) {
            *error = "RSA modulus is too small. Use at least 40 bits.";
        }
        return {};
    }

    const int modulusBytes = (nBits + 7) / 8;
    const int blockSize = modulusBytes - 2;
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
        const int chunkSize = std::min(blockSize, static_cast<int>(input.size() - offset));
        QByteArray chunk = input.mid(offset, chunkSize);
        QByteArray padded;
        padded.reserve(modulusBytes - 1);
        padded.append(static_cast<char>(chunkSize));
        padded.append(chunk);
        while (padded.size() < modulusBytes - 1) {
            padded.append(static_cast<char>(QRandomGenerator::global()->bounded(1, 255)));
        }
        const uint64_t m = fromBytes64(padded);
        if (m >= n) {
            if (error) {
                *error = "Block is larger than modulus.";
            }
            return {};
        }
        const uint64_t c = modPow64(m, e, n);
        output.append(toFixedBytes64(c, modulusBytes));
    }
    return QString::fromLatin1(output.toBase64());
#endif
}

QString RsaCipher::decryptText(const QString& text, const QString& key, QString* error) const {
#ifdef _WIN32
    BCRYPT_ALG_HANDLE alg = nullptr;
    BCRYPT_KEY_HANDLE hKey = nullptr;
    if (importCngKey(key, true, &alg, &hKey, error)) {
        const QByteArray input = QByteArray::fromBase64(text.toLatin1());
        if (input.isEmpty() && !text.trimmed().isEmpty()) {
            if (error) *error = "Input is not valid Base64.";
            BCryptDestroyKey(hKey);
            BCryptCloseAlgorithmProvider(alg, 0);
            return {};
        }
        DWORD bits = 0;
        ULONG cbRes = 0;
        NTSTATUS st = BCryptGetProperty(hKey, BCRYPT_KEY_LENGTH, reinterpret_cast<PUCHAR>(&bits), sizeof(bits), &cbRes, 0);
        if (!ntSuccess(st) || bits < 512) {
            if (error) *error = "Invalid RSA key length.";
            BCryptDestroyKey(hKey);
            BCryptCloseAlgorithmProvider(alg, 0);
            return {};
        }
        const int keyBytes = static_cast<int>(bits / 8);
        if (input.size() % keyBytes != 0) {
            if (error) *error = "Ciphertext length is invalid for this key.";
            BCryptDestroyKey(hKey);
            BCryptCloseAlgorithmProvider(alg, 0);
            return {};
        }
        QByteArray output;
        for (int off = 0; off < input.size(); off += keyBytes) {
            QByteArray block = input.mid(off, keyBytes);
            ULONG outLen = 0;
            st = BCryptDecrypt(hKey,
                               reinterpret_cast<PUCHAR>(block.data()),
                               static_cast<ULONG>(block.size()),
                               nullptr, nullptr, 0,
                               nullptr, 0, &outLen,
                               BCRYPT_PAD_PKCS1);
            if (!ntSuccess(st) || outLen == 0) {
                if (error) *error = "RSA decryption failed.";
                BCryptDestroyKey(hKey);
                BCryptCloseAlgorithmProvider(alg, 0);
                return {};
            }
            QByteArray plain(static_cast<int>(outLen), 0);
            st = BCryptDecrypt(hKey,
                               reinterpret_cast<PUCHAR>(block.data()),
                               static_cast<ULONG>(block.size()),
                               nullptr, nullptr, 0,
                               reinterpret_cast<PUCHAR>(plain.data()),
                               outLen, &outLen,
                               BCRYPT_PAD_PKCS1);
            if (!ntSuccess(st)) {
                if (error) *error = "RSA decryption failed.";
                BCryptDestroyKey(hKey);
                BCryptCloseAlgorithmProvider(alg, 0);
                return {};
            }
            plain.resize(static_cast<int>(outLen));
            output.append(plain);
        }
        BCryptDestroyKey(hKey);
        BCryptCloseAlgorithmProvider(alg, 0);
        return QString::fromUtf8(output);
    }
#endif

    QString aText;
    QString nText;
    if (!parseKeyPairParts(key, &aText, &nText, error)) {
        return {};
    }
    const QByteArray input = QByteArray::fromBase64(text.toLatin1());
    if (input.isEmpty()) {
        if (!text.trimmed().isEmpty() && error) {
            *error = "Input is not valid Base64.";
        }
        return {};
    }

#if SDS_RSA_BIGINT
    cpp_int d;
    cpp_int n;
    if (!parseBigInt(aText, &d) || !parseBigInt(nText, &n)) {
        if (error) {
            *error = "Key contains invalid numbers.";
        }
        return {};
    }
    const int nBits = bitLengthBig(n);
    const int modulusBytes = (nBits + 7) / 8;
    const int blockSize = modulusBytes - 2;
    if (blockSize <= 0 || input.size() % modulusBytes != 0) {
        if (error) {
            *error = "Ciphertext length is invalid for this key.";
        }
        return {};
    }

    QByteArray output;
    for (int offset = 0; offset < input.size(); offset += modulusBytes) {
        const cpp_int c = fromBytesBig(input.mid(offset, modulusBytes));
        const cpp_int m = modPowBig(c, d, n);
        const QByteArray plainBlock = toFixedBytesBig(m, modulusBytes - 1);
        const int chunkLen = static_cast<unsigned char>(plainBlock.at(0));
        if (chunkLen < 0 || chunkLen > blockSize) {
            if (error) {
                *error = "Invalid RSA block header.";
            }
            return {};
        }
        output.append(plainBlock.mid(1, chunkLen));
    }
#else
    uint64_t d = 0;
    uint64_t n = 0;
    if (!parseUInt64(aText, &d) || !parseUInt64(nText, &n)) {
        if (error) {
            *error = "Key contains invalid numbers.";
        }
        return {};
    }
    const int nBits = bitLength64(n);
    const int modulusBytes = (nBits + 7) / 8;
    const int blockSize = modulusBytes - 2;
    if (blockSize <= 0 || input.size() % modulusBytes != 0) {
        if (error) {
            *error = "Ciphertext length is invalid for this key.";
        }
        return {};
    }

    QByteArray output;
    for (int offset = 0; offset < input.size(); offset += modulusBytes) {
        const uint64_t c = fromBytes64(input.mid(offset, modulusBytes));
        const uint64_t m = modPow64(c, d, n);
        const QByteArray plainBlock = toFixedBytes64(m, modulusBytes - 1);
        const int chunkLen = static_cast<unsigned char>(plainBlock.at(0));
        if (chunkLen < 0 || chunkLen > blockSize) {
            if (error) {
                *error = "Invalid RSA block header.";
            }
            return {};
        }
        output.append(plainBlock.mid(1, chunkLen));
    }
#endif

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
#ifdef _WIN32
    if (bits < 512) {
        bits = 512;
    }
    if (bits > 4096) {
        bits = 4096;
    }
    BCRYPT_ALG_HANDLE alg = nullptr;
    BCRYPT_KEY_HANDLE hKey = nullptr;
    NTSTATUS st = BCryptOpenAlgorithmProvider(&alg, BCRYPT_RSA_ALGORITHM, nullptr, 0);
    if (!ntSuccess(st)) {
        if (error) *error = "Failed to open RSA provider.";
        return {};
    }
    st = BCryptGenerateKeyPair(alg, &hKey, static_cast<ULONG>(bits), 0);
    if (!ntSuccess(st)) {
        if (error) *error = "Failed to generate RSA key pair.";
        BCryptCloseAlgorithmProvider(alg, 0);
        return {};
    }
    st = BCryptFinalizeKeyPair(hKey, 0);
    if (!ntSuccess(st)) {
        if (error) *error = "Failed to finalize RSA key pair.";
        BCryptDestroyKey(hKey);
        BCryptCloseAlgorithmProvider(alg, 0);
        return {};
    }

    ULONG pubLen = 0;
    ULONG privLen = 0;
    st = BCryptExportKey(hKey, nullptr, BCRYPT_RSAPUBLIC_BLOB, nullptr, 0, &pubLen, 0);
    if (!ntSuccess(st) || pubLen == 0) {
        if (error) *error = "Failed to export RSA public key.";
        BCryptDestroyKey(hKey);
        BCryptCloseAlgorithmProvider(alg, 0);
        return {};
    }
    st = BCryptExportKey(hKey, nullptr, BCRYPT_RSAFULLPRIVATE_BLOB, nullptr, 0, &privLen, 0);
    if (!ntSuccess(st) || privLen == 0) {
        if (error) *error = "Failed to export RSA private key.";
        BCryptDestroyKey(hKey);
        BCryptCloseAlgorithmProvider(alg, 0);
        return {};
    }

    QByteArray pubBlob(static_cast<int>(pubLen), 0);
    QByteArray privBlob(static_cast<int>(privLen), 0);
    st = BCryptExportKey(hKey, nullptr, BCRYPT_RSAPUBLIC_BLOB,
                         reinterpret_cast<PUCHAR>(pubBlob.data()), pubLen, &pubLen, 0);
    if (!ntSuccess(st)) {
        if (error) *error = "Failed to export RSA public key.";
        BCryptDestroyKey(hKey);
        BCryptCloseAlgorithmProvider(alg, 0);
        return {};
    }
    st = BCryptExportKey(hKey, nullptr, BCRYPT_RSAFULLPRIVATE_BLOB,
                         reinterpret_cast<PUCHAR>(privBlob.data()), privLen, &privLen, 0);
    if (!ntSuccess(st)) {
        if (error) *error = "Failed to export RSA private key.";
        BCryptDestroyKey(hKey);
        BCryptCloseAlgorithmProvider(alg, 0);
        return {};
    }
    pubBlob.resize(static_cast<int>(pubLen));
    privBlob.resize(static_cast<int>(privLen));

    BCryptDestroyKey(hKey);
    BCryptCloseAlgorithmProvider(alg, 0);

    RsaKeyPair result;
    result.publicKey = kCngPubPrefix + QString::fromLatin1(pubBlob.toBase64());
    result.privateKey = kCngPrivPrefix + QString::fromLatin1(privBlob.toBase64());
    return result;
#endif

    const int hardMaxBits = maxSupportedBits();
    if (bits < 40) {
        bits = 40;
    }
    if (bits > hardMaxBits) {
        bits = hardMaxBits;
    }

    const uint64_t eSmall = 65537;

#if SDS_RSA_BIGINT
    const cpp_int e = 65537;
    cpp_int p = 0;
    cpp_int q = 0;
    cpp_int phi = 0;

    for (int attempts = 0; attempts < 80; ++attempts) {
        p = generatePrimeBig(bits / 2);
        q = generatePrimeBig(bits / 2);
        if (p == q) {
            continue;
        }
        phi = (p - 1) * (q - 1);
        if (gcdBig(e, phi) == 1) {
            break;
        }
    }
    if (gcdBig(e, phi) != 1) {
        if (error) {
            *error = "Failed to generate RSA keys.";
        }
        return {};
    }

    const cpp_int n = p * q;
    const cpp_int d = modInverseBig(e, phi);
    if (d == 0) {
        if (error) {
            *error = "Failed to derive private exponent.";
        }
        return {};
    }

    RsaKeyPair legacyResultBig;
    legacyResultBig.publicKey = QString::fromStdString(e.convert_to<std::string>()) + "," +
                                QString::fromStdString(n.convert_to<std::string>());
    legacyResultBig.privateKey = QString::fromStdString(d.convert_to<std::string>()) + "," +
                                 QString::fromStdString(n.convert_to<std::string>());
    return legacyResultBig;
#else
    uint64_t p = 0;
    uint64_t q = 0;
    uint64_t phi = 0;
    for (int attempts = 0; attempts < 20; ++attempts) {
        p = generatePrime64(bits / 2);
        q = generatePrime64(bits / 2);
        if (p == q) {
            continue;
        }
        phi = (p - 1) * (q - 1);
        if (gcd64(eSmall, phi) == 1) {
            break;
        }
    }
    if (gcd64(eSmall, phi) != 1) {
        if (error) {
            *error = "Failed to generate RSA keys.";
        }
        return {};
    }
    const uint64_t n = p * q;
    const uint64_t d = modInverse64(eSmall, phi);
    if (d == 0) {
        if (error) {
            *error = "Failed to derive private exponent.";
        }
        return {};
    }
    RsaKeyPair legacyResult64;
    legacyResult64.publicKey = QString::number(eSmall) + "," + QString::number(n);
    legacyResult64.privateKey = QString::number(d) + "," + QString::number(n);
    return legacyResult64;
#endif
}

} // namespace sds
