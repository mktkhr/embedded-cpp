.PHONY: all build upload clean monitor test setup lib-install lib-update check format help

all: build

# プロジェクトのビルド
build:
	@echo "プロジェクトをビルド中..."
	platformio run

# ESP32へのアップロード
upload:
	@echo "ESP32にアップロード中..."
	platformio run --target upload

# ビルドファイルのクリーンアップ
clean:
	@echo "ビルドファイルをクリーンアップ中..."
	platformio run --target clean
	rm -rf .pio

# シリアルモニターの起動
monitor:
	@echo "シリアルモニターを起動中..."
	platformio device monitor

# ビルド、アップロード後にモニター
flash: upload monitor

# テストの実行(native)
test:
	@echo "テストを実行中..."
	platformio test -e native

# 初期セットアップ
setup:
	@echo "PlatformIO環境をセットアップ中..."
	pip install --upgrade platformio
	platformio update
	$(MAKE) lib-install

# ライブラリのインストール
lib-install:
	@echo "ライブラリをインストール中..."
	platformio lib install

# ライブラリのアップデート
lib-update:
	@echo "ライブラリをアップデート中..."
	platformio lib update

# コードの検査
check:
	@echo "コードを検査中..."
	platformio check

# コードフォーマット (clang-format必須)
format:
	@echo "コードをフォーマット中..."
	find src include -name "*.cpp" -o -name "*.h" | xargs clang-format -i

# 接続デバイスの一覧表示
devices:
	@echo "接続デバイスを一覧表示中..."
	platformio device list

# プロジェクト情報の表示
info:
	@echo "プロジェクト情報:"
	platformio project config

# 特定環境用ビルド
build-esp32:
	platformio run -e esp32dev

# ポート指定アップロード
upload-port:
	@read -p "アップロードポートを入力 (例: /dev/ttyUSB0): " port; \
	platformio run --target upload --upload-port $$port

# ボーレート指定モニター
monitor-baud:
	@read -p "ボーレートを入力 (デフォルト 115200): " baud; \
	platformio device monitor -b $${baud:-115200}

# クリーンアップ後リビルド
rebuild: clean build

# フルサイクル: クリーンアップ、ビルド、アップロード、モニター
full: clean build upload monitor

# デバッグビルド
debug:
	@echo "デバッグフラグ付きでビルド中..."
	platformio run --target debug

# IDE用 compile_commands.json 生成
compiledb:
	@echo "compile_commands.json を生成中..."
	platformio run --target compiledb


# ヘルプ
help:
	@echo "利用可能なターゲット:"
	@echo "  make build        - プロジェクトのビルド"
	@echo "  make upload       - ESP32へのアップロード"
	@echo "  make monitor      - シリアルモニターの起動"
	@echo "  make flash        - アップロードとモニター"
	@echo "  make clean        - ビルドファイルのクリーンアップ"
	@echo "  make rebuild      - クリーンアップ後リビルド"
	@echo "  make full         - フルサイクル実行"
	@echo "  make test         - テストの実行(native)"
	@echo "  make setup        - 初期セットアップ"
	@echo "  make lib-install  - ライブラリのインストール"
	@echo "  make lib-update   - ライブラリのアップデート"
	@echo "  make check        - コードの検査"
	@echo "  make format       - コードフォーマット"
	@echo "  make devices      - 接続デバイスの一覧表示"
	@echo "  make info         - プロジェクト情報の表示"
	@echo "  make debug        - デバッグビルド"
	@echo "  make compiledb    - compile_commands.json生成"
	@echo "  make help         - このヘルプの表示"
