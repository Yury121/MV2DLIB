rm -r CMakeFiles
rm CMakeCache.txt
rm -r out
cmake -DCMAKE_BUILD_TYPE=Release -S . -B ./out/build
cd ./out/build
make -B
cd ../../
cd ./out/build