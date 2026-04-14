#ifndef SDS_CIPHER_MANAGER_H
#define SDS_CIPHER_MANAGER_H

#include "crypto/cipher_base.h"
#include <memory>
#include <vector>

namespace sds {

class CipherManager {
public:
    CipherManager();

    const std::vector<std::unique_ptr<CipherBase>>& ciphers() const { return ciphers_; }

    const CipherBase* findById(const QString& id) const;
    const CipherBase* findByName(const QString& name) const;

private:
    std::vector<std::unique_ptr<CipherBase>> ciphers_;
};

} // namespace sds

#endif // SDS_CIPHER_MANAGER_H
