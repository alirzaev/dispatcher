# Модель диспетчера задач операционной системы

[![Build Status](https://travis-ci.org/alirzaev/dispatcher.svg?branch=master)](https://travis-ci.org/alirzaev/dispatcher)
[![Build status](https://ci.appveyor.com/api/projects/status/k043jd35wesnah6b?svg=true)](https://ci.appveyor.com/project/alirzaev/dispatcher)

Программная модель диспетчера задач для лабораторной работы по дисциплине "Операционные системы"

# Сборка и запуск

## Зависимости

- Qt 5.11+
- g++ 7+ или любой другой компилятор, совместимый с Qt и имеющий поддержку c++17

## Используемые сторонние библиотеки

- [Catch2 2.4.1](https://github.com/catchorg/Catch2)
- [nlohmann::json 3.3.0](https://github.com/nlohmann/json)

## Сборка

Графический интерфейс программной модели находится в папке `gui`. Сборка проекта выполняется стандартными средаствами (qmake или QtCreator).

Алгоритмы работы диспетчера памяти находятся в папке `schedulers`.

Генератор заданий находится в папке `generator`.
