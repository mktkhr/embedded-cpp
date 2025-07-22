#include "sensor_handler.h"

#define ADC_PIN 32
#define ANALOG_PIN 33
#define ADC_CHANNEL ADC_CHANNEL_4

void adcInit(void) {
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1_CHANNEL_4, ADC_ATTEN_DB_12);
  esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_12, ADC_WIDTH_BIT_12, 1100, &adcChar);
}

String readAdcValue(int calculateNumber) {
  uint32_t readValue = 0;
  int readValueSum = 0;
  float readValueAvg = 0;
  for (int i = 0; i < calculateNumber; i++) {
    esp_adc_cal_get_voltage(ADC_CHANNEL, &adcChar, &readValue);
    readValueSum += readValue;
  }
  readValueAvg = readValueSum / float(calculateNumber);
  Serial.print("Average value(ADC): ");
  Serial.println(String(readValueAvg));
  return String(readValueAvg);
}

String readAnalogValue() {
  String response = "";
  response = analogRead(ANALOG_PIN);
  Serial.println("Analog Value: " + response);
  return response;
}

String measureEnv3() {
  float pressure = qmp6988.calcPressure() / 100;
  if (sht30.get() == 0) {
    temperature = sht30.cTemp;
    humidity = sht30.humidity;
  } else {
    temperature = 0, humidity = 0;
    Serial.println("Measurement failed (ENV3)");
    return "++";
  }
  Serial.printf("Temperature: %2.2f*C  Humiedity: %0.2f%%  Pressure: %0.2fhPa\r\n", temperature, humidity, pressure);
  return String(pressure) + "+" + String(temperature) + "+" + String(humidity);
}

uint32_t getAbsoluteHumidity(float temperature, float humidity) {
  const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature));
  const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity);
  return absoluteHumidityScaled;
}

String measureSgp30() {
  if (temperature != 0 || humidity != 0) {
    sgp.setHumidity(getAbsoluteHumidity(temperature, humidity));
  }

  if (!sgp.IAQmeasure()) {
    Serial.println("Measurement failed (SGP30)");
    return "++";
  }
  Serial.print("TVOC: ");
  Serial.print(sgp.TVOC);
  Serial.print("ppb ");
  Serial.print("eCO2: ");
  Serial.print(sgp.eCO2);
  Serial.println("ppm");

  if (!sgp.IAQmeasureRaw()) {
    Serial.println("Raw Measurement failed");
    return "++";
  }
  Serial.print("Raw H2: ");
  Serial.print(sgp.rawH2);
  Serial.print(" Raw Ethanol: ");
  Serial.println(sgp.rawEthanol);
  return String(sgp.eCO2) + "+" + String(sgp.TVOC);
}

String measureIllumination() {
  uint16_t lux = bh1750.getLUX();
  Serial.println("デジタル光センサ: " + String(lux) + " (lux)");
  return String(lux);
}

String sendCommandAndCollectResponse(String myCommand, int sendInterval, int requestNumber) {
  String response = "";
  for (int j = 0; j < requestNumber; j++) {
    mySDI12.sendCommand(myCommand);
    delay(sendInterval);

    if (mySDI12.available()) {
      Serial.println("Response detected (sendInterval: " + String(sendInterval) + ")");
      while (mySDI12.available()) {
        char responseChar = mySDI12.read();
        if ((responseChar != '\n') && (responseChar != '\r')) {
          response += responseChar;
          delay(10);
        }
      }
      mySDI12.clearBuffer();
      Serial.println("Response: " + response);
      return response;
      break;
    } else {
      mySDI12.clearBuffer();
      sendInterval = sendInterval + 10;
    }
    Serial.print(".");
  }
  Serial.println("Failed to connect to sensor");
  return response = "\0";
}

boolean checkActiveSdi12(byte i) {
  Serial.println("Checking address " + String(i) + "...");
  String response = "";
  String myCommand = String(i) + "!";
  int sendInterval = 50;
  int requestNumber = 5;

  response = sendCommandAndCollectResponse(myCommand, sendInterval, requestNumber);

  if (response != "\0") {
    return true;
  } else {
    return false;
  }
}

