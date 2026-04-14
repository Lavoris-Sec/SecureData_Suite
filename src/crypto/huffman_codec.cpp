#include "crypto/huffman_codec.h"
#include <QByteArray>
#include <QDataStream>
#include <QIODevice>
#include <array>
#include <cstdint>
#include <queue>
#include <vector>

namespace sds {

namespace {

struct Node {
    int freq = 0;
    int symbol = -1;
    Node* left = nullptr;
    Node* right = nullptr;

    Node(int f, int s) : freq(f), symbol(s) {}
    Node(Node* l, Node* r) : freq(l->freq + r->freq), symbol(-1), left(l), right(r) {}
};

struct NodeCompare {
    bool operator()(const Node* a, const Node* b) const {
        return a->freq > b->freq;
    }
};

static void buildCodes(Node* node, const QByteArray& prefix, std::array<QByteArray, 256>& codes) {
    if (!node) {
        return;
    }
    if (!node->left && !node->right && node->symbol >= 0) {
        codes[static_cast<size_t>(node->symbol)] = prefix.isEmpty() ? QByteArray("0") : prefix;
        return;
    }
    QByteArray leftPrefix = prefix + '0';
    QByteArray rightPrefix = prefix + '1';
    buildCodes(node->left, leftPrefix, codes);
    buildCodes(node->right, rightPrefix, codes);
}

static void freeTree(Node* node) {
    if (!node) {
        return;
    }
    freeTree(node->left);
    freeTree(node->right);
    delete node;
}

static bool writeHeader(QByteArray* output, const std::array<int, 256>& freq) {
    QDataStream stream(output, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream.writeRawData("HUF1", 4);
    for (int i = 0; i < 256; ++i) {
        stream << static_cast<quint32>(freq[static_cast<size_t>(i)]);
    }
    return !stream.status();
}

static bool readHeader(const QByteArray& input, std::array<int, 256>* freq, int* offset, QString* error) {
    if (input.size() < 4 + 256 * 4) {
        if (error) {
            *error = "Invalid Huffman data.";
        }
        return false;
    }

    QDataStream stream(input);
    stream.setByteOrder(QDataStream::LittleEndian);
    char magic[4] = {0};
    stream.readRawData(magic, 4);
    if (QByteArray(magic, 4) != "HUF1") {
        if (error) {
            *error = "Invalid Huffman header.";
        }
        return false;
    }

    for (int i = 0; i < 256; ++i) {
        quint32 value = 0;
        stream >> value;
        (*freq)[static_cast<size_t>(i)] = static_cast<int>(value);
    }

    if (stream.status() != QDataStream::Ok) {
        if (error) {
            *error = "Invalid Huffman header.";
        }
        return false;
    }

    *offset = 4 + 256 * 4;
    return true;
}

} // namespace

QString HuffmanCodec::encryptText(const QString& text, const QString& key, QString* error) const {
    Q_UNUSED(key)

    const QByteArray input = text.toUtf8();
    if (input.isEmpty()) {
        return QString();
    }

    std::array<int, 256> freq{};
    freq.fill(0);
    for (unsigned char c : input) {
        freq[static_cast<size_t>(c)]++;
    }

    std::priority_queue<Node*, std::vector<Node*>, NodeCompare> pq;
    for (int i = 0; i < 256; ++i) {
        if (freq[static_cast<size_t>(i)] > 0) {
            pq.push(new Node(freq[static_cast<size_t>(i)], i));
        }
    }

    if (pq.empty()) {
        return QString();
    }

    while (pq.size() > 1) {
        Node* a = pq.top();
        pq.pop();
        Node* b = pq.top();
        pq.pop();
        pq.push(new Node(a, b));
    }

    Node* root = pq.top();
    std::array<QByteArray, 256> codes;
    buildCodes(root, QByteArray(), codes);

    QByteArray header;
    if (!writeHeader(&header, freq)) {
        if (error) {
            *error = "Failed to build Huffman header.";
        }
        freeTree(root);
        return {};
    }

    QByteArray bitStream;
    bitStream.reserve(input.size());
    int bitCount = 0;
    uint8_t current = 0;

    for (unsigned char c : input) {
        const QByteArray& code = codes[static_cast<size_t>(c)];
        for (char bitChar : code) {
            current <<= 1;
            if (bitChar == '1') {
                current |= 1;
            }
            bitCount++;
            if (bitCount == 8) {
                bitStream.append(static_cast<char>(current));
                bitCount = 0;
                current = 0;
            }
        }
    }

    if (bitCount > 0) {
        current <<= (8 - bitCount);
        bitStream.append(static_cast<char>(current));
    }

    freeTree(root);

    QByteArray output = header + bitStream;
    return QString::fromLatin1(output.toBase64());
}

QString HuffmanCodec::decryptText(const QString& text, const QString& key, QString* error) const {
    Q_UNUSED(key)

    const QByteArray input = QByteArray::fromBase64(text.toLatin1());
    if (input.isEmpty()) {
        if (!text.trimmed().isEmpty() && error) {
            *error = "Input is not valid Base64.";
        }
        return {};
    }

    std::array<int, 256> freq{};
    freq.fill(0);
    int offset = 0;
    if (!readHeader(input, &freq, &offset, error)) {
        return {};
    }

    std::priority_queue<Node*, std::vector<Node*>, NodeCompare> pq;
    int totalSymbols = 0;
    for (int i = 0; i < 256; ++i) {
        if (freq[static_cast<size_t>(i)] > 0) {
            pq.push(new Node(freq[static_cast<size_t>(i)], i));
            totalSymbols += freq[static_cast<size_t>(i)];
        }
    }

    if (pq.empty()) {
        return {};
    }

    while (pq.size() > 1) {
        Node* a = pq.top();
        pq.pop();
        Node* b = pq.top();
        pq.pop();
        pq.push(new Node(a, b));
    }

    Node* root = pq.top();

    QByteArray output;
    output.reserve(totalSymbols);

    Node* node = root;
    for (int i = offset; i < input.size() && output.size() < totalSymbols; ++i) {
        const uint8_t byte = static_cast<uint8_t>(input.at(i));
        for (int bit = 7; bit >= 0 && output.size() < totalSymbols; --bit) {
            const bool isOne = (byte >> bit) & 1;
            node = isOne ? node->right : node->left;
            if (!node) {
                if (error) {
                    *error = "Corrupted Huffman data.";
                }
                freeTree(root);
                return {};
            }
            if (!node->left && !node->right) {
                output.append(static_cast<char>(node->symbol));
                node = root;
            }
        }
    }

    freeTree(root);
    return QString::fromUtf8(output);
}

} // namespace sds
