# HandMouse

[![Build Status](https://travis-ci.com/cvlabmiet/gestures-recognition.svg?branch=master)](https://travis-ci.com/cvlabmiet/gestures-recognition)

[![GitHub Releases](https://img.shields.io/github/downloads/cvlabmiet/gestures-recognition/latest/total?style=plastic)](https://github.com/cvlabmiet/gestures-recognition/releases/latest)

![GitHub Release Date](https://img.shields.io/github/release-date/cvlabmiet/gestures-recognition)

## Linux: Установка пакета

Сборка пакета из исходников:

```sh
$ mkdir build && cd build
$ cmake -DCPACK_GENERATOR=DEB ..
$ make package
```

Установка собранного пакета:

```
$ dpkg -i HandMouse-version.deb
```

## Подключение через cmake ExternalProject

```cmake
cmake_minimum_required(3.10.3)
include(ExternalProject)

ExternalProject_Add(HandMouse
    URL https://api.github.com/repos/gestures-recognition/tarball/master
    URL_HASH #sha256sum
    )
```
