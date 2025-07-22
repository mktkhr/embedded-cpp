#!/bin/bash

GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

print_status() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

export PATH=$PATH:$HOME/.local/bin

if command -v platformio &> /dev/null; then
    print_status "PlatformIO インストール済み"
    print_status "PlatformIO version: $(platformio --version)"
else
    print_warning "PlatformIO が見つかりません, インストールします"
    pip install --user platformio
fi

print_status "PlatformIO を更新します"
platformio update

print_status "依存ライブラリをインストールします"
platformio lib install

print_status "接続済みのデバイスを検出します"
platformio device list
