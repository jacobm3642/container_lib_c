set -e

export LD_LIBRARY_PATH=./ 

s_flag='-shared -fPIC'
test_flags='-L./ -lcontainers -lcheck'

gcc $s_flag ./containers.c -o libcontainers.so
gcc $test_flags test.c -o test_main

./test_main