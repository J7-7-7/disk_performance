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
    gettimeofday(&tv, 0);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

void find_optimal_block_count(const char *fileName, unsigned int blockSize) 
{
    struct stat st;
    if (stat(fileName, &st) == -1) 
    {
        perror("Error getting file size");
        return;
    }

    off_t fileSize = st.st_size;
    unsigned int blockCount = 1; 
    double time_seconds, speed;
    unsigned int xorValue;
    unsigned int *buffer = malloc(blockSize);

    if (!buffer) 
    {
        perror("Error: Malloc failed while allocating for buffer");
        return;
    }

    while (1) 
    {
        int fd = open(fileName, O_RDONLY);
        if (fd == -1) 
        {
            perror("Error opening file for reading");
            free(buffer);
            return;
        }

        xorValue = 0;
        unsigned long totalBytesRead = 0;

        double startTime = now();
        for (int i = 0; i < blockCount; i++) 
        {
            int bytesRead = read(fd, buffer, blockSize);
            if (bytesRead == -1) 
            {
                perror("Error reading from file");
                close(fd);
                free(buffer);
                return;
            }
            totalBytesRead += bytesRead;
            xorValue ^= xorbuf(buffer, bytesRead / sizeof(unsigned int));
        }
        double endTime = now();
        close(fd);

        time_seconds = endTime - startTime;
        speed = (totalBytesRead / (1024.0 * 1024.0)) / time_seconds; 

        if ((time_seconds >= 5 && time_seconds <= 15) || (totalBytesRead >= fileSize)) 
        {
            printf("Optimal Block Count: %u\n", blockCount);
            printf("XOR Value: %x\n", xorValue);
            printf("File Size: %f MB\n", fileSize / (1024.0 * 1024.0));
            printf("Speed: %f MiB/s\n", speed);
            if (time_seconds < 5) 
            {
                printf("Time to complete read: %f\n", time_seconds);
                printf("Note: Need a bigger file to run read for more than 5s. Please update Block Size\n");
            }
            break;
        } 
        else if (time_seconds < 5) 
        {
            blockCount *= 2; 
        } 
        else 
        { 
            blockCount = (blockCount > 1) ? blockCount / 2 : 1; 
        }
    }

    free(buffer);
}



int main(int argc, char **argv) 
{
    if (argc != 3) 
    {
        printf("Usage: ./run2 <file name> <block size>\n");
        return -1;
    }

    const char *fileName = argv[1];
    unsigned int blockSize = strtoul(argv[2], NULL, 10);

    if (blockSize <= 0) 
    {
        printf("Block size should be positive\n");
        return -1;
    }

    find_optimal_block_count(fileName, blockSize);
    return 0;
}