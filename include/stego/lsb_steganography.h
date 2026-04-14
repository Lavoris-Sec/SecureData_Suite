#ifndef SDS_LSB_STEGANOGRAPHY_H
#define SDS_LSB_STEGANOGRAPHY_H

#include <QString>

namespace sds {

class LsbSteganography {
public:
    static bool embedText(const QString& inputPath, const QString& outputPath, const QString& text, QString* error);
    static QString extractText(const QString& inputPath, QString* error);
};

} // namespace sds

#endif // SDS_LSB_STEGANOGRAPHY_H
