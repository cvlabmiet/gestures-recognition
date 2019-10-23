include(ExternalProject)

FIND_PACKAGE(PythonLibs 3 REQUIRED)

add_compile_options(-Wno-unused-function -Wno-unused-variable)

file(GLOB SOURCES *.cpp)
PYTHON_ADD_MODULE(${PROJECT_NAME}_py MODULE ${SOURCES})

find_package(Boost COMPONENTS system python3 REQUIRED)
target_include_directories(${PROJECT_NAME}_py PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${PYTHON_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME}_py PUBLIC ${Boost_LIBRARIES} ${PROJECT_NAME}-lib)

ExternalProject_Add(pyboostcvconverter
                    URL https://api.github.com/repos/Algomorph/pyboostcvconverter/tarball/3338a4c
                    URL_HASH MD5=b02287f63b4ab543e1277de499da87b3
                    CMAKE_ARGS "-DCMAKE_BUILD_TYPE=Release;-DPYTHON_DESIRED_VERSION=3.X"
                    INSTALL_COMMAND ""
)

add_dependencies(${PROJECT_NAME}_py pyboostcvconverter)
ExternalProject_Get_Property(pyboostcvconverter SOURCE_DIR)
ExternalProject_Get_Property(pyboostcvconverter BINARY_DIR)

add_library(pyboostcvconverter_lib SHARED IMPORTED)
set_property(TARGET pyboostcvconverter_lib PROPERTY IMPORTED_LOCATION ${BINARY_DIR}/pbcvt.cpython-36m-x86_64-linux-gnu.so)
target_include_directories(${PROJECT_NAME}_py PUBLIC ${SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME}_py PUBLIC pyboostcvconverter_lib)

find_package(OpenCV REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${OpenCV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME}_py PUBLIC ${OpenCV_LIBS})
