#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <pthread.h>

typedef struct 
{
    int fd;
    int blockSize;
    off_t offset;
    unsigned int maxBlockCount;
    unsigned int xor_val;
} ThreadData;


double now() 
{
    struct timeval tv;
    gettimeofday(&tv, 0);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

unsigned int XOR_VALUE = 0;

void *read_file(void *arg) 
{
    ThreadData *data = (ThreadData *)arg;
    unsigned int *buffer = (unsigned int *)malloc(data->blockSize);
    if (!buffer) 
    {
        perror("Failed to allocate memory for buffer");
        pthread_exit(NULL);
    }

    off_t offset = data->offset;
    unsigned int xor_val = 0;
    for (int i = 0; i < data->maxBlockCount; i++) 
    {
        unsigned int bytes_read = pread(data->fd, buffer, data->blockSize, offset);
        if (bytes_read == -1) 
        {
            perror("Error reading from file");
            break;
        }

        for (int j = 0; j < (bytes_read / sizeof(unsigned int)); j++) 
        {
            xor_val ^= buffer[j];
        }
        offset += data->blockSize;
    }
    data->xor_val = xor_val;

    free(buffer);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) 
{
    if (argc != 2) 
    {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return -1;
    }

    const char *fileName = argv[1];
    int threadCount = 16; 
    int blockSize = 1048576; 

    int fd = open(fileName, O_RDONLY);
    if (fd == -1) 
    {
        perror("Error opening file");
        return -1;
    }

    struct stat st;
    if (stat(fileName, &st) == -1) 
    {
        perror("Error getting file size");
        close(fd);
        return -1;
    }

    off_t fileSize = st.st_size;
    pthread_t threads[threadCount];
    ThreadData threadData[threadCount];

    int maxBlockCount = fileSize / blockSize;
    if (fileSize % blockSize != 0) 
    {
        maxBlockCount++;
    }
    
    int blocksPerThread = maxBlockCount / threadCount;
    int remainingBlocks = maxBlockCount % threadCount;

    double startTime = now();
    for (int i = 0; i < threadCount; i++) 
    {
        threadData[i].fd = fd;
        threadData[i].blockSize = blockSize;
        threadData[i].offset = (off_t)i * blocksPerThread * blockSize;
        threadData[i].maxBlockCount = (i == threadCount - 1) ? (blocksPerThread + remainingBlocks) : blocksPerThread;
        threadData[i].xor_val = 0;

        pthread_create(&threads[i], NULL, read_file, &threadData[i]);
    }

    for (int i = 0; i < threadCount; i++) 
    {
        pthread_join(threads[i], NULL);
        XOR_VALUE ^= threadData[i].xor_val;
    }

    double endTime = now();
    double speed = fileSize / ((1024.0 * 1024.0) * (endTime - startTime));

    close(fd);
    printf("Performance: %.4f MiB/s, XOR Value: %08x\n", speed, XOR_VALUE);
    return 0;
}