#ifndef ARDUINO_MOCK_H
#define ARDUINO_MOCK_H

#include <string>
#include <cmath>
#include <cstdlib>

// Arduino String クラスのシンプルなモック実装
class String {
private:
    std::string data;
    
public:
    String() : data("") {}
    String(const char* str) : data(str ? str : "") {}
    String(const std::string& str) : data(str) {}
    String(int value) : data(std::to_string(value)) {}
    String(float value) : data(std::to_string(value)) {}
    String(char c) : data(1, c) {}
    
    // 基本的なメソッド
    int length() const { return data.length(); }
    bool isEmpty() const { return data.empty(); }
    const char* c_str() const { return data.c_str(); }
    
    // 文字アクセス
    char charAt(int index) const {
        if (index >= 0 && index < data.length()) {
            return data[index];
        }
        return '\0';
    }
    
    // 検索
    int indexOf(char c) const {
        size_t pos = data.find(c);
        return (pos != std::string::npos) ? pos : -1;
    }
    
    int indexOf(const String& str) const {
        size_t pos = data.find(str.data);
        return (pos != std::string::npos) ? pos : -1;
    }
    
    int indexOf(char c, int fromIndex) const {
        if (fromIndex < 0) fromIndex = 0;
        size_t pos = data.find(c, fromIndex);
        return (pos != std::string::npos) ? pos : -1;
    }
    
    // 部分文字列
    String substring(int beginIndex) const {
        if (beginIndex < 0) beginIndex = 0;
        if (beginIndex >= data.length()) return String("");
        return String(data.substr(beginIndex));
    }
    
    String substring(int beginIndex, int endIndex) const {
        if (beginIndex < 0) beginIndex = 0;
        if (endIndex > data.length()) endIndex = data.length();
        if (beginIndex >= endIndex) return String("");
        return String(data.substr(beginIndex, endIndex - beginIndex));
    }
    
    // 演算子
    String operator+(const String& other) const {
        return String(data + other.data);
    }
    
    String operator+(const char* other) const {
        return String(data + (other ? other : ""));
    }
    
    friend String operator+(const char* lhs, const String& rhs) {
        return String(std::string(lhs ? lhs : "") + rhs.data);
    }
    
    String& operator+=(const String& other) {
        data += other.data;
        return *this;
    }
    
    String& operator+=(const char* other) {
        if (other) data += other;
        return *this;
    }
    
    bool operator==(const String& other) const {
        return data == other.data;
    }
    
    bool operator!=(const String& other) const {
        return data != other.data;
    }
    
    String& operator=(const String& other) {
        data = other.data;
        return *this;
    }
};

// 基本的な型定義
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;

#endif // ARDUINO_MOCK_H