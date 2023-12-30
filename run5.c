#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>


double now() 
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + (tv.tv_usec / 1000000.0);
}


void measure_performance(const char *filename, unsigned int block_size, unsigned int block_count) 
{
    int fd = open(filename, O_RDONLY); 

    if (fd == -1) 
    {
        perror("Error opening file for reading");
        return;
    }

    unsigned int *buffer = (unsigned int *)malloc(block_size);
    if (buffer == NULL) 
    {
        perror("Memory allocation failed for buffer");
        close(fd);
        return;
    }

    int bytes_read;
    unsigned long total_bytes_read = 0;
    double read_start_time = now();


    for (int i = 0; i < block_count; i++) 
    {
        bytes_read = read(fd, buffer, block_size);
        if (bytes_read == -1) 
        {
            perror("Error reading from file");
            break;
        }
        total_bytes_read += bytes_read;
    }


    double read_end_time = now();
    double time_seconds = read_end_time - read_start_time;
    double performance_MiB_s = (total_bytes_read / (1024.0 * 1024.0)) / time_seconds; 
    double performance_B_s = total_bytes_read / time_seconds; 

    printf("Read - Block Count: %u, Performance: %.5f MiB/s, Performance: %.5f B/s\n", block_count, performance_MiB_s, performance_B_s);

    
    double start_time = now();
    
    for(int i=0;i<block_count;i++)
    {
        lseek(fd, block_count, SEEK_SET);
        bytes_read = read(fd, buffer, block_size);
    }


    double end_time = now();
    time_seconds = end_time - start_time;
    performance_MiB_s = ((block_size * block_count) / (1024.0 * 1024.0)) / time_seconds; 
    performance_B_s = block_count / time_seconds; 

    printf("lseek - Block Count: %u, Performance: %.5f MiB/s, Performance: %.5f B/s\n\n", block_count, performance_MiB_s, performance_B_s);

    free(buffer);
    close(fd);
}


int main(int argc, char *argv[]) 
{
    if (argc != 4) 
    {
        fprintf(stderr, "Usage: %s <filename> <block_size> <block_count>\n", argv[0]);
        return -1;
    }

    const char *filename = argv[1];
    unsigned int block_size = strtoul(argv[2], NULL, 10);
    unsigned int block_count = strtoul(argv[3], NULL, 10);

    if (block_size == 0 || block_count == 0) 
    {
        fprintf(stderr, "Block size and block count must be positive.\n");
        return -1;
    }

    measure_performance(filename, block_size, block_count);
    return 0;
}