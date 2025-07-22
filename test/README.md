# テストディレクトリ

このディレクトリにはC++単体テストが含まれています。

## テスト実行方法

```bash
# 全テスト実行（ネイティブ環境）
pio test -e native

# 特定のテストのみ実行
pio test -e native -f test_sensor_utils

# テストの詳細出力
pio test -e native -v
```

## ディレクトリ構成

```
test/
├── common/                    # 共通テストヘルパー
│   └── arduino_mock.h         # Arduino.hのモック実装
├── test_sensor_utils/         # センサー関連テスト
│   ├── sensor_functions.h     # テスト対象関数実装
│   └── test_sensor_calculations.cpp
├── test_storage/              # ストレージ関連テスト
│   ├── storage_functions.h    # Mock Preferences実装
│   └── test_preferences.cpp
└── README.md                  # このファイル
```

## テスト構成

- `common/` - 全テストで共有するヘルパー関数とMock
- `test_sensor_utils/` - センサー関連のユーティリティ関数テスト (12テスト)
- `test_storage/` - ストレージ関連関数テスト (12テスト)

## テストフレームワーク

- **Unity**: C言語向けテストフレームワーク
- **PlatformIO Test Runner**: テスト実行環境
- **Custom Arduino Mock**: native環境での Arduino.h 代替実装

## 新しいテストの追加方法

1. `test/test_新機能/` ディレクトリを作成
2. `#include "../common/arduino_mock.h"` でMockを使用
3. 必要に応じて追加のMockを `common/` に配置
4. Unity テストフレームワークでテストケースを記述