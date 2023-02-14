
/* direct_write.c

   Demonstrate the use of O_DIRECT to perform I/O bypassing the buffer cache
   ("direct I/O").

   Usage: direct_write file length [offset [alignment]]

   This program is Linux-specific.
*/
#define _GNU_SOURCE     /* Obtain O_DIRECT definition from <fcntl.h> */
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>

#include "./LRU/lru_cache.h"
#include "./LRU/lru_cache_impl.h"

#define CACHE_SIZE 50
#define VALUE_SIZE 4096 //4K
//#define KEY_SIZE  50

// cache size
//value size

int writecache(void *LruCache, char *key, char *value)
{
    if( 0 != LRUCacheSet(LruCache, key, value)){
        printf("put (%s, %s) failed!\n", *key, *value);
	return 0;
    }else{
    	printf("put (%s, %s) to cache success.\n", *key, *value);
	return 1;
    }
}

int readcache(void *LruCache, char *key, char *buff)
{
    
    if(NULL == LRUCacheGet(LruCache, key)){
        printf("%s is miss!\n", *key);
	return 0;
    }else{
        strncpy(buff, LRUcacheGet(LruCache, key), VALUE_SIZE);
	//if(!strncmp(buff, key, KEY_SIZE))
	printf("key %s 's value: %s", key, buff);
	return 1;
    }
}

//static char *generatestring(void *s, int ssize)
//{
//    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIGKLMNOPQRSTUVWXYZ";
//    if(ssize){
//        --ssize;
//	for(int n = 0; n < ssize; n++){
//	    int key = rand() % (int)(sizeof(charset) - 1);
//	    s[n] = charset[key];
//	}
//	s[ssize] = '\0';
//    }
//    return s;
//}

void handle_read()
{
}

int handle_write(void *LruCache, char *fname, int offset, int length, int size)
{
    //int ret;
    //char *s = malloc(VALUE_SIZE - 1);
    //if(s){
    //	generatestring(s, VALUE_SIZE - 2);
    //}
    char *buf;
    int ret;
    //size_t alignment;
    //alignment = 4096;

    ret = posix_memalign((void **)&buf, 512, length);
    if (ret){
        perror("posix_memalign failed");
        exit(-1);
    }

    memset(buf, 'c', length);

    ret = writecache(LruCache, key, buf)
    if( 0 == ret){
        //miss
	writetodisk(fname, offset, length, size);
	return 0;
    
    }else if (1 == ret){
    	return 0;
    }

    free(buf);
}


int writetodisk(char *fname, int offset, int length, int size, char *buf)
{
    int fd;
    ssize_t numWrite;
    //size_t length, alignment;
    //off_t offset;
    //char *buf;
    int ret;
    struct timespec start, end;
    float t_diff;


//    if (argc < 3 || strcmp(argv[1], "--help") == 0)
//        printf("%s file length [offset [alignment]]\n", argv[0]);
//
//    char *endptr;
//    length = strtol(argv[2], &endptr, 10);
//    offset = (argc > 3) ? strtol(argv[3], &endptr, 10) : 0;
    //alignment = 4096;

    //ret = posix_memalign((void **)&buf, 512, length);
    //if (ret){
    //    perror("posix_memalign failed");
    //    exit(-1);
    //}

    //memset(buf, 'c', length);

    clock_gettime(CLOCK_MONOTONIC, &start);

    fd = open(argv[1], O_WRONLY | O_DIRECT | O_NONBLOCK, 0600);
    if (fd == -1){
        perror("open input file error");
        exit(-1);
    }

    if (lseek(fd, offset, SEEK_SET) == -1){
        free(buf);
        perror("lseek error");
        exit(-1);
    }

    numWrite = write(fd, buf, length);

    clock_gettime(CLOCK_MONOTONIC, &end) ;

    if (numWrite == -1){
        perror("write error");
    }

    t_diff = ((end.tv_sec - start.tv_sec)*(1000*1000*1000) + (end.tv_nsec - start.tv_nsec)) / 1000.0 ;

    printf("Write %ld bytes, spent %.2f usec\n", (long) numWrite, t_diff);

    //free(buf);
    close(fd);
}


int main(int argc, char *argv[])
{
//    int fd;
//    ssize_t numWrite;
    size_t length; 
    int rw; // read = 0, write = 1;
    off_t offset;
//    char *buf;
    int ret;
    void *LruCache;
//    struct timespec start, end;
//    float t_diff;

    if (argc < 3 || strcmp(argv[1], "--help") == 0)
        printf("%s file length [offset [alignment]]\n", argv[0]);

    char *endptr;
    length = strtol(argv[2], &endptr, 10);
    offset = (argc > 3) ? strtol(argv[3], &endptr, 10) : 0;
    rw = (argc > 4) ? strtol(argv[4], &endptr, 10) : 1;

    if (0 != LRUCacheCreate(CACHE_SIZE, &LruCache)){
    	perror("LRUCacheCreate error!");
    }

    if(0 == rw){
    	ret = handle_read(LruCache, fname, offset, length, size);
    }else if(1 == rw){
        ret = handle_write(LruCache, fname, offset, length, size);
    }


//    ret = posix_memalign((void **)&buf, 512, length);
//    if (ret){
//        perror("posix_memalign failed");
//        exit(-1);
//    }    
//
//    memset(buf, 'c', length);
//
//    clock_gettime(CLOCK_MONOTONIC, &start);
//
//    fd = open(argv[1], O_WRONLY | O_DIRECT | O_NONBLOCK, 0600);
//    if (fd == -1){
//        perror("open input file error");
//	exit(-1);
//    }
//
//    if (lseek(fd, offset, SEEK_SET) == -1){
//	free(buf);
//        perror("lseek error");
//	exit(-1);
//    }
//  
//    numWrite = write(fd, buf, length);
//
//    clock_gettime(CLOCK_MONOTONIC, &end) ;
//
//    if (numWrite == -1){
//        perror("write error");
//    }
//
//    t_diff = ((end.tv_sec - start.tv_sec)*(1000*1000*1000) + (end.tv_nsec - start.tv_nsec)) / 1000.0 ;
//
//    printf("Write %ld bytes, spent %.2f usec\n", (long) numWrite, t_diff);
//
//    free(buf);
//    close(fd);
    exit(EXIT_SUCCESS);
}
