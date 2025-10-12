#ifndef VIGENERECIPHER_HPP
#define VIGENERECIPHER_HPP

#include <string>

class VigenereCipher
{
public:
    explicit VigenereCipher(const std::string& key);

    std::string encrypt(const std::string& plaintext) const;
    std::string decrypt(const std::string& ciphertext) const;

    bool encryptFile(const std::string& inputFile, const std::string& outputFile) const;
    bool decryptFile(const std::string& inputFile, const std::string& outputFile) const;

    std::string getKey() const;
    void setKey(const std::string& key);

private:
    std::string key_;
    char shiftChar(char c, int shift) const;
    std::string extendKey(const std::string& key, size_t length) const;
};

#endif // VIGENERECIPHER_HPP
