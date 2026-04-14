#ifndef SDS_HUFFMAN_CODEC_H
#define SDS_HUFFMAN_CODEC_H

#include "crypto/cipher_base.h"

namespace sds {

class HuffmanCodec final : public CipherBase {
public:
    QString id() const override { return "huffman"; }
    QString name() const override { return "Huffman"; }
    QString description() const override { return "Lossless compression using Huffman coding."; }
    bool requiresKey() const override { return false; }

    QString encryptText(const QString& text, const QString& key, QString* error) const override;
    QString decryptText(const QString& text, const QString& key, QString* error) const override;
};

} // namespace sds

#endif // SDS_HUFFMAN_CODEC_H
