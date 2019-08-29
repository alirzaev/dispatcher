# Модель диспетчера задач операционной системы

[![Linux & macOS Build Status](https://img.shields.io/travis/alirzaev/dispatcher?label=Linux%20%26%20macOS%20Build%20Status)](https://travis-ci.org/alirzaev/dispatcher)
[![Windows Build Status](https://img.shields.io/appveyor/ci/alirzaev/dispatcher?label=Windows%20Build%20Status)](https://ci.appveyor.com/project/alirzaev/dispatcher)

Программная модель диспетчера задач для лабораторной работы по дисциплине "Операционные системы"

Руководство пользователя: https://alirzaev1996.gitbook.io/model-dispetchera-zadach-os/

# Структура проекта

- dispatcher.pro - Файл основного проекта

  - schedulers/schedulers.pro - Библиотека с алгоритмами работы диспетчера задач

  - tests/tests.pro - Тесты

  - generator/generator.pro - Библиотека для генерации заданий

  - gui/gui.pro - Программная модель с графическим интерфейсом

# Файл задания

Структуру файла задания можно найти [здесь](docs/TASK.md)

# Сборка

## Необходимые компоненты

### Windows

- Qt 5.11 или новее

- Visual Studio 2017 или новее со следующими компонентами:
  
  - Разработка классических приложений на C++

  - Debugging Tools for Windows

### macOS

- Qt 5.11 или новее

- Xcode 10 (Command Line Tools)

### Ubuntu

- Минимальная версия Ubuntu - 16.04

- Qt 5.11 или новее

- g++ 7 или новее

## Используемые сторонние библиотеки

- [Catch2 2.4.1](https://github.com/catchorg/Catch2)
- [nlohmann::json 3.6.1](https://github.com/nlohmann/json)
- [Mapbox Variant 1.1.6](https://github.com/mapbox/variant)
- [TartanLlama optional 1.0.0](https://github.com/TartanLlama/optional)

## Сборка

Проект собирается штатными средствами Qt: либо открываем `dispatcher.pro` через QtCreator, либо 
собираем с помощью qmake:

```
mkdir build
cd build
qmake ../dispatcher.pro
make
```

# В случае возникновения проблем

Задания, которые генерируются самой программой, сохраняются в файл `dispatcher.json` во временной папке пользователя. 
Открыть данную папку в стандартном файловом менеджере можно через меню программной модели: 
"Справка" > "Устранение неполадок" > "Открыть временную папку". 
При составлении [issue](https://github.com/alirzaev/dispatcher/issues), пожалуйста, прикрепите этот файл тоже.