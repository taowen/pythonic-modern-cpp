cd build
cmake -D CMAKE_C_COMPILER=clang -D CMAKE_CXX_COMPILER=clang++ -D USE_MODULES=on ../
make string -j 8 && ./string -d yes
