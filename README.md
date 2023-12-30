# disk_performance

# Goal
The project is centered around performance.

We will try to get disk I/O as fast as possible and evaluate the effects of caches and the cost of system calls. In the end you should have a good understanding of what sits in the way between your process requesting data from disk and receiving it.

# Breakdown
## 1. Basics
Write a program that can read and write a file from disk using the standard C/C++ library's open, read, write, and close functions.
Add parameter for the file name;
Add parameter for how big the file should be (for writing);
Add a parameter to specify how much to read with a single call (block size);
Way to execute: ./run <filename> [-r|-w] <block_size> <block_count>

Expected output: For write (-w flag) a new file should be created with name as . The content written to the file can be any garbage value and the size of the file should be block_size * block_count bytes. Print the performance of write the file in MBs/sec or bytes/sec.

For read (-r flag) your code should read file from the start and the number of bytes read should be equal to block_size * block_count. You will need to XOR the content read and print it to the terminal. Along with the XOR value print the performance in MiB/s or bytes/sec.


## 2. Measurement
When measuring things it helps if they run for "reasonable" time. It is hard to measure things that run too fast as you need high-precision clocks and a lot of other things can affect the measurement. It is also annoying to wait for a long time for an experiment, especially if you have to do many experiments. For this reason you should make sure that your experiments take "reasonable" time. I recommend something between 5 and 15 seconds.

Write a program to find a file size which can be read in "reasonable" time.
Input: block size
Output: file size
Hint: You can start with a small file size and use your program from (1) above to read it and measure the time it takes. You can keep doubling your file size until you get between 5 and 15 seconds.
How to run : ./run2 <filename> <block_size>

Expected Output: Given the file and block_size output the block_count for which the time taken is between 5 and 15. The output for this case should be the XOR value, the file size for which the read time is between 5 and 15 sec, and the performance of reading the file in MiB/s or bytes/sec.

## 3. Raw Performance
Because you would be looking at different file sizes when using different block sizes, it makes sense to use units for performance that are independent of the size. Use MiB/s (this is megabytes per second).

Make your program output the performance it achieved in MiB/s
Make a graph that shows its performance as you change the block size.
block size on the x axis and performance on the y axis.
We will not mandate a way to run this... anything that works for you to produce the graphs.

Expected Output: In this case you will read the entire file by varying the block size. Plot a graph in your report with block size on the x axis and performance on the y axis.

## 4. Caching
Once you have a file of "reasonable" size created, reboot your machine.

Call your program to read the file and note the performance.

Call your program to read the file again (immediately after) and note the performance.

Ideally you should observe the second read be much faster, assuming the file can fit in your physical memory. This is the effect of caching.

NOTE: On Linux there is a way to clear the disk caches without rebooting your machine. E.g. `sudo sh -c "/usr/bin/echo 3 > /proc/sys/vm/drop_caches"`.
Extra credit: Why "3"? Read up on it and explain.

Experiment with clearing the caches and not.

Make a graph that shows performance for cached and non-cached reads for various block sizes.
We will not mandate a way to run this... anything that works for you to produce the graphs.

Expected Output: Same as 3, just add the performance reading for cached file. The file blocks can be cached by reading the file multiple times.


## 5. System Calls
If you have very, very small block size (e.g. 1 byte), most of the time would be spent trapping into the kernel. We talked in class how system calls are expensive. Let's quantify it!

Measure performance MiB/s when using block size of 1 byte
Measure performance in B/s. This is how many system calls you can do per second.
Try with other system calls that arguably do even less real work (e.g. lseek)
We will not mandate a way to run this... anything that works for you to produce the graphs.

Expected Output: A graph comparing the read system call and other system call that does relatively less work like lseek. You need to plot a graph with number of system calls on the y-axis and time taken on x-axis.

## 6. Raw Performance
Try to optimize your program as much as you can to run as fast as it could.

Find a good enough block size?
Use multiple threads?
Report both cached and non-cached performance numbers.

