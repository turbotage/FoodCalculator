# CMakeList.txt : CMake project for NextGengine, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.12)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add source to this project's executable.
set(SOURCES "src/FoodCalculator.cpp" "src/FoodCalculator.h" "src/GenericVector.cpp" "src/GenericVector.h")

add_executable(FoodCalculator ${SOURCES})

#matplotlib
target_include_directories(FoodCalculator PRIVATE "lib/matplotlib-cpp/include")
find_package(Python3 COMPONENTS Development NumPy)
target_include_directories(FoodCalculator PRIVATE ${Python3_INCLUDE_DIRS} ${Python3_NumPy_INCLUDE_DIRS})
target_link_libraries(FoodCalculator Python3::Python Python3::NumPy)

#rapiccsv
target_include_directories(FoodCalculator PRIVATE "lib/rapidcsv/src")