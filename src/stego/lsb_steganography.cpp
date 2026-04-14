#include "stego/lsb_steganography.h"
#include <QImage>

namespace sds {

static void setBit(uint8_t& value, int bit, bool on) {
    if (on) {
        value |= (1 << bit);
    } else {
        value &= ~(1 << bit);
    }
}

static bool getBit(uint8_t value, int bit) {
    return (value >> bit) & 1;
}

bool LsbSteganography::embedText(const QString& inputPath, const QString& outputPath, const QString& text, QString* error) {
    QImage image(inputPath);
    if (image.isNull()) {
        if (error) {
            *error = "Failed to open image.";
        }
        return false;
    }

    image = image.convertToFormat(QImage::Format_ARGB32);

    const QByteArray data = text.toUtf8();
    const quint32 length = static_cast<quint32>(data.size());
    const int requiredBits = 32 + length * 8;
    const int capacity = image.width() * image.height() * 3;

    if (requiredBits > capacity) {
        if (error) {
            *error = "Message is too large for this image.";
        }
        return false;
    }

    int bitIndex = 0;
    auto nextBit = [&](int idx) -> bool {
        if (idx < 32) {
            return (length >> (31 - idx)) & 1;
        }
        const int dataBit = idx - 32;
        const int byteIndex = dataBit / 8;
        const int bitInByte = 7 - (dataBit % 8);
        return (static_cast<uint8_t>(data.at(byteIndex)) >> bitInByte) & 1;
    };

    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            QRgb pixel = image.pixel(x, y);
            uint8_t r = static_cast<uint8_t>(qRed(pixel));
            uint8_t g = static_cast<uint8_t>(qGreen(pixel));
            uint8_t b = static_cast<uint8_t>(qBlue(pixel));

            if (bitIndex < requiredBits) {
                setBit(r, 0, nextBit(bitIndex++));
            }
            if (bitIndex < requiredBits) {
                setBit(g, 0, nextBit(bitIndex++));
            }
            if (bitIndex < requiredBits) {
                setBit(b, 0, nextBit(bitIndex++));
            }

            image.setPixel(x, y, qRgba(r, g, b, qAlpha(pixel)));

            if (bitIndex >= requiredBits) {
                break;
            }
        }
        if (bitIndex >= requiredBits) {
            break;
        }
    }

    if (!image.save(outputPath)) {
        if (error) {
            *error = "Failed to save output image.";
        }
        return false;
    }

    return true;
}

QString LsbSteganography::extractText(const QString& inputPath, QString* error) {
    QImage image(inputPath);
    if (image.isNull()) {
        if (error) {
            *error = "Failed to open image.";
        }
        return {};
    }

    image = image.convertToFormat(QImage::Format_ARGB32);

    quint32 length = 0;
    int bitIndex = 0;

    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            const QRgb pixel = image.pixel(x, y);
            const uint8_t r = static_cast<uint8_t>(qRed(pixel));
            const uint8_t g = static_cast<uint8_t>(qGreen(pixel));
            const uint8_t b = static_cast<uint8_t>(qBlue(pixel));

            if (bitIndex < 32) {
                length = (length << 1) | (getBit(r, 0) ? 1 : 0);
                bitIndex++;
            }
            if (bitIndex < 32) {
                length = (length << 1) | (getBit(g, 0) ? 1 : 0);
                bitIndex++;
            }
            if (bitIndex < 32) {
                length = (length << 1) | (getBit(b, 0) ? 1 : 0);
                bitIndex++;
            }

            if (bitIndex >= 32) {
                break;
            }
        }
        if (bitIndex >= 32) {
            break;
        }
    }

    const int totalBits = static_cast<int>(length) * 8;
    const int capacity = image.width() * image.height() * 3;
    if (32 + totalBits > capacity) {
        if (error) {
            *error = "Hidden message length is invalid.";
        }
        return {};
    }

    QByteArray data;
    data.resize(static_cast<int>(length));
    data.fill(0);

    int dataBitIndex = 0;
    int globalBitIndex = 0;
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            const QRgb pixel = image.pixel(x, y);
            const uint8_t r = static_cast<uint8_t>(qRed(pixel));
            const uint8_t g = static_cast<uint8_t>(qGreen(pixel));
            const uint8_t b = static_cast<uint8_t>(qBlue(pixel));

            const bool bits[3] = { getBit(r, 0), getBit(g, 0), getBit(b, 0) };
            for (int i = 0; i < 3; ++i) {
                if (globalBitIndex >= 32 && dataBitIndex < totalBits) {
                    const int byteIndex = dataBitIndex / 8;
                    const int bitInByte = 7 - (dataBitIndex % 8);
                    if (bits[i]) {
                        data[byteIndex] = static_cast<char>(static_cast<uint8_t>(data.at(byteIndex)) | (1 << bitInByte));
                    }
                    dataBitIndex++;
                }
                globalBitIndex++;
                if (dataBitIndex >= totalBits) {
                    break;
                }
            }

            if (dataBitIndex >= totalBits) {
                break;
            }
        }
        if (dataBitIndex >= totalBits) {
            break;
        }
    }

    return QString::fromUtf8(data);
}

} // namespace sds
