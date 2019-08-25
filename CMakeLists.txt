cmake_minimum_required(VERSION 2.8)
project(HandMouse)

include(GNUInstallDirs)

# Сборка библиотеки
file(GLOB SOURCES Src/*.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/)

find_package(OpenCV REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})

target_compile_options(${PROJECT_NAME} PRIVATE -std=c++17 -Wall -Wextra -pedantic -Werror -Wno-unused-parameter)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

# Сборка исполняемого файла
# TODO

# Упаковка в пакет
install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}-targets DESTINATION bin)
export(TARGETS ${PROJECT_NAME} FILE ${PROJECT_NAME}-config.cmake)
install(EXPORT ${PROJECT_NAME}-targets DESTINATION ${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME} FILE ${PROJECT_NAME}-config.cmake)
export(PACKAGE ${PROJECT_NAME})

list(APPEND CPACK_GENERATOR TGZ)
set(CPACK_PACKAGE_CONTACT "dvorotnev")
include(CPack)
