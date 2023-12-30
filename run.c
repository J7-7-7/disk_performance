#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <math.h>

double now() 
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

unsigned int xorbuf(unsigned int *buffer, int size) {
    unsigned int result = 0;
    for (int i = 0; i < size; i++) {
        result ^= buffer[i];
    }
    return result;
}

void write_file(const char *filename, unsigned int block_size, unsigned int block_count) 
{
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) 
    {
        perror("Error opening file for writing");
        return;
    }
    char *buffer = malloc(block_size);
    if (!buffer) 
    {
        perror("Memory allocation failed for write buffer");
        close(fd);
        return;
    }

    memset(buffer, 'A', block_size);  
    double start = now();

    for (int i = 0; i < block_count; ++i) 
    {
        if (write(fd, buffer, block_size) == -1) 
        {
            perror("Error writing to file");
            break;
        }
    }

    double end = now();
    double time_seconds = end - start;
    double total = block_size * (double)block_count;
    double performance = total / ((1024.0 * 1024.0) * time_seconds);

    printf("Write completed in %f seconds\n", time_seconds);
    printf("Performance: %f MiB/s\n", performance);

    free(buffer);
    close(fd);
}

void read_file(const char *filename, unsigned int block_size, unsigned int block_count) 
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

    if (st.st_size < (off_t)(block_size * block_count)) 
    {
        printf("Error: File size is insufficient for the requested read operation\n");
        close(fd);
        return;
    }

    unsigned int *buffer = malloc(block_size);
    if (!buffer) 
    {
        perror("Memory allocation failed for read buffer");
        close(fd);
        return;
    }

    double start = now();
    unsigned int xorValue = 0;

    for (int i = 0; i < block_count; ++i) 
    {
        int bytes_read = read(fd, buffer, block_size);
        if (bytes_read == -1) 
        {
            perror("Error reading from file");
            break;
        }
        xorValue ^= xorbuf(buffer, bytes_read / sizeof(unsigned int));
    }

    double end = now();
    double time_seconds = end - start;
    double totalBytesRead = block_size * (double)block_count;
    double performance = totalBytesRead / ((1024.0 * 1024.0) * time_seconds);

    printf("Read completed in %f seconds\n", time_seconds);
    printf("Performance: %f MiB/s\n", performance);
    printf("XOR Value: %08x\n", xorValue);

    free(buffer);
    close(fd);
}


int main(int argc, char *argv[]) 
{
    if (argc != 5) 
    {
        printf("Usage: %s <filename> [-r|-w] <block_size> <block_count>\n", argv[0]);
        return -1;
    }

    const char *filename = argv[1];
    const char *mode = argv[2];
    unsigned int block_size = strtoul(argv[3], NULL, 10);
    unsigned int block_count = strtoul(argv[4], NULL, 10);

    if (block_size < 1) 
    {
        printf("Invalid block size! Block size must be positive.\n");
        return -1;
    }

    if (block_size % 4 != 0) 
    {
        printf("Invalid block size! Block size should be a multiple of 4.\n");
        return -1;
    }

    if (block_count < 1) 
    {
        printf("Invalid block count! Block count must be positive.\n");
        return -1;
    }

    if (strcmp(mode, "-w") == 0) 
    {
        write_file(filename, block_size, block_count);
    } 
    else if (strcmp(mode, "-r") == 0) 
    {
        read_file(filename, block_size, block_count);
    } 
    else 
    {
        printf("Invalid mode. Use -r for read or -w for write.\n");
        return -1;
    }
    return 0;
}