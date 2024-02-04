# CMakeList.txt : CMake project for CMakeLuaTest, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("MV2LIN")


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
SET(CMAKE_CXX_FLAGS "-O3 -Wa,-adhln=main1.lst -fverbose-asm")
# -Wa,-adhln=main1.lst -fverbose-asm")

add_library(mv2lin SHARED "./src/libmv2lin"	"./src/kexpan.cpp"	"./src/mv2base.cpp"	"./src/mvl-aes.cpp"	"./src/RIJNDAEL.cpp" )
set_property(TARGET mv2lin PROPERTY POSITION_INDEPENDENT_CODE ON)

add_executable (MyConsole "./TestMv2Lin/TestSO.cpp")
#target_link_options(MyConsole -Wl,-rpath="./")
target_link_libraries(MyConsole mv2lin pthread)
