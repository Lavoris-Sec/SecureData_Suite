#include "crypto/caesar_cipher.h"
#include <QFile>
#include <QTextStream>
#include <QStringConverter>

namespace sds {

static bool parseShift(const QString& key, int* shiftOut, QString* error) {
    bool ok = false;
    int shift = key.trimmed().toInt(&ok);
    if (!ok) {
        if (error) {
            *error = "Key must be an integer shift.";
        }
        return false;
    }
    *shiftOut = shift;
    return true;
}

struct CyrillicAlphabet {
    QString lower;
    QString upper;
    bool loaded = false;
};

static const CyrillicAlphabet& cyrillicAlphabet() {
    static CyrillicAlphabet alphabet;
    if (alphabet.loaded) {
        return alphabet;
    }

    QFile file(":/alphabets.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        stream.setEncoding(QStringConverter::Utf8);
        alphabet.lower = stream.readLine().trimmed();
        alphabet.upper = stream.readLine().trimmed();
    }

    alphabet.loaded = true;
    return alphabet;
}

static QChar shiftChar(const QChar& c, int shift, const QString& lower, const QString& upper) {
    int idx = lower.indexOf(c);
    if (idx >= 0) {
        const int n = lower.size();
        const int s = ((shift % n) + n) % n;
        return lower.at((idx + s) % n);
    }

    idx = upper.indexOf(c);
    if (idx >= 0) {
        const int n = upper.size();
        const int s = ((shift % n) + n) % n;
        return upper.at((idx + s) % n);
    }

    return c;
}

static QString caesarTransform(const QString& text, int shift) {
    const QString latinLower = QStringLiteral("abcdefghijklmnopqrstuvwxyz");
    const QString latinUpper = QStringLiteral("ABCDEFGHIJKLMNOPQRSTUVWXYZ");

    const CyrillicAlphabet& cyr = cyrillicAlphabet();

    QString result;
    result.reserve(text.size());

    for (const QChar& c : text) {
        QChar shifted = shiftChar(c, shift, latinLower, latinUpper);
        if (shifted == c && !cyr.lower.isEmpty()) {
            shifted = shiftChar(c, shift, cyr.lower, cyr.upper);
        }
        result.append(shifted);
    }

    return result;
}

QString CaesarCipher::encryptText(const QString& text, const QString& key, QString* error) const {
    int shift = 0;
    if (!parseShift(key, &shift, error)) {
        return {};
    }
    return caesarTransform(text, shift);
}

QString CaesarCipher::decryptText(const QString& text, const QString& key, QString* error) const {
    int shift = 0;
    if (!parseShift(key, &shift, error)) {
        return {};
    }
    return caesarTransform(text, -shift);
}

} // namespace sds
