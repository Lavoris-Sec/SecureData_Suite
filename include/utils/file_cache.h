#ifndef SDS_FILE_CACHE_H
#define SDS_FILE_CACHE_H

#include <QString>

namespace sds {

class FileCache {
public:
    FileCache();

    QString cacheDir() const { return cacheDir_; }

    bool saveEncryptedCopy(const QString& originalPath,
                           const QByteArray& encryptedData,
                           const QString& cipherId,
                           QString* error,
                           QString* outPath = nullptr) const;

private:
    QString cacheDir_;
};

} // namespace sds

#endif // SDS_FILE_CACHE_H
