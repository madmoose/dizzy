gcc -o utils/check_whitespace utils/check_whitespace.c
find . \( -name '*.cpp' -or -name '*.h' \) ! -name '.*' -print0 | xargs -0 ./utils/check_whitespace
g++ main.cpp -g -I. analyzer/*.cpp analyzer/support/*.cpp binaries/*.cpp base/*.cpp x86/*.cpp
