#include <unity.h>
#include "../common/arduino_mock.h"
#include "sensor_functions.h"

void setUp(void) {
    // 各テスト前の初期化
}

void tearDown(void) {
    // 各テスト後のクリーンアップ
}

// 絶対湿度計算のテスト
void test_getAbsoluteHumidity_normal_values(void) {
    // 正常な温度・湿度値でのテスト
    uint32_t result = getAbsoluteHumidity(25.0, 60.0);
    TEST_ASSERT_GREATER_THAN(0, result);
    TEST_ASSERT_LESS_THAN(30000, result); // 妥当な範囲内
}

void test_getAbsoluteHumidity_zero_humidity(void) {
    // 湿度0%の場合
    uint32_t result = getAbsoluteHumidity(25.0, 0.0);
    TEST_ASSERT_EQUAL(0, result);
}

void test_getAbsoluteHumidity_high_temperature(void) {
    // 高温での計算
    uint32_t result = getAbsoluteHumidity(50.0, 50.0);
    TEST_ASSERT_GREATER_THAN(0, result);
}

void test_getAbsoluteHumidity_low_temperature(void) {
    // 低温での計算
    uint32_t result = getAbsoluteHumidity(0.0, 50.0);
    TEST_ASSERT_GREATER_THAN(0, result);
}

// 文字列分割のテスト
void test_split_single_address(void) {
    String output[1];
    split("1", output);
    TEST_ASSERT_EQUAL_STRING("1", output[0].c_str());
}

void test_split_multiple_addresses(void) {
    String output[3];
    split("123", output);
    TEST_ASSERT_EQUAL_STRING("1", output[0].c_str());
    TEST_ASSERT_EQUAL_STRING("2", output[1].c_str());
    TEST_ASSERT_EQUAL_STRING("3", output[2].c_str());
}

void test_split_empty_string(void) {
    String output[1];
    split("", output);
    // 空文字列の場合の挙動確認
}

// ENV3結果変換のテスト（SDカード用）
void test_convertEnv3ResultForSd_valid_data(void) {
    String input = "1013.25,25.5,60.2";
    String result = convertEnv3ResultForSd(input);
    TEST_ASSERT_TRUE(result.indexOf("1013.25") >= 0);
    TEST_ASSERT_TRUE(result.indexOf("25.5") >= 0);
    TEST_ASSERT_TRUE(result.indexOf("60.2") >= 0);
}

void test_convertEnv3ResultForSd_empty_data(void) {
    String input = "";
    String result = convertEnv3ResultForSd(input);
    // 空データの処理確認
    TEST_ASSERT_NOT_NULL(result.c_str());
}

// ENV3結果変換のテスト（POST用）
void test_convertEnv3ResultForPost_valid_data(void) {
    String input = "1013.25,25.5,60.2";
    String result = convertEnv3ResultForPost(input);
    // JSON形式での出力確認
    TEST_ASSERT_TRUE(result.indexOf("pressure") >= 0 || result.indexOf("1013.25") >= 0);
}

// SGP30結果変換のテスト（SDカード用）
void test_convertSgp30ResultForSd_valid_data(void) {
    String input = "400,50";
    String result = convertSgp30ResultForSd(input);
    TEST_ASSERT_TRUE(result.indexOf("400") >= 0);
    TEST_ASSERT_TRUE(result.indexOf("50") >= 0);
}

// SGP30結果変換のテスト（POST用）
void test_convertSgp30ResultForPost_valid_data(void) {
    String input = "400,50";
    String result = convertSgp30ResultForPost(input);
    // JSON形式での出力確認
    TEST_ASSERT_TRUE(result.indexOf("co2") >= 0 || result.indexOf("400") >= 0);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    
    // 絶対湿度計算テスト
    RUN_TEST(test_getAbsoluteHumidity_normal_values);
    RUN_TEST(test_getAbsoluteHumidity_zero_humidity);
    RUN_TEST(test_getAbsoluteHumidity_high_temperature);
    RUN_TEST(test_getAbsoluteHumidity_low_temperature);
    
    // 文字列分割テスト
    RUN_TEST(test_split_single_address);
    RUN_TEST(test_split_multiple_addresses);
    RUN_TEST(test_split_empty_string);
    
    // データ変換テスト
    RUN_TEST(test_convertEnv3ResultForSd_valid_data);
    RUN_TEST(test_convertEnv3ResultForSd_empty_data);
    RUN_TEST(test_convertEnv3ResultForPost_valid_data);
    RUN_TEST(test_convertSgp30ResultForSd_valid_data);
    RUN_TEST(test_convertSgp30ResultForPost_valid_data);
    
    return UNITY_END();
}