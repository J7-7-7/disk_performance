#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <string.h>
#include <sys/stat.h>

unsigned int xorbuf(unsigned int *buffer, int size) 
{
    unsigned int result = 0;
    for (int i = 0; i < size; i++) 
    {
        result ^= buffer[i];
    }
    return result;
}

double now() 
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

void read_and_measure_performance(const char *filename, unsigned int block_size) 
{
    int fd = open(filename, O_RDONLY); 
    if (fd == -1) 
    {
        perror("Error opening file for reading");
        return;
    }

    struct stat st;
    if (stat(filename, &st) == -1) 
    {
        perror("Error getting file size");
        close(fd);
        return;
    }

    off_t file_size = st.st_size;
    unsigned long block_count = file_size / block_size;
    if (file_size % block_size != 0) 
    {
        block_count += 1; 
    }

    unsigned int *buffer = (unsigned int *)malloc(block_size);
    if (buffer == NULL) 
    {
        perror("Memory allocation failed for buffer");
        close(fd);
        return;
    }

    unsigned int xor_value = 0;
    unsigned long total_bytes_read = 0;
    double start_time = now();

    for (unsigned int i = 0; i < block_count; i++) 
    {
        int bytes_read = read(fd, buffer, block_size);
        if (bytes_read == -1) 
        {
            perror("Error reading from file");
            break;
        }
        xor_value ^= xorbuf(buffer, bytes_read / sizeof(unsigned int));
        total_bytes_read += bytes_read;
    }

    double end_time = now();
    close(fd);

    double time_seconds = end_time - start_time;
    double performance = (total_bytes_read / (1024.0 * 1024.0)) / time_seconds;

    printf("Read performance for block size %u bytes: %.5f MiB/s\n", block_size, performance);
    printf("XOR Checksum: %08x\n", xor_value);

    free(buffer);
}


int main(int argc, char *argv[]) 
{
    if (argc != 3) 
    {
        fprintf(stderr, "Usage: %s <filename> <block_size>\n", argv[0]);
        return -1;
    }

    const char *filename = argv[1];
    unsigned int block_size = strtoul(argv[2], NULL, 10);

    if (block_size == 0) 
    {
        fprintf(stderr, "Invalid block size! Block size must be positive.\n");
        return -1;
    }

    
    read_and_measure_performance(filename, block_size);
    return 0;
}