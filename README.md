# Модель диспетчера задач операционной системы

Программная модель диспетчера задач для лабораторной работы по дисциплине "Операционные системы"

Руководство пользователя: [HTML](https://alirzaev.github.io/dispatcher/user-manual), [PDF](https://alirzaev.github.io/dispatcher/user-manual.pdf)

Загрузки: https://github.com/alirzaev/dispatcher/releases/latest

# Структура проекта

- qtutils - Библиотека со вспомогательными функциями

- schedulers - Библиотека с алгоритмами работы диспетчера задач

- tests - Тесты

- generator - Библиотека для генерации заданий

- dispatcher - Программная модель с графическим интерфейсом

- taskbuilder - Конструктор заданий

- widgets - Библиотека с UI-компонентами

# Файл задания

Структуру файла задания можно найти [здесь](docs/TASK.md)

# Сборка приложений

## Необходимые компоненты

### Windows

- CMake 3.10 или новее

- Qt 5.12 или новее

- Botan (криптографическая библиотека) 2.4 или новее

- Visual Studio 2017 или новее со следующими компонентами:

  - Разработка классических приложений на C++

  - Debugging Tools for Windows

vcpkg:

```
vcpkg install qt5-base botan
```

### macOS

- Минимальная версия macOS - 10.13

- CMake 3.10 или новее

- Qt 5.14 или новее

- Botan (криптографическая библиотека) 2.4 или новее

- Xcode 10 (Command Line Tools)

Homebrew:

```sh
brew install cmake qt botan
```

### Ubuntu

- Минимальная версия Ubuntu - 18.04

- CMake 3.10 или новее

- Qt 5.12 или новее ([ссылка на ppa-репозиторий](https://launchpad.net/~beineri))

- Botan (криптографическая библиотека) 2.4 или новее

- g++ 7 или новее

## Используемые сторонние библиотеки

- [Catch2 2.12.2](https://github.com/catchorg/Catch2)
- [nlohmann::json 3.7.3](https://github.com/nlohmann/json)
- [Mapbox Variant 1.1.6](https://github.com/mapbox/variant)
- [TartanLlama optional 1.0.0](https://github.com/TartanLlama/optional)

## Используемые сторонние шрифты

- [Microsoft Cascadia Code 1911.21](https://github.com/microsoft/cascadia-code)

## Сборка

Проект собирается штатными средствами Qt: либо открываем `CMkaeLists.txt` через QtCreator, либо
собираем с помощью CMake:

```sh
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH="<Qt root dir>/lib/cmake" -DDISPATCHER_DEBUG=1 ..
make
```

`DISPATCHER_DEBUG=1` - включение дополнительной отладочной информации.

# Сборка руководства пользователя

## Необходимые компоненты

- Asciidoctor 2.0 или новее

- Asciidoctor PDF 1.5 или новее (для сборки руководства в PDF)

- Pandoc 2.9 или новее (для сборки руководства в другие форматы)

## Сборка

```sh
cd docs/user-manual

# HTML

asciidoctor -o index.html index.adoc

# PDF

asciidoctor-pdf -a pdf-theme=theme.yml -o user-manual.pdf index.adoc

# Другие форматы (fb2, epub, docx и пр.)

asciidoctor -b docbook5 -o user-manual.docbook index.adoc
pandoc -f docbook -t <output format> -o <output file> index.docbook
```

# В случае возникновения проблем

Задания, которые генерируются самой программой, сохраняются в файл `dispatcher.json` во временной папке пользователя.
Открыть данную папку в стандартном файловом менеджере можно через меню программной модели:
"Справка" > "Устранение неполадок" > "Открыть временную папку".
При составлении [issue](https://github.com/alirzaev/dispatcher/issues), пожалуйста, прикрепите этот файл тоже.
