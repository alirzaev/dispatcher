#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include <QString>

#include <botan/cipher_mode.h>
#include <botan/hash.h>
#include <botan/hex.h>
#include <botan/rng.h>
#include <botan/auto_rng.h>

namespace QtUtils::Cryptography {

using ByteArray = Botan::secure_vector<uint8_t>;

inline ByteArray hash(const std::string &bytes) {
  std::unique_ptr<Botan::HashFunction> hasher(
      Botan::HashFunction::create("SHA-256"));
  return hasher->process(bytes);
}

inline std::string encrypt(const std::string &tasks,
                           const QString &passphrase) {
  std::unique_ptr<Botan::Cipher_Mode> enc(
      Botan::get_cipher_mode("AES-256/CBC/PKCS7", Botan::ENCRYPTION));
  enc->set_key(hash(passphrase.toStdString()));

  Botan::AutoSeeded_RNG rng;

  ByteArray iv = rng.random_vec(enc->default_nonce_length());
  ByteArray ciphertext((uint8_t *)tasks.data(),
                       (uint8_t *)(tasks.data() + tasks.size()));

  enc->start(iv);
  enc->finish(ciphertext);

  return Botan::hex_encode(iv) + Botan::hex_encode(ciphertext);
}

inline std::string decrypt(const std::string &ciphertext,
                           const QString &passphrase) {
  std::unique_ptr<Botan::Cipher_Mode> enc(
      Botan::get_cipher_mode("AES-256/CBC/PKCS7", Botan::DECRYPTION));
  enc->set_key(hash(passphrase.toStdString()));

  auto decoded = Botan::hex_decode(ciphertext);

  Botan::secure_vector<uint8_t> plaintext(
      (uint8_t *)decoded.data(), (uint8_t *)(decoded.data() + decoded.size()));

  enc->start(plaintext.data(), enc->default_nonce_length());
  enc->finish(plaintext);

  return std::string((char *)(plaintext.data() + enc->default_nonce_length()),
                     plaintext.size() - enc->default_nonce_length());
}
} // namespace QtUtils::Cryptography