String measureSdi12(String sensorAddress) {
  String totalResponseString = "";
  int requestNumber = 5;
  int sendInterval = 50;

  String sdi12Response[10] = {"\0"};

  Serial.println("Start measurement (Sensor address: " + sensorAddress + ")");

  String myCommand = sensorAddress + "I!";
  String identResponse = sendCommandAndCollectResponse(myCommand, sendInterval, requestNumber);
  String sensorIdent = identResponse.substring(11, 16);

  if (sensorIdent == "5WTA " || sensorIdent == "5WET" || sensorIdent == "5WT  ") {
    sendInterval = 150;
  }

  String rawResponse = "";
  myCommand = sensorAddress + "C!";
  Serial.println("Command: " + myCommand);

  rawResponse = sendCommandAndCollectResponse(myCommand, sendInterval, requestNumber);

  if (rawResponse == "\0") {
    totalResponseString += "++++++++++";
    return totalResponseString;
  }

  int waitTime = rawResponse.substring(3, 4).toInt() * 1000;
  delay(waitTime);

  myCommand = sensorAddress + "D0!";
  Serial.println("Command: " + myCommand);
  rawResponse = sendCommandAndCollectResponse(myCommand, sendInterval, requestNumber);

  if (rawResponse == "\0") {
    totalResponseString += "++++++++++";
    return totalResponseString;
  }

  split(rawResponse, sdi12Response);

  if (sensorIdent == "5WET ") {
    totalResponseString = "1+" + sdi12Response[0] + "+" + sdi12Response[1] + "+" + sdi12Response[3] + "+" + sdi12Response[2] + "+++++";
  } else if (sensorIdent == "5WTA ") {
    totalResponseString = "2+" + sdi12Response[0] + "+" + sdi12Response[1] + "+" + sdi12Response[2] + "++++" + sdi12Response[3] + "+" + sdi12Response[4] + "+" + sdi12Response[5];
  } else if (sensorIdent == "5WT  ") {
    totalResponseString = "3+" + sdi12Response[0] + "+" + sdi12Response[1] + "+" + sdi12Response[2] + "++++++";
  } else if (sensorIdent == "TDT  " || sensorIdent == "00303" || sensorIdent == " 0030") {
    totalResponseString = "4+" + sdi12Response[0] + "+" + sdi12Response[1] + "+" + sdi12Response[2] + "+" + sdi12Response[3] + "+" + sdi12Response[4] + "++++";
  } else if (sensorIdent == "TR315") {
    totalResponseString = "5+" + sdi12Response[0] + "+" + sdi12Response[1] + "+" + sdi12Response[2] + "+" + sdi12Response[3] + "+" + sdi12Response[4] + "+" + sdi12Response[5] + "+++";
  } else if (sensorIdent == "TER12") {
    totalResponseString = "6+" + sdi12Response[0] + "+" + sdi12Response[1] + "+" + sdi12Response[2] + "++" + sdi12Response[3] + "++++";
  } else if (sensorIdent == "TER11") {
    totalResponseString = "7+" + sdi12Response[0] + "+" + sdi12Response[1] + "+" + sdi12Response[2] + "++++++";
  } else if (sensorIdent == "TER21" || sensorIdent == "MPS-2" || sensorIdent == "MPS-6") {
    totalResponseString = "8+" + sdi12Response[0] + "+" + sdi12Response[1] + "+" + sdi12Response[2] + "++++++";
  } else {
    totalResponseString = "999+" + sdi12Response[0] + "+" + sdi12Response[1] + "+" + sdi12Response[2] + "+" + sdi12Response[3] + "+" + sdi12Response[4] + "+" + sdi12Response[5] + "+" + sdi12Response[6] + "+" + sdi12Response[7] + "+" + sdi12Response[8];
  }

  return totalResponseString;
}

boolean addressChange(String present_address, String following_address) {
  String command = present_address + "A" + following_address + "!";
  String response = sendCommandAndCollectResponse(command, 50, 5);
  if (response == following_address) {
    return true;
  } else {
    return false;
  }
}

void split(String data, String *dataArray) {
  int index = 0;
  int arraySize = (sizeof(data) / sizeof((data)[0]));
  int datalength = data.length();
  for (int i = 0; i < datalength; i++) {
    char dataChar = data.charAt(i);
    if (dataChar == '+' || dataChar == ',') {
      index++;
    } else if (dataChar == '-') {
      index++;
      dataArray[index] += dataChar;
    } else {
      dataArray[index] += dataChar;
    }
  }
  return;
}

String convertEnv3ResultForSd(String result) {
  String response[3] = {"\0"};
  String resultForSd;
  split(result, response);
  return resultForSd = response[0] + "," + response[1] + "," + response[2] + ",";
}

String convertEnv3ResultForPost(String result) {
  String response[3] = {"\0"};
  String resultForPost;
  split(result, response);
  if (response[0] == "" && response[1] == "" && response[2] == "") {
    return "";
  }
  return resultForPost = "\"airPress\":\"" + response[0] + "\"," + "\"temp\":\"" + response[1] + "\",\"humi\":\"" + response[2] + "\",";
}

String convertSgp30ResultForSd(String result) {
  String response[2] = {"\0"};
  String resultForSd;
  split(result, response);
  return resultForSd = response[0] + "," + response[1] + ",";
}

String convertSgp30ResultForPost(String result) {
  String response[2] = {"\0"};
  String resultForPost;
  split(result, response);
  if (response[0] == "" && response[1] == "") {
    return "";
  }
  return resultForPost = "\"co2Concent\":\"" + response[0] + "\"," + "\"tvoc\":\"" + response[1] + "\",";
}

String convertSdi12ResultForSd(String result) {
  String response[10] = {"\0"};
  String resultForSd;
  split(result, response);
  return resultForSd = response[1] + "," + response[2] + "," + response[3] + "," + response[4] + "," + response[5] + "," + response[6] + "," + response[7] + "," + response[8] + ",";
}

String convertSdi12ResultForPost(String result) {
  String response[10] = {"\0"};
  String resultForPost;
  split(result, response);
  boolean isBlank = true;

  for (int i = 0; i < 10; i++) {
    if (!isBlank) {
      continue;
    }
    if (response[i] != "") {
      isBlank = false;
    }
  }

  if (isBlank) {
    return "";
  }

  return resultForPost = "{\"sensorId\":\"" + response[0] + "\"," + "\"sdiAddress\":\"" + response[1] + "\",\"vwc\":\"" + response[2] + "\",\"soilTemp\":\"" + response[3] + "\",\"brp\":\"" + response[4] + "\",\"sbec\":\"" + response[5] + "\",\"spwec\":\"" + response[6] + "\",\"gax\":\"" + response[7] + "\",\"gay\":\"" + response[8] + "\",\"gaz\":\"" + response[9] + "\"}";
}