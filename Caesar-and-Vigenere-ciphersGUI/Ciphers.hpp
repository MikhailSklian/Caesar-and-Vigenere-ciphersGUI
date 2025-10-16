#ifndef CIPHERS_HPP
#define CIPHERS_HPP

#include <string>

/**
 * Класс для работы с шифром Цезаря
 */
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

/**
 * Класс для работы с шифром Виженера
 */
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

#endif // CIPHERS_HPP
