# M5Stamp Pico EMS Sensor Project

M5Stamp Picoを使用したESP32ベースの環境監視システムです。PlatformIOで開発されています。

## プロジェクト構造

```
embedded-cpp/
├── .devcontainer/       # VS Code Dev Container設定
│   └── devcontainer.json
├── include/             # ヘッダーファイル
│   ├── index_html.h
│   ├── sensor_handler.h
│   ├── storage_handler.h
│   ├── version.h
│   ├── web_server_handler.h
│   └── wifi_handler.h
├── src/                 # ソースファイル
│   ├── index_html.cpp
│   ├── main.cpp
│   ├── sensor_handler.cpp
│   ├── storage_handler.cpp
│   ├── web_server_handler.cpp
│   └── wifi_handler.cpp
├── Makefile             # 開発用コマンド
├── CLAUDE.md            # Claude Code用ガイダンス
├── platformio.ini       # PlatformIO設定
└── README.md

```

## ハードウェア

- **メインボード**: M5Stamp Pico (ESP32, 240MHz, 4MB Flash, 320KB RAM)
- **センサー**: 複数の環境センサーをサポート
- **通信**: WiFi, SDI-12プロトコル
- **ストレージ**: SDカード
- **LED**: ステータス表示用RGB LED

## 機能

- **環境データ収集**: 複数センサーからの自動データ収集
- **SDI-12対応**: 土壌センサー等の専門センサーに対応
- **データ送信**: WiFi経由でHTTPSサーバーへ送信
- **ローカル保存**: SDカードにCSV形式で記録
- **Web設定**: ブラウザ経由での設定変更
- **センサー対応**:
  - SHT30 (温湿度)
  - QMP6988 (大気圧)
  - SGP30 (CO2/TVOC濃度)
  - BH1750 (照度)
  - SDI-12土壌センサー (最大4台)
  - アナログセンサー

## 開発環境のセットアップ

### 1. VS Code Dev Container を使用する場合（推奨）

1. VS Code と Docker をインストール
2. VS Code で「Remote - Containers」拡張機能をインストール
3. Claude CLIを使用する場合は、ローカル環境で API キーを設定:
   ```bash
   export ANTHROPIC_API_KEY='your-api-key-here'
   ```
4. このフォルダを VS Code で開く
5. 「Reopen in Container」を選択

### 2. ローカル環境の場合

1. PlatformIO Core をインストール:
   ```bash
   pip install platformio
   ```

2. 依存関係をインストール:
   ```bash
   platformio lib install
   ```

## ビルドとアップロード

### PlatformIOコマンド
```bash
# ビルド
platformio run

# M5Stamp Picoへアップロード
platformio run --target upload

# シリアルモニター
platformio device monitor
```

### Makefileコマンド（推奨）
```bash
# ビルド
make build

# アップロード
make upload

# アップロード＋モニター
make flash

# フルサイクル（クリーン＋ビルド＋アップロード＋モニター）
make full

# ヘルプ
make help
```

## 設定

- WiFi認証情報は Web インターフェース経由で設定
- ボタンを押しながら起動すると設定モードに入る
- 設定モードでは `ems_stamp` という SSID の AP が起動
- `192.168.4.11` にアクセスして設定

## 動作モード

### 通常モード
- 設定された間隔でセンサーデータを収集
- HTTPSでサーバーに送信
- SDカードにCSV保存
- LED: 緑（正常）、赤（エラー）

### メンテナンスモード
- GPIO39ボタンを押しながら起動
- AP「ems_stamp」(パスワード: 00000000)を起動
- http://192.168.4.11 でWeb設定画面
- WiFi設定、測定間隔、センサー設定が可能
- LED: 青（設定モード）

## ライブラリ依存関係

- **Adafruit SGP30 Sensor**: CO2/TVOCセンサー
- **M5Unit-ENV**: 温湿度・気圧センサー
- **M5-DLight**: 照度センサー
- **EnviroDIY SDI-12**: SDI-12プロトコル通信
- **ArduinoJson**: JSON処理
- **FastLED**: RGB LED制御

## 技術仕様

### プラットフォーム
- **Framework**: Arduino for ESP32
- **Build System**: PlatformIO
- **Target**: M5Stamp Pico (ESP32)
- **Flash**: 4MB
- **RAM**: 320KB (PSRAM非搭載)

### 通信プロトコル
- **WiFi**: IEEE 802.11 b/g/n
- **HTTP/HTTPS**: データ送信
- **SDI-12**: 土壌センサー通信
- **I2C**: 環境センサー通信
- **SPI**: SDカードアクセス

### ピン配置（M5Stamp Pico）
- **GPIO19**: SDI-12 データ
- **GPIO21/22**: I2C (SDA/SCL)
- **GPIO27**: RGB LED
- **GPIO39**: 設定モードボタン
- **GPIO0**: SDカード CS
- **GPIO32/33**: アナログ入力

## 開発者向け情報

詳細な開発ガイダンスは [CLAUDE.md](./CLAUDE.md) を参照してください。
