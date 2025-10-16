#include "Ciphers.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

constexpr int ENG = 26;
constexpr int RUS = 32;

CaesarCipher::CaesarCipher(int key) : key_(key) {}

std::string CaesarCipher::encrypt(const std::string& plaintext) const {
    std::string result;
    for (char c : plaintext)
        result += shiftChar(c, key_);
    return result;
}

std::string CaesarCipher::decrypt(const std::string& ciphertext) const {
    std::string result;
    for (char c : ciphertext)
        result += shiftChar(c, -key_);
    return result;
}

int CaesarCipher::getKey() const { return key_; }
void CaesarCipher::setKey(int key) { key_ = key; }

char CaesarCipher::shiftChar(char c, int shift) const {
    char result = c;

    if (c >= 'A' && c <= 'Z') {
        c += (shift % ENG);
        if (c > 'Z') c = 'A' + (c - 'Z' - 1);
        else if (c < 'A') c = 'Z' - ('A' - c - 1);
        return c;
    }
    if (c >= 'a' && c <= 'z') {
        c += (shift % ENG);
        if (c > 'z') c = 'a' + (c - 'z' - 1);
        else if (c < 'a') c = 'z' - ('a' - c - 1);
        return c;
    }
    if (c >= 'À' && c <= 'ß') {
        c += (shift % RUS);
        if (c > 'ß') c = 'À' + (c - 'ß' - 1);
        else if (c < 'À') c = 'ß' - ('À' - c - 1);
        return c;
    }
    if (c >= 'à' && c <= 'ÿ') {
        c += (shift % RUS);
        if (c > 'ÿ') c = 'à' + (c - 'ÿ' - 1);
        else if (c < 'à') c = 'ÿ' - ('à' - c - 1);
        return c;
    }

    return result;
}

bool CaesarCipher::encryptFile(const std::string& inputFile, const std::string& outputFile) const {
    try {
        std::ifstream inFile(inputFile);
        if (!inFile.is_open()) return false;

        std::stringstream buffer;
        buffer << inFile.rdbuf();
        std::string content = buffer.str();
        inFile.close();

        std::ofstream outFile(outputFile);
        if (!outFile.is_open()) return false;

        outFile << encrypt(content);
        outFile.close();
        return true;
    }
    catch (...) { return false; }
}

bool CaesarCipher::decryptFile(const std::string& inputFile, const std::string& outputFile) const {
    try {
        std::ifstream inFile(inputFile);
        if (!inFile.is_open()) return false;

        std::stringstream buffer;
        buffer << inFile.rdbuf();
        std::string content = buffer.str();
        inFile.close();

        std::ofstream outFile(outputFile);
        if (!outFile.is_open()) return false;

        outFile << decrypt(content);
        outFile.close();
        return true;
    }
    catch (...) { return false; }
}
