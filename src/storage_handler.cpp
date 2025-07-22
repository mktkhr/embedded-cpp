#include "storage_handler.h"
#include "SD.h"
#include "FS.h"

void writePreference(int target, String value) {
  preferences.begin(emsPreference, false);

  switch (target) {
  case 0:
    preferences.putString("bootMode", value);
    preferences.end();
    break;
  case 1:
    preferences.putString("ssid", value);
    preferences.end();
    break;
  case 2:
    preferences.putString("pass", value);
    preferences.end();
    break;
  case 3:
    preferences.putString("host", value);
    preferences.end();
    break;
  case 4:
    preferences.putString("interval", value);
    preferences.end();
    break;
  case 5:
    preferences.putString("address", value);
    preferences.end();
    break;
  default:
    break;
  }
}

String readPreference(int target) {
  preferences.begin(emsPreference, false);
  String value;

  switch (target) {
  case 0:
    value = preferences.getString("bootMode", String(NORMAL_MODE));
    preferences.end();
    return value;
  case 1:
    value = preferences.getString("ssid", "未設定");
    preferences.end();
    return value;
  case 2:
    value = preferences.getString("pass", "未設定");
    preferences.end();
    return value;
  case 3:
    value = preferences.getString("host", "未設定");
    preferences.end();
    return value;
  case 4:
    value = preferences.getString("interval", "60");
    preferences.end();
    return value;
  case 5:
    value = preferences.getString("address", "");
    preferences.end();
    return value;
  default:
    preferences.end();
    return "";
  }
}

void listDir(const char *dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\n", dirname);

  File root = SD.open(dirname);
  if (!root) {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        listDir(file.name(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

void createDir(const char *path) {
  Serial.printf("Creating Dir: %s\n", path);
  if (SD.mkdir(path)) {
    Serial.println("Directory created");
  } else {
    Serial.println("Make directory failed");
  }
}

void removeDir(const char *path) {
  Serial.printf("Removing Directory: %s\n", path);
  if (SD.rmdir(path)) {
    Serial.println("Directory removed");
  } else {
    Serial.println("Remove directory failed");
  }
}

void readFile(const char *path) {
  Serial.printf("Reading file: %s\n", path);

  File file = SD.open(path);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.print("Read from file: ");
  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
}

void writeFile(const char *path, String message) {
  Serial.printf("Writing file: %s\n", path);

  File file = SD.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

void appendFile(const char *path, String message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = SD.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for appending");
    writeFile(path, message);
    return;
  }
  if (file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

void renameFile(const char *path1, const char *path2) {
  Serial.printf("Renaming file %s to %s\n", path1, path2);
  if (SD.rename(path1, path2)) {
    Serial.println("File renamed");
  } else {
    Serial.println("Rename failed");
  }
}

String deleteFile(const char *path) {
  String response;
  Serial.printf("Deleting file: %s\n", path);
  if (SD.remove(path)) {
    Serial.println("File deleted");
    response = "正常に削除されました。";
  } else {
    Serial.println("Delete failed");
    response = "削除に失敗しました。";
  }
  return response;
}

String readSdSize() {
  int total = SD.totalBytes() / (1024 * 1024);
  int used = SD.usedBytes() / (1024 * 1024);
  String sizelist = "SDカードの最大容量: " + String(total) + " MB<br>使用済み容量: " + String(used) + " MB<br>空き容量： " + String(total - used) + " MB";
  return sizelist;
}