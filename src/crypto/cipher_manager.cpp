#include "crypto/cipher_manager.h"
#include "crypto/atbash_cipher.h"
#include "crypto/caesar_cipher.h"
#include "crypto/huffman_codec.h"
#include "crypto/permutation_cipher.h"
#include "crypto/rsa_cipher.h"
#include "crypto/simple_aes_cipher.h"
#include "crypto/vigenere_cipher.h"
#include "crypto/xor_cipher.h"

namespace sds {

CipherManager::CipherManager() {
    ciphers_.push_back(std::make_unique<CaesarCipher>());
    ciphers_.push_back(std::make_unique<VigenereCipher>());
    ciphers_.push_back(std::make_unique<AtbashCipher>());
    ciphers_.push_back(std::make_unique<XorCipher>());
    ciphers_.push_back(std::make_unique<PermutationCipher>());
    ciphers_.push_back(std::make_unique<SimpleAesCipher>());
    ciphers_.push_back(std::make_unique<HuffmanCodec>());
    ciphers_.push_back(std::make_unique<RsaCipher>());
}

const CipherBase* CipherManager::findById(const QString& id) const {
    for (const auto& cipher : ciphers_) {
        if (cipher->id() == id) {
            return cipher.get();
        }
    }
    return nullptr;
}

const CipherBase* CipherManager::findByName(const QString& name) const {
    for (const auto& cipher : ciphers_) {
        if (cipher->name() == name) {
            return cipher.get();
        }
    }
    return nullptr;
}

} // namespace sds
