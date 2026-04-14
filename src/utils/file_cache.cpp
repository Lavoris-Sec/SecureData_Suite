#include "utils/file_cache.h"
#include <QCryptographicHash>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>

namespace sds {

FileCache::FileCache() {
    const QString baseDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    cacheDir_ = QDir(baseDir).filePath("cache");
    QDir().mkpath(cacheDir_);
}

bool FileCache::saveEncryptedCopy(const QString& originalPath,
                                  const QByteArray& encryptedData,
                                  const QString& cipherId,
                                  QString* error,
                                  QString* outPath) const {
    const QByteArray seed = originalPath.toUtf8() + QByteArray::number(QDateTime::currentSecsSinceEpoch());
    const QString hash = QString::fromLatin1(QCryptographicHash::hash(seed, QCryptographicHash::Sha256).toHex());

    const QString encPath = QDir(cacheDir_).filePath(hash + ".enc");
    QFile encFile(encPath);
    if (!encFile.open(QIODevice::WriteOnly)) {
        if (error) {
            *error = "Failed to write cache file.";
        }
        return false;
    }
    encFile.write(encryptedData);
    encFile.close();

    QJsonObject meta;
    meta["original_path"] = originalPath;
    meta["cipher_id"] = cipherId;
    meta["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    meta["size"] = static_cast<qint64>(encryptedData.size());

    const QString metaPath = QDir(cacheDir_).filePath(hash + ".json");
    QFile metaFile(metaPath);
    if (metaFile.open(QIODevice::WriteOnly)) {
        metaFile.write(QJsonDocument(meta).toJson(QJsonDocument::Indented));
        metaFile.close();
    }

    if (outPath) {
        *outPath = encPath;
    }

    return true;
}

} // namespace sds
