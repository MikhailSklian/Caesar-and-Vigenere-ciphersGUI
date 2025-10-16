#include "Ciphers.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

constexpr int ENG = 26;
constexpr int RUS = 32;

// CP1251 коды для кириллицы
constexpr unsigned char CYR_A = 0xC0;   // А
constexpr unsigned char CYR_YA = 0xDF;  // Я
constexpr unsigned char CYR_a = 0xE0;   // а
constexpr unsigned char CYR_ya = 0xFF;  // я

VigenereCipher::VigenereCipher(const std::string& key) : key_(key) {}

char VigenereCipher::shiftChar(char c, int shift) const {
    unsigned char uc = static_cast<unsigned char>(c);

    if (uc >= 'A' && uc <= 'Z') return 'A' + (uc - 'A' + shift + ENG) % ENG;
    if (uc >= 'a' && uc <= 'z') return 'a' + (uc - 'a' + shift + ENG) % ENG;

    if (uc >= CYR_A && uc <= CYR_YA) return CYR_A + (uc - CYR_A + shift + RUS) % RUS;
    if (uc >= CYR_a && uc <= CYR_ya) return CYR_a + (uc - CYR_a + shift + RUS) % RUS;

    return c;
}

std::string VigenereCipher::extendKey(const std::string& key, size_t length) const {
    if (key.empty()) return "A";
    std::string extended;
    extended.reserve(length);
    for (size_t i = 0; i < length; ++i)
        extended += key[i % key.length()];
    return extended;
}

std::string VigenereCipher::encrypt(const std::string& plaintext) const {
    std::string result;
    std::string longKey = extendKey(key_, plaintext.size());

    for (size_t i = 0; i < plaintext.size(); ++i) {
        unsigned char k = static_cast<unsigned char>(longKey[i]);
        int shift = 0;

        if (k >= 'A' && k <= 'Z') shift = k - 'A';
        else if (k >= 'a' && k <= 'z') shift = k - 'a';
        else if (k >= CYR_A && k <= CYR_YA) shift = k - CYR_A;
        else if (k >= CYR_a && k <= CYR_ya) shift = k - CYR_a;

        result += shiftChar(plaintext[i], shift);
    }
    return result;
}

std::string VigenereCipher::decrypt(const std::string& ciphertext) const {
    std::string result;
    std::string longKey = extendKey(key_, ciphertext.size());

    for (size_t i = 0; i < ciphertext.size(); ++i) {
        unsigned char k = static_cast<unsigned char>(longKey[i]);
        int shift = 0;

        if (k >= 'A' && k <= 'Z') shift = k - 'A';
        else if (k >= 'a' && k <= 'z') shift = k - 'a';
        else if (k >= CYR_A && k <= CYR_YA) shift = k - CYR_A;
        else if (k >= CYR_a && k <= CYR_ya) shift = k - CYR_a;

        result += shiftChar(ciphertext[i], -shift);
    }
    return result;
}

bool VigenereCipher::encryptFile(const std::string& inputFile, const std::string& outputFile) const {
    try {
        std::ifstream inFile(inputFile, std::ios::binary);
        if (!inFile.is_open()) return false;

        std::stringstream buffer;
        buffer << inFile.rdbuf();
        inFile.close();

        std::ofstream outFile(outputFile, std::ios::binary);
        if (!outFile.is_open()) return false;

        outFile << encrypt(buffer.str());
        outFile.close();
        return true;
    }
    catch (...) { return false; }
}

bool VigenereCipher::decryptFile(const std::string& inputFile, const std::string& outputFile) const {
    try {
        std::ifstream inFile(inputFile, std::ios::binary);
        if (!inFile.is_open()) return false;

        std::stringstream buffer;
        buffer << inFile.rdbuf();
        inFile.close();

        std::ofstream outFile(outputFile, std::ios::binary);
        if (!outFile.is_open()) return false;

        outFile << decrypt(buffer.str());
        outFile.close();
        return true;
    }
    catch (...) { return false; }
}

std::string VigenereCipher::getKey() const { return key_; }

void VigenereCipher::setKey(const std::string& key) {
    if (key.empty()) throw std::invalid_argument("Key cannot be empty");
    key_ = key;
}
