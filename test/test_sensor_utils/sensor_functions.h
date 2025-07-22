#ifndef SENSOR_FUNCTIONS_H
#define SENSOR_FUNCTIONS_H

#include "../common/arduino_mock.h"

// sensor_handler.cppから移植するテスト対象関数

/**
 * 絶対湿度を計算する関数
 * @param temperature 温度(摂氏)
 * @param humidity 相対湿度(%)
 * @return 絶対湿度 (g/m³ * 1000)
 */
uint32_t getAbsoluteHumidity(float temperature, float humidity)
{
    // 飽和水蒸気圧の計算（Magnus式）
    float saturationVaporPressure = 6.112 * exp((17.67 * temperature) / (temperature + 243.5));

    // 実際の水蒸気圧
    float actualVaporPressure = saturationVaporPressure * (humidity / 100.0);

    // 絶対湿度の計算 (g/m³)
    float absoluteHumidity = (actualVaporPressure * 2.1674) / (273.15 + temperature);

    return (uint32_t)(absoluteHumidity * 1000); // mg/m³単位で返す
}

/**
 * 文字列を1文字ずつに分割する関数
 * @param input 入力文字列
 * @param output 出力配列
 */
void split(String input, String *output)
{
    for (int i = 0; i < input.length(); i++)
    {
        output[i] = String(input.charAt(i));
    }
}

/**
 * ENV3センサーデータをSDカード用フォーマットに変換
 * @param data "気圧,温度,湿度" 形式の文字列
 * @return SDカード用カンマ区切り文字列
 */
String convertEnv3ResultForSd(String data)
{
    if (data.isEmpty())
    {
        return ",,,"; // 空データの場合
    }
    return "," + data; // カンマを先頭に追加
}

/**
 * ENV3センサーデータをPOST用JSON形式に変換
 * @param data "気圧,温度,湿度" 形式の文字列
 * @return JSON文字列の一部
 */
String convertEnv3ResultForPost(String data)
{
    if (data.isEmpty())
    {
        return "";
    }

    int firstComma = data.indexOf(',');
    int secondComma = data.indexOf(',', firstComma + 1);

    if (firstComma == -1 || secondComma == -1)
    {
        return "";
    }

    String pressure = data.substring(0, firstComma);
    String temperature = data.substring(firstComma + 1, secondComma);
    String humidity = data.substring(secondComma + 1);

    return "\"pressure\":\"" + pressure + "\",\"temperature\":\"" + temperature + "\",\"humidity\":\"" + humidity + "\",";
}

/**
 * SGP30センサーデータをSDカード用フォーマットに変換
 * @param data "CO2,TVOC" 形式の文字列
 * @return SDカード用カンマ区切り文字列
 */
String convertSgp30ResultForSd(String data)
{
    if (data.isEmpty())
    {
        return ",,"; // 空データの場合
    }
    return "," + data; // カンマを先頭に追加
}

/**
 * SGP30センサーデータをPOST用JSON形式に変換
 * @param data "CO2,TVOC" 形式の文字列
 * @return JSON文字列の一部
 */
String convertSgp30ResultForPost(String data)
{
    if (data.isEmpty())
    {
        return "";
    }

    int commaIndex = data.indexOf(',');
    if (commaIndex == -1)
    {
        return "";
    }

    String co2 = data.substring(0, commaIndex);
    String tvoc = data.substring(commaIndex + 1);

    return "\"co2\":\"" + co2 + "\",\"tvoc\":\"" + tvoc + "\",";
}

#endif // SENSOR_FUNCTIONS_H
