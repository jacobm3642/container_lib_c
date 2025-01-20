set -e

export LD_LIBRARY_PATH=./ 

while getopts "dr" flag; do
 case $flag in
   d) 
   build_flags="-g -fno-inline -ggdb"
   ;;
   r) 
   build_flags="-O3 -s -ftree-vectorize -funroll-loops -flto"
   ;;
   \?)
   echo "invald flag passed"
   exit 1
   ;;
 esac
done

s_flag='-s -std=c99 -shared -fPIC -g -Wall -Werror -Wextra'
test_flags='-L./ -lcontainers -lcheck'

gcc $s_flag $build_flags ./containers.c -o libcontainers.so
gcc $test_flags $build_flags test.c -o test_main

./test_main
