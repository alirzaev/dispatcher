# Модель диспетчера задач операционной системы

[![Build Status](https://travis-ci.org/alirzaev/dispatcher.svg?branch=master)](https://travis-ci.org/alirzaev/dispatcher)
[![Build status](https://ci.appveyor.com/api/projects/status/k043jd35wesnah6b?svg=true)](https://ci.appveyor.com/project/alirzaev/dispatcher)

Программная модель диспетчера задач для лабораторной работы по дисциплине "Операционные системы"

# Структура проекта

- dispatcher.pro - Файл основного проекта

  - schedulers - Библиотека с алгоритмами работы диспетчера задач

  - tests - Тесты

  - generator - Библиотека для генерации заданий

  - gui - Собственно программная модель с GUI

# Файл задания

Структуру файла задания можно найти [здесь](https://github.com/alirzaev/dispatcher/blob/master/docs/TASK.md)

# Сборка

## Необходимые компоненты

- Qt (>= 5.11)
- G++ (>= 7) или Visual C++ (>= 2017; короче, Visual Studio 2017)

## Используемые сторонние библиотеки

- [Catch2 2.4.1](https://github.com/catchorg/Catch2)
- [nlohmann::json 3.3.0](https://github.com/nlohmann/json)

## Сборка

Проект собирается штатными средствами Qt: либо открываем `dispatcher.pro` через QtCreator, либо 
собираем с помощью qmake.