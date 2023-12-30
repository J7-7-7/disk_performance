#!/bin/bash
# Part 1
gcc -O3 run.c -o run
# ./run ubuntu-21.04-desktop-amd64.iso -r 1024 1000

# Part 2
gcc -O3 run2.c -o run2
# ./run2 ubuntu-21.04-desktop-amd64.iso 1024

# Part 3
gcc -O3 run3.c -o run3
# for ((c=128; c<=1048576; c=c*2)); do ./run3 ubuntu-21.04-desktop-amd64.iso $c ; done

# Part 4
gcc -O3 run3.c -o run3
# for ((c=128; c<=1048576; c=c*2)); do 
#    sudo sh -c "echo 3 > /proc/sys/vm/drop_caches" 
#    ./run3 ubuntu-21.04-desktop-amd64.iso $c
#  sleep 2
# done

# Part 5
gcc -O3 run5.c -o run5
# for ((count=1; count<=1048576; count=count*2)); do  ./run5 ubuntu-21.04-desktop-amd64.iso 1 $count; done

# Part 6
gcc -O3 -pthread fast.c -o fast
# ./fast ubuntu-21.04-desktop-amd64.iso 