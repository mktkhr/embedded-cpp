# CLAUDE.md

このファイルはClaude Code (claude.ai/code)がこのリポジトリで作業する際のガイダンスを提供します。

**重要**: このファイル以降に追記する内容は全て日本語で記載してください。

## 一般的な開発コマンド

### ビルドとアップロード
- `make build` または `platformio run` - プロジェクトをビルド
- `make upload` - ESP32にファームウェアをアップロード
- `make flash` - アップロードとモニタリング開始 (build + upload + monitor)
- `make monitor` - シリアルモニターを開く (115200 baud)
- `make full` - 完全サイクル: clean, build, upload, monitor

### 開発ユーティリティ
- `make setup` - 初期プロジェクトセットアップ (PlatformIOと依存関係のインストール)
- `make lib-install` - ライブラリ依存関係のインストール
- `make clean` - ビルド成果物のクリーンアップ
- `make format` - clang-formatを使用したコードフォーマット
- `make check` - PlatformIOコード解析の実行
- `make devices` - 接続されたESP32デバイスの一覧表示

### テストとデバッグ
- `make test` - テストの実行 (設定されている場合)
- `make debug` - デバッグフラグ付きビルド
- `make compiledb` - IDE サポート用の compile_commands.json 生成

## プロジェクトアーキテクチャ

これは以下のモジュラーアーキテクチャを持つESP32ベースの環境監視システムです:

### コアコンポーネント
- **main.cpp** - 測定スケジューリングとデータ収集を含むメインアプリケーションループ
- **sensor_handler** - 複数のセンサータイプに対する統一センサーインターフェース
- **wifi_handler** - WiFi接続とHTTP/HTTPS通信
- **storage_handler** - SDカードデータロギングと設定管理  
- **web_server_handler** - 設定用Webインターフェース (メンテナンスモード)

### センサー統合
システムは統一インターフェースを通じて複数のセンサータイプをサポート:
- **SDI-12センサー** - 土壌監視センサー (最大4デバイス)
- **環境センサー** - SHT30 (温湿度), QMP6988 (気圧)
- **空気質** - SGP30 (CO2/TVOC)  
- **光センサー** - BH1750 照度
- **アナログ入力** - カスタム電圧測定

### 動作モード
- **通常モード** - 継続的なセンサー監視とデータ送信
- **メンテナンスモード** - Web設定インターフェース (起動時にボタン押下)
  - 192.168.4.11でAP "ems_stamp" を作成
  - WiFi認証情報とセンサー設定を許可

### データフロー
1. **収集** - 設定可能な間隔でセンサーを読み取り (src/main.cpp:175)
2. **処理** - データをJSONとCSV形式に変換
3. **保存** - SDカードへのローカルロギング (/log.csv)
4. **送信** - リモートサーバーへのHTTPS POST (www.ems-engineering.jp)

### 主要設定
- **起動モード検出** - GPIO 39のボタン (src/main.cpp:90)
- **センサータイミング** - 設定による測定間隔設定
- **WiFi認証情報** - ESP32設定に保存、Webインターフェースで設定
- **LEDステータス** - GPIO 27の単一RGB LEDでステータス表示

### ライブラリ依存関係 (platformio.ini)
- Adafruit SGP30, M5Unit-ENV, M5-DLight (センサー用)
- EnviroDIY SDI-12 (SDI-12通信用)
- ArduinoJson (データシリアライゼーション用)  
- FastLED (ステータス表示用)

### ファイル構成
- `include/` - 関数宣言を含むヘッダーファイル
- `src/` - 実装ファイル  
