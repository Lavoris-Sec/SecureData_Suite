#include "crypto/permutation_cipher.h"
#include <algorithm>
#include <vector>

namespace sds {

static bool normalizeKey(const QString& key, QString* out, QString* error) {
    QString cleaned = key.simplified();
    cleaned.remove(' ');
    if (cleaned.size() < 2) {
        if (error) {
            *error = "Key must contain at least 2 characters.";
        }
        return false;
    }
    *out = cleaned;
    return true;
}

static std::vector<int> buildOrder(const QString& key) {
    const int n = key.size();
    std::vector<int> indices(n);
    for (int i = 0; i < n; ++i) {
        indices[i] = i;
    }

    std::stable_sort(indices.begin(), indices.end(), [&](int a, int b) {
        return key.at(a).toLower() < key.at(b).toLower();
    });

    return indices;
}

QString PermutationCipher::encryptText(const QString& text, const QString& key, QString* error) const {
    QString normalized;
    if (!normalizeKey(key, &normalized, error)) {
        return {};
    }

    const int columns = normalized.size();
    const QString input = text;
    const int length = input.size();
    const int rows = (length + columns - 1) / columns;

    QString padded = input;
    padded.reserve(rows * columns);
    while (padded.size() < rows * columns) {
        padded.append('X');
    }

    const std::vector<int> order = buildOrder(normalized);

    QString cipher;
    cipher.reserve(rows * columns + 9);
    cipher.append(QString("%1:").arg(length, 8, 16, QLatin1Char('0')));

    for (int colIndex : order) {
        for (int row = 0; row < rows; ++row) {
            cipher.append(padded.at(row * columns + colIndex));
        }
    }

    return cipher;
}

QString PermutationCipher::decryptText(const QString& text, const QString& key, QString* error) const {
    QString normalized;
    if (!normalizeKey(key, &normalized, error)) {
        return {};
    }

    if (text.size() < 9 || text.at(8) != ':') {
        if (error) {
            *error = "Ciphertext is missing the length header.";
        }
        return {};
    }

    bool ok = false;
    const int originalLength = text.left(8).toInt(&ok, 16);
    if (!ok || originalLength < 0) {
        if (error) {
            *error = "Invalid length header.";
        }
        return {};
    }

    const QString payload = text.mid(9);
    const int columns = normalized.size();
    const int rows = (payload.size() + columns - 1) / columns;
    if (rows * columns != payload.size()) {
        if (error) {
            *error = "Ciphertext length does not match key.";
        }
        return {};
    }

    const std::vector<int> order = buildOrder(normalized);
    std::vector<int> inverse(columns);
    for (int i = 0; i < columns; ++i) {
        inverse[order[i]] = i;
    }

    QString matrix(payload.size(), QChar(' '));
    int index = 0;
    for (int ord = 0; ord < columns; ++ord) {
        const int col = order[ord];
        for (int row = 0; row < rows; ++row) {
            matrix[row * columns + col] = payload.at(index++);
        }
    }

    return matrix.left(originalLength);
}

} // namespace sds
