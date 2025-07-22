#include <unity.h>
#include "../common/arduino_mock.h"
#include "storage_functions.h"

void setUp(void) {
    TestPreferences::clear();
    TestPreferences::begin("test");
}

void tearDown(void) {
    TestPreferences::end();
}

// writePreferenceのテスト
void test_writePreference_valid_target(void) {
    writePreference(1, "test_ssid");
    TEST_ASSERT_EQUAL_STRING("test_ssid", TestPreferences::getString("1").c_str());
}

void test_writePreference_overwrite_existing(void) {
    writePreference(2, "old_password");
    writePreference(2, "new_password");
    TEST_ASSERT_EQUAL_STRING("new_password", TestPreferences::getString("2").c_str());
}

void test_writePreference_empty_value(void) {
    writePreference(3, "");
    TEST_ASSERT_EQUAL_STRING("", TestPreferences::getString("3").c_str());
}

void test_writePreference_long_value(void) {
    String longValue = "this_is_a_very_long_value_that_might_cause_issues_in_some_storage_systems";
    writePreference(4, longValue);
    TEST_ASSERT_EQUAL_STRING(longValue.c_str(), TestPreferences::getString("4").c_str());
}

// readPreferenceのテスト
void test_readPreference_existing_value(void) {
    TestPreferences::putString("1", "existing_ssid");
    String result = readPreference(1);
    TEST_ASSERT_EQUAL_STRING("existing_ssid", result.c_str());
}

void test_readPreference_nonexistent_value(void) {
    String result = readPreference(9); // 未設定のキー
    TEST_ASSERT_EQUAL_STRING("", result.c_str());
}

void test_readPreference_after_write(void) {
    writePreference(5, "test_host");
    String result = readPreference(5);
    TEST_ASSERT_EQUAL_STRING("test_host", result.c_str());
}

// 設定値の種類別テスト（実際の設定項目に基づく）
void test_boot_mode_preference(void) {
    // target 0: ブートモード
    writePreference(0, "通常モード");
    String result = readPreference(0);
    TEST_ASSERT_EQUAL_STRING("通常モード", result.c_str());
}

void test_wifi_credentials_preference(void) {
    // target 1: SSID, target 2: パスワード
    writePreference(1, "my_wifi_ssid");
    writePreference(2, "my_wifi_password");
    
    TEST_ASSERT_EQUAL_STRING("my_wifi_ssid", readPreference(1).c_str());
    TEST_ASSERT_EQUAL_STRING("my_wifi_password", readPreference(2).c_str());
}

void test_server_settings_preference(void) {
    // target 3: サーバーホスト
    writePreference(3, "192.168.1.100");
    String result = readPreference(3);
    TEST_ASSERT_EQUAL_STRING("192.168.1.100", result.c_str());
}

void test_measurement_settings_preference(void) {
    // target 4: 測定間隔, target 5: SDI-12アドレス
    writePreference(4, "15");
    writePreference(5, "123");
    
    TEST_ASSERT_EQUAL_STRING("15", readPreference(4).c_str());
    TEST_ASSERT_EQUAL_STRING("123", readPreference(5).c_str());
}

// エラーハンドリングのテスト
void test_invalid_target_range(void) {
    // 無効な範囲のtarget値
    writePreference(-1, "invalid");
    writePreference(100, "invalid");
    
    // エラーハンドリングの確認（実装依存）
    String result1 = readPreference(-1);
    String result2 = readPreference(100);
    
    // 通常は空文字列が返されることを期待
    TEST_ASSERT_EQUAL_STRING("", result1.c_str());
    TEST_ASSERT_EQUAL_STRING("", result2.c_str());
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    
    // 基本的な書き込み・読み込みテスト
    RUN_TEST(test_writePreference_valid_target);
    RUN_TEST(test_writePreference_overwrite_existing);
    RUN_TEST(test_writePreference_empty_value);
    RUN_TEST(test_writePreference_long_value);
    
    RUN_TEST(test_readPreference_existing_value);
    RUN_TEST(test_readPreference_nonexistent_value);
    RUN_TEST(test_readPreference_after_write);
    
    // 実際の設定項目テスト
    RUN_TEST(test_boot_mode_preference);
    RUN_TEST(test_wifi_credentials_preference);
    RUN_TEST(test_server_settings_preference);
    RUN_TEST(test_measurement_settings_preference);
    
    // エラーハンドリングテスト
    RUN_TEST(test_invalid_target_range);
    
    return UNITY_END();
}