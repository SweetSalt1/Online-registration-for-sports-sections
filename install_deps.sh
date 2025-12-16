#!/bin/bash

echo "Обновление пакетов"
sudo apt update

echo "Установка зависимостей для сборки..."
sudo apt install -y build-essential cmake libboost-all-dev libmysqlcppconn-dev libmysqlclient-dev default-libmysqlclient-dev qt6-base-dev libssl-dev git libasio-dev libpsl-dev

# Определяем правильную рабочую директорию
WORK_DIR="$HOME/Desktop/zadania/tecn_prog/Online-registration-for-sports-sections"
cd "$WORK_DIR" || exit 1

echo "Установка Crow..."
git clone https://github.com/CrowCpp/Crow.git
cd Crow || exit 1
mkdir -p build && cd build || exit 1
cmake .. -DCROW_BUILD_EXAMPLES=OFF -DCROW_BUILD_TESTS=OFF
sudo make install
cd "$WORK_DIR" || exit 1

echo "Проверка установленных версий"
cmake --version
gcc --version
echo "Зависимости успешно установлены!"
