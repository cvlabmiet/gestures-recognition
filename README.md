# HandMouse

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
