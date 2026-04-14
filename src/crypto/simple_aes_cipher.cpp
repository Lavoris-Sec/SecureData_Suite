#include "crypto/simple_aes_cipher.h"
#include <QByteArray>
#include <array>

namespace sds {

namespace {

constexpr int kBlockSize = 16;
constexpr int kRounds = 10;

static const uint8_t sbox[256] = {
    0x63,0x7c,0x77,0x7b,0xf2,0x6b,0x6f,0xc5,0x30,0x01,0x67,0x2b,0xfe,0xd7,0xab,0x76,
    0xca,0x82,0xc9,0x7d,0xfa,0x59,0x47,0xf0,0xad,0xd4,0xa2,0xaf,0x9c,0xa4,0x72,0xc0,
    0xb7,0xfd,0x93,0x26,0x36,0x3f,0xf7,0xcc,0x34,0xa5,0xe5,0xf1,0x71,0xd8,0x31,0x15,
    0x04,0xc7,0x23,0xc3,0x18,0x96,0x05,0x9a,0x07,0x12,0x80,0xe2,0xeb,0x27,0xb2,0x75,
    0x09,0x83,0x2c,0x1a,0x1b,0x6e,0x5a,0xa0,0x52,0x3b,0xd6,0xb3,0x29,0xe3,0x2f,0x84,
    0x53,0xd1,0x00,0xed,0x20,0xfc,0xb1,0x5b,0x6a,0xcb,0xbe,0x39,0x4a,0x4c,0x58,0xcf,
    0xd0,0xef,0xaa,0xfb,0x43,0x4d,0x33,0x85,0x45,0xf9,0x02,0x7f,0x50,0x3c,0x9f,0xa8,
    0x51,0xa3,0x40,0x8f,0x92,0x9d,0x38,0xf5,0xbc,0xb6,0xda,0x21,0x10,0xff,0xf3,0xd2,
    0xcd,0x0c,0x13,0xec,0x5f,0x97,0x44,0x17,0xc4,0xa7,0x7e,0x3d,0x64,0x5d,0x19,0x73,
    0x60,0x81,0x4f,0xdc,0x22,0x2a,0x90,0x88,0x46,0xee,0xb8,0x14,0xde,0x5e,0x0b,0xdb,
    0xe0,0x32,0x3a,0x0a,0x49,0x06,0x24,0x5c,0xc2,0xd3,0xac,0x62,0x91,0x95,0xe4,0x79,
    0xe7,0xc8,0x37,0x6d,0x8d,0xd5,0x4e,0xa9,0x6c,0x56,0xf4,0xea,0x65,0x7a,0xae,0x08,
    0xba,0x78,0x25,0x2e,0x1c,0xa6,0xb4,0xc6,0xe8,0xdd,0x74,0x1f,0x4b,0xbd,0x8b,0x8a,
    0x70,0x3e,0xb5,0x66,0x48,0x03,0xf6,0x0e,0x61,0x35,0x57,0xb9,0x86,0xc1,0x1d,0x9e,
    0xe1,0xf8,0x98,0x11,0x69,0xd9,0x8e,0x94,0x9b,0x1e,0x87,0xe9,0xce,0x55,0x28,0xdf,
    0x8c,0xa1,0x89,0x0d,0xbf,0xe6,0x42,0x68,0x41,0x99,0x2d,0x0f,0xb0,0x54,0xbb,0x16
};

static const uint8_t rsbox[256] = {
    0x52,0x09,0x6a,0xd5,0x30,0x36,0xa5,0x38,0xbf,0x40,0xa3,0x9e,0x81,0xf3,0xd7,0xfb,
    0x7c,0xe3,0x39,0x82,0x9b,0x2f,0xff,0x87,0x34,0x8e,0x43,0x44,0xc4,0xde,0xe9,0xcb,
    0x54,0x7b,0x94,0x32,0xa6,0xc2,0x23,0x3d,0xee,0x4c,0x95,0x0b,0x42,0xfa,0xc3,0x4e,
    0x08,0x2e,0xa1,0x66,0x28,0xd9,0x24,0xb2,0x76,0x5b,0xa2,0x49,0x6d,0x8b,0xd1,0x25,
    0x72,0xf8,0xf6,0x64,0x86,0x68,0x98,0x16,0xd4,0xa4,0x5c,0xcc,0x5d,0x65,0xb6,0x92,
    0x6c,0x70,0x48,0x50,0xfd,0xed,0xb9,0xda,0x5e,0x15,0x46,0x57,0xa7,0x8d,0x9d,0x84,
    0x90,0xd8,0xab,0x00,0x8c,0xbc,0xd3,0x0a,0xf7,0xe4,0x58,0x05,0xb8,0xb3,0x45,0x06,
    0xd0,0x2c,0x1e,0x8f,0xca,0x3f,0x0f,0x02,0xc1,0xaf,0xbd,0x03,0x01,0x13,0x8a,0x6b,
    0x3a,0x91,0x11,0x41,0x4f,0x67,0xdc,0xea,0x97,0xf2,0xcf,0xce,0xf0,0xb4,0xe6,0x73,
    0x96,0xac,0x74,0x22,0xe7,0xad,0x35,0x85,0xe2,0xf9,0x37,0xe8,0x1c,0x75,0xdf,0x6e,
    0x47,0xf1,0x1a,0x71,0x1d,0x29,0xc5,0x89,0x6f,0xb7,0x62,0x0e,0xaa,0x18,0xbe,0x1b,
    0xfc,0x56,0x3e,0x4b,0xc6,0xd2,0x79,0x20,0x9a,0xdb,0xc0,0xfe,0x78,0xcd,0x5a,0xf4,
    0x1f,0xdd,0xa8,0x33,0x88,0x07,0xc7,0x31,0xb1,0x12,0x10,0x59,0x27,0x80,0xec,0x5f,
    0x60,0x51,0x7f,0xa9,0x19,0xb5,0x4a,0x0d,0x2d,0xe5,0x7a,0x9f,0x93,0xc9,0x9c,0xef,
    0xa0,0xe0,0x3b,0x4d,0xae,0x2a,0xf5,0xb0,0xc8,0xeb,0xbb,0x3c,0x83,0x53,0x99,0x61,
    0x17,0x2b,0x04,0x7e,0xba,0x77,0xd6,0x26,0xe1,0x69,0x14,0x63,0x55,0x21,0x0c,0x7d
};

static const uint8_t Rcon[11] = {
    0x00,0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x1B,0x36
};

static uint8_t gmul(uint8_t a, uint8_t b) {
    uint8_t p = 0;
    for (int i = 0; i < 8; ++i) {
        if (b & 1) {
            p ^= a;
        }
        const bool hi = a & 0x80;
        a <<= 1;
        if (hi) {
            a ^= 0x1b;
        }
        b >>= 1;
    }
    return p;
}

static void keyExpansion(const uint8_t* key, uint8_t* roundKeys) {
    std::array<uint8_t, 4> temp{};
    for (int i = 0; i < kBlockSize; ++i) {
        roundKeys[i] = key[i];
    }

    int bytesGenerated = kBlockSize;
    int rconIter = 1;
    while (bytesGenerated < (kRounds + 1) * kBlockSize) {
        for (int i = 0; i < 4; ++i) {
            temp[i] = roundKeys[bytesGenerated - 4 + i];
        }

        if (bytesGenerated % kBlockSize == 0) {
            uint8_t t = temp[0];
            temp[0] = temp[1];
            temp[1] = temp[2];
            temp[2] = temp[3];
            temp[3] = t;

            for (int i = 0; i < 4; ++i) {
                temp[i] = sbox[temp[i]];
            }

            temp[0] ^= Rcon[rconIter++];
        }

        for (int i = 0; i < 4; ++i) {
            roundKeys[bytesGenerated] = roundKeys[bytesGenerated - kBlockSize] ^ temp[i];
            bytesGenerated++;
        }
    }
}

static void addRoundKey(uint8_t* state, const uint8_t* roundKey) {
    for (int i = 0; i < kBlockSize; ++i) {
        state[i] ^= roundKey[i];
    }
}

static void subBytes(uint8_t* state) {
    for (int i = 0; i < kBlockSize; ++i) {
        state[i] = sbox[state[i]];
    }
}

static void invSubBytes(uint8_t* state) {
    for (int i = 0; i < kBlockSize; ++i) {
        state[i] = rsbox[state[i]];
    }
}

static void shiftRows(uint8_t* state) {
    uint8_t temp;

    temp = state[1];
    state[1] = state[5];
    state[5] = state[9];
    state[9] = state[13];
    state[13] = temp;

    temp = state[2];
    state[2] = state[10];
    state[10] = temp;
    temp = state[6];
    state[6] = state[14];
    state[14] = temp;

    temp = state[3];
    state[3] = state[15];
    state[15] = state[11];
    state[11] = state[7];
    state[7] = temp;
}

static void invShiftRows(uint8_t* state) {
    uint8_t temp;

    temp = state[13];
    state[13] = state[9];
    state[9] = state[5];
    state[5] = state[1];
    state[1] = temp;

    temp = state[2];
    state[2] = state[10];
    state[10] = temp;
    temp = state[6];
    state[6] = state[14];
    state[14] = temp;

    temp = state[3];
    state[3] = state[7];
    state[7] = state[11];
    state[11] = state[15];
    state[15] = temp;
}

static void mixColumns(uint8_t* state) {
    for (int i = 0; i < 4; ++i) {
        uint8_t* col = state + i * 4;
        const uint8_t a0 = col[0];
        const uint8_t a1 = col[1];
        const uint8_t a2 = col[2];
        const uint8_t a3 = col[3];
        col[0] = gmul(a0, 2) ^ gmul(a1, 3) ^ a2 ^ a3;
        col[1] = a0 ^ gmul(a1, 2) ^ gmul(a2, 3) ^ a3;
        col[2] = a0 ^ a1 ^ gmul(a2, 2) ^ gmul(a3, 3);
        col[3] = gmul(a0, 3) ^ a1 ^ a2 ^ gmul(a3, 2);
    }
}

static void invMixColumns(uint8_t* state) {
    for (int i = 0; i < 4; ++i) {
        uint8_t* col = state + i * 4;
        const uint8_t a0 = col[0];
        const uint8_t a1 = col[1];
        const uint8_t a2 = col[2];
        const uint8_t a3 = col[3];
        col[0] = gmul(a0, 0x0e) ^ gmul(a1, 0x0b) ^ gmul(a2, 0x0d) ^ gmul(a3, 0x09);
        col[1] = gmul(a0, 0x09) ^ gmul(a1, 0x0e) ^ gmul(a2, 0x0b) ^ gmul(a3, 0x0d);
        col[2] = gmul(a0, 0x0d) ^ gmul(a1, 0x09) ^ gmul(a2, 0x0e) ^ gmul(a3, 0x0b);
        col[3] = gmul(a0, 0x0b) ^ gmul(a1, 0x0d) ^ gmul(a2, 0x09) ^ gmul(a3, 0x0e);
    }
}

static void encryptBlock(uint8_t* state, const uint8_t* roundKeys) {
    addRoundKey(state, roundKeys);

    for (int round = 1; round < kRounds; ++round) {
        subBytes(state);
        shiftRows(state);
        mixColumns(state);
        addRoundKey(state, roundKeys + round * kBlockSize);
    }

    subBytes(state);
    shiftRows(state);
    addRoundKey(state, roundKeys + kRounds * kBlockSize);
}

static void decryptBlock(uint8_t* state, const uint8_t* roundKeys) {
    addRoundKey(state, roundKeys + kRounds * kBlockSize);

    for (int round = kRounds - 1; round >= 1; --round) {
        invShiftRows(state);
        invSubBytes(state);
        addRoundKey(state, roundKeys + round * kBlockSize);
        invMixColumns(state);
    }

    invShiftRows(state);
    invSubBytes(state);
    addRoundKey(state, roundKeys);
}

static QByteArray padPkcs7(const QByteArray& data) {
    const int pad = kBlockSize - (data.size() % kBlockSize);
    QByteArray padded = data;
    padded.append(QByteArray(pad, static_cast<char>(pad)));
    return padded;
}

static bool unpadPkcs7(QByteArray* data, QString* error) {
    if (data->isEmpty()) {
        if (error) {
            *error = "Invalid padding.";
        }
        return false;
    }

    const uint8_t pad = static_cast<uint8_t>(data->back());
    if (pad == 0 || pad > kBlockSize || pad > data->size()) {
        if (error) {
            *error = "Invalid padding.";
        }
        return false;
    }

    for (int i = 0; i < pad; ++i) {
        if (static_cast<uint8_t>(data->at(data->size() - 1 - i)) != pad) {
            if (error) {
                *error = "Invalid padding.";
            }
            return false;
        }
    }

    data->chop(pad);
    return true;
}

static bool deriveKey(const QString& key, std::array<uint8_t, kBlockSize>* out, QString* error) {
    const QByteArray bytes = key.toUtf8();
    if (bytes.isEmpty()) {
        if (error) {
            *error = "Key must not be empty.";
        }
        return false;
    }

    out->fill(0);
    for (int i = 0; i < kBlockSize && i < bytes.size(); ++i) {
        (*out)[i] = static_cast<uint8_t>(bytes.at(i));
    }
    return true;
}

} // namespace

QString SimpleAesCipher::encryptText(const QString& text, const QString& key, QString* error) const {
    const QByteArray encrypted = encryptBytes(text.toUtf8(), key, error);
    if (encrypted.isEmpty() && error && !error->isEmpty()) {
        return {};
    }
    return QString::fromLatin1(encrypted.toBase64());
}

QString SimpleAesCipher::decryptText(const QString& text, const QString& key, QString* error) const {
    const QByteArray decoded = QByteArray::fromBase64(text.toLatin1());
    if (decoded.isEmpty() && !text.trimmed().isEmpty()) {
        if (error) {
            *error = "Input is not valid Base64.";
        }
        return {};
    }

    const QByteArray decrypted = decryptBytes(decoded, key, error);
    if (decrypted.isEmpty() && error && !error->isEmpty()) {
        return {};
    }
    return QString::fromUtf8(decrypted);
}

QByteArray SimpleAesCipher::encryptBytes(const QByteArray& data, const QString& key, QString* error) const {
    std::array<uint8_t, kBlockSize> keyBytes{};
    if (!deriveKey(key, &keyBytes, error)) {
        return {};
    }

    uint8_t roundKeys[(kRounds + 1) * kBlockSize] = {0};
    keyExpansion(keyBytes.data(), roundKeys);

    QByteArray padded = padPkcs7(data);
    QByteArray output;
    output.resize(padded.size());

    for (int i = 0; i < padded.size(); i += kBlockSize) {
        uint8_t block[kBlockSize];
        for (int j = 0; j < kBlockSize; ++j) {
            block[j] = static_cast<uint8_t>(padded.at(i + j));
        }
        encryptBlock(block, roundKeys);
        for (int j = 0; j < kBlockSize; ++j) {
            output[i + j] = static_cast<char>(block[j]);
        }
    }

    return output;
}

QByteArray SimpleAesCipher::decryptBytes(const QByteArray& data, const QString& key, QString* error) const {
    if (data.size() % kBlockSize != 0) {
        if (error) {
            *error = "Ciphertext size must be a multiple of 16 bytes.";
        }
        return {};
    }

    std::array<uint8_t, kBlockSize> keyBytes{};
    if (!deriveKey(key, &keyBytes, error)) {
        return {};
    }

    uint8_t roundKeys[(kRounds + 1) * kBlockSize] = {0};
    keyExpansion(keyBytes.data(), roundKeys);

    QByteArray output;
    output.resize(data.size());

    for (int i = 0; i < data.size(); i += kBlockSize) {
        uint8_t block[kBlockSize];
        for (int j = 0; j < kBlockSize; ++j) {
            block[j] = static_cast<uint8_t>(data.at(i + j));
        }
        decryptBlock(block, roundKeys);
        for (int j = 0; j < kBlockSize; ++j) {
            output[i + j] = static_cast<char>(block[j]);
        }
    }

    if (!unpadPkcs7(&output, error)) {
        return {};
    }

    return output;
}

} // namespace sds
