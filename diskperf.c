#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define BLOCKSIZE   512         // In bytes
#define FILESIZE    (2000*100)  // In blocks
#define NS_PER_MS   (1000*1000)
#define MS_PER_S    1000

/* 
 * Sequentially write 'blocks' BLOCKSIZE blocks to 'file' starting from the 
 * current end of the file. Return the number of blocks added, or -1 on failure.
 */
int growFile(int file, int blocks) {
    char data[BLOCKSIZE];
    if (lseek(file, 0, SEEK_END) < 0) {
        printf("lseek failed: %s\n", strerror(errno));
        return -1;
    }
    for (int i = 0; i < blocks; i++) {
        if (write(file, data, BLOCKSIZE) < BLOCKSIZE) {
            printf("write failed: %s\n", strerror(errno));
            return -1;
        }
    }
    if (fsync(file) < 0) {
        printf("fsync failed: %s\n", strerror(errno));
        return -1;
    }
    return blocks;
}

/* 
 * Sequentially read 'blocks' BLOCKSIZE blocks from 'file' starting at the
 * first block. Return the number of blocks read, or -1 on failure.
 */
int sequentialRead(int file, int blocks) {
    char buffer[BLOCKSIZE*blocks] __attribute__ ((__aligned__ (BLOCKSIZE)));
    // TODO
    return -1;
}

/* 
 * Randomly read 'blocks' BLOCKSIZE blocks from 'file'. Return the number of 
 * blocks read, or -1 on failure.
 */
int randomRead(int file, int blocks) {
    char buffer[BLOCKSIZE] __attribute__ ((__aligned__ (BLOCKSIZE)));
    // TODO
    return -1;
}

/* 
 * Sequentially write 'blocks' BLOCKSIZE blocks from 'file' starting at the
 * first block. Return the number of blocks read, or -1 on failure.
 */
int sequentialWrite(int file, int blocks) {
    char data[BLOCKSIZE*blocks] __attribute__ ((__aligned__ (BLOCKSIZE)));
    // TODO
    return -1;
}

/* 
 * Randomly write 'blocks' BLOCKSIZE blocks from 'file'. Return the number of 
 * blocks read, or -1 on failure.
 */
int randomWrite(int file, int blocks) {
    char data[BLOCKSIZE] __attribute__ ((__aligned__ (BLOCKSIZE)));
    // TODO
    return -1;
}

/*
 * Run a single 'testcase' that performs some type of operation on 'blocks' 
 * BOCKSIZE blocks from 'file'. Return the time in milliseconds taken to
 * complete the test.
 */
long timedTest(int(*testcase)(int , int), int file, int blocks) {
    struct timespec startTime, endTime;

    // Run test
    clock_gettime(CLOCK_MONOTONIC, &startTime);
    if ((*testcase)(file, blocks) < 0) {
        return -1;
    }
    clock_gettime(CLOCK_MONOTONIC, &endTime);

    // Compute elapsed time
    long s = endTime.tv_sec - startTime.tv_sec;
    long ns = endTime.tv_nsec - startTime.tv_nsec;
    return (s * MS_PER_S) + (ns / NS_PER_MS);
}

int main(int argc, char *argv[]) {
    // Obtain user-provided arguments
    if (argc != 3) {
        printf("Usage: %s <filepath> <testsize>\n", argv[0]);
        return 1;
    }
    char *path = argv[1];
    int testsize = atoi(argv[2]);

    if (testsize < 1 || testsize > FILESIZE) {
       printf("Test size must be between 1 and %d blocks\n", FILESIZE);
       return 1; 
    }

    // Open file
    int fd = open(path, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
    if (fd < 0) {
        printf("Failed to open file %s: %s\n", path, strerror(errno));
        return 1;
    }
   
    // Grow file 
    int currSize = lseek(fd, 0, SEEK_END);
    if (currSize < FILESIZE * BLOCKSIZE) {
        int growth = ((FILESIZE * BLOCKSIZE) - currSize) / BLOCKSIZE;
        if (growFile(fd, growth) < growth) {
            printf("Failed to grow file by %d blocks\n", FILESIZE);
            return 1;
        } else {
            printf("Created file with %d blocks\n", FILESIZE);
        }
    }
    
    // Close file
    close(fd);

    // Open file
    fd = open(path, O_RDWR|O_DIRECT|O_SYNC);
    if (fd < 0) {
        printf("Failed to open file %s: %s\n", path, strerror(errno));
        return 1;
    }

    // Measure sequential read performance
    long seqRead = timedTest(&sequentialRead, fd, testsize);
    if (seqRead < 0) {
        printf("Sequential read test failed\n");
    } else {
        printf("Sequential read of %d blocks took %ld ms\n", testsize, seqRead);
    }

    // Measure random read performance
    long randRead = timedTest(&randomRead, fd, testsize);
    if (randRead < 0) {
        printf("Random read test failed\n");
    } else {
        printf("Random read of %d blocks took %ld ms\n", testsize, randRead);
    }

    // Measure sequential write performance
    long seqWrite = timedTest(&sequentialWrite, fd, testsize);
    if (seqWrite < 0) {
        printf("Sequential write test failed\n");
    } else {
        printf("Sequential write of %d blocks took %ld ms\n", testsize, 
                seqWrite);
    }

    // Measure random write performance
    long randWrite = timedTest(&randomWrite, fd, testsize);
    if (randWrite < 0) {
        printf("Random write test failed\n");
    } else {
        printf("Random write of %d blocks took %ld ms\n", testsize, 
                randWrite);
    }

    // Close file
    close(fd);

    return 0;
}
