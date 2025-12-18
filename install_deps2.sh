#!/bin/bash

# Определение языка системы
if [[ $LANG =~ ru_RU ]]  [[ $LANGUAGE =~ ru ]]  [[ $LANG =~ ru ]] || [[ $LANG =~ "ru" ]]; then
    LANG_RU=true
else
    LANG_RU=false
fi

# Функции для локализации
msg_update_packages() {
    if [ "$LANG_RU" = true ]; then
        echo "Обновление пакетов"
    else
        echo "Updating packages"
    fi
}

msg_install_deps() {
    if [ "$LANG_RU" = true ]; then
        echo "Установка зависимостей для сборки..."
    else
        echo "Installing build dependencies..."
    fi
}

msg_install_crow() {
    if [ "$LANG_RU" = true ]; then
        echo "Установка Crow..."
    else
        echo "Installing Crow..."
    fi
}

msg_check_versions() {
    if [ "$LANG_RU" = true ]; then
        echo "Проверка установленных версий"
    else
        echo "Checking installed versions"
    fi
}

msg_success() {
    if [ "$LANG_RU" = true ]; then
        echo "Зависимости успешно установлены!"
    else
        echo "Dependencies successfully installed!"
    fi
}

msg_error() {
    local dir="$1"
    if [ "$LANG_RU" = true ]; then
        echo "Ошибка: не удалось перейти в директорию $dir"
    else
        echo "Error: Failed to change directory to $dir"
    fi
}

msg_current_lang() {
    if [ "$LANG_RU" = true ]; then
        echo "Определен русский язык интерфейса"
    else
        echo "Detected English interface language"
    fi
}

# Выводим информацию о языке
msg_current_lang

# Обновление пакетов
msg_update_packages
sudo apt update

# Установка зависимостей
msg_install_deps
sudo apt install -y build-essential cmake libboost-all-dev libmysqlcppconn-dev \
    libmysqlclient-dev default-libmysqlclient-dev qt6-base-dev libssl-dev git \
    libasio-dev libpsl-dev

# Определяем правильную рабочую директорию
WORK_DIR="$HOME/Desktop/zadania/tecn_prog/Online-registration-for-sports-sections"
if ! cd "$WORK_DIR"; then
    msg_error "$WORK_DIR"
    exit 1
fi

# Установка Crow
msg_install_crow
if ! cd Crow; then
    msg_error "Crow"
    exit 1
fi

if ! mkdir -p build; then
    if [ "$LANG_RU" = true ]; then
        echo "Ошибка создания директории build"
    else
        echo "Error creating build directory"
    fi
    exit 1
fi

if ! cd build; then
    msg_error "build"
    exit 1
fi

cmake .. -DCROW_BUILD_EXAMPLES=OFF -DCROW_BUILD_TESTS=OFF
sudo make install

if ! cd "$WORK_DIR"; then
    msg_error "$WORK_DIR"
    exit 1
fi

# Проверка версий
msg_check_versions
cmake --version
gcc --version

msg_success
