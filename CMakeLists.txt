cmake_minimum_required(VERSION 3.0)
project(HandMouse VERSION 0.0.1)

include(GNUInstallDirs)
set (CMAKE_CXX_STANDARD 17)
add_compile_options(-std=c++17 -Wall -Wextra -pedantic -Werror -Wno-unused-parameter -fPIC)

# Сборка библиотеки
file(GLOB SOURCES src/*.cpp)
add_library(${PROJECT_NAME}-lib ${SOURCES})
target_include_directories(${PROJECT_NAME}-lib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>
)

find_package(OpenCV REQUIRED)
target_include_directories(${PROJECT_NAME}-lib PUBLIC ${OpenCV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME}-lib PUBLIC ${OpenCV_LIBS})

# Сборка исполняемого файла
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}-lib)

# Сборка модуля python
add_subdirectory(python)

# Упаковка в пакет
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})
install(TARGETS ${PROJECT_NAME}-lib ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
export(TARGETS ${PROJECT_NAME}-lib ${PROJECT_NAME} FILE ${PROJECT_NAME}-config.cmake)
install(EXPORT ${PROJECT_NAME}-targets DESTINATION ${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME} FILE ${PROJECT_NAME}-config.cmake)
export(PACKAGE ${PROJECT_NAME})

list(APPEND CPACK_GENERATOR TGZ)
set(CPACK_PACKAGE_CONTACT "dvorotnev")
include(CPack)
