#ifndef CAESARCIPHER_HPP
#define CAESARCIPHER_HPP

#include <string>

class CaesarCipher
{
public:
    explicit CaesarCipher(int key);

    std::string encrypt(const std::string& plaintext) const;
    std::string decrypt(const std::string& ciphertext) const;

    bool encryptFile(const std::string& inputFile, const std::string& outputFile) const;
    bool decryptFile(const std::string& inputFile, const std::string& outputFile) const;

    int getKey() const;
    void setKey(int key);

private:
    int key_;
    char shiftChar(char c, int shift) const;
};

#endif // CAESARCIPHER_HPP
