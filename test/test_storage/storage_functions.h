#ifndef STORAGE_FUNCTIONS_H
#define STORAGE_FUNCTIONS_H

#include "../common/arduino_mock.h"

// Mock Preferences実装
class TestPreferences {
private:
    static String data[10];
    static bool initialized;
    
public:
    static bool begin(const char* name, bool readOnly = false) {
        initialized = true;
        return true;
    }
    
    static void end() {
        initialized = false;
    }
    
    static String getString(const char* key, String defaultValue = "") {
        int index = std::atoi(key);
        if (index >= 0 && index < 10) {
            return data[index].isEmpty() ? defaultValue : data[index];
        }
        return defaultValue;
    }
    
    static size_t putString(const char* key, String value) {
        int index = std::atoi(key);
        if (index >= 0 && index < 10) {
            data[index] = value;
            return value.length();
        }
        return 0;
    }
    
    static void clear() {
        for (int i = 0; i < 10; i++) {
            data[i] = "";
        }
    }
};

// 静的メンバーの定義
String TestPreferences::data[10];
bool TestPreferences::initialized = false;

// テスト対象関数の実装（storage_handler.cppから移植）
void writePreference(int target, String value) {
    TestPreferences::begin("emsPreference", false);
    TestPreferences::putString(String(target).c_str(), value);
    TestPreferences::end();
}

String readPreference(int target) {
    TestPreferences::begin("emsPreference", true);
    String value = TestPreferences::getString(String(target).c_str(), "");
    TestPreferences::end();
    return value;
}

#endif // STORAGE_FUNCTIONS_H