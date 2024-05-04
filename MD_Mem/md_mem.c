#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

#define MB (1024*1024)


///Vajag 100mb alocet un atrast laiku 
///Max daudzums ko var alocet ar funkcijam 
/// malloc()   |   mmap()    |   sbrk()

void allocateMalloc100mb(){
int *ptr;
    int i;

    for (i = 0; i < 100; i++) {
        ptr = malloc(MB);

        if (ptr == NULL) {
            printf("Memory allocation failed.\n");
            exit(0);
        }

    }
    sleep(60);
}

void MaxMalloc(){
void *p;
unsigned long long int  inc = MB; // Start with 1MB
unsigned long long int max_allocated = 0;

    while (1) {
        p = (int*)malloc(inc);
        if (p == NULL) {
            break;
        }
        max_allocated = inc; // Update max_allocated
        inc += MB; // Increment by 1MB
        free(p);
    }

    printf("Maximum memory allocated: %lld MB\n", max_allocated/MB);

}


/*void allocateMmap100mb(){

    for (int i = 0; i < 100; i++) {
        void *ptr = mmap(NULL, MB, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (ptr == MAP_FAILED) {
            perror("mmap failed");
            exit(EXIT_FAILURE);
        }
    }
    sleep(60);
}

void MaxMmap(){
unsigned long long int max_allocated = 0;
unsigned long long int inc = MB;
    void *ptr;

    while (1) {
        ptr = mmap(NULL, inc, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (ptr == MAP_FAILED) {
            // Allocation failed, exit the loop
            break;
        }
        max_allocated += MB;
        inc+=MB;
    }

    // Free the last allocation
    munmap(ptr, MB);

    printf("Maximum memory allocated with mmap: %lld MB\n", max_allocated / MB);

}*/

/*void allocateSbrk100mb(){
    int mb = 100;
    int total_allocated = 0;

    while (total_allocated < MB*100) {
        // Increase the program's data space by 1MB
        void *mem = sbrk(MB);
        if (mem == (void*) -1) {
            perror("sbrk");
            exit(EXIT_FAILURE);
        }
        total_allocated += MB;
    }
    sleep(60);

}

void MaxSbrk(){
void *ptr;
unsigned long long int max_allocated = 0;
unsigned long long int inc = MB;
 while (1) {
        // Increase the program's data space by 1MB
        void *mem = sbrk(inc);
        if (mem == (void*) -1) {
            break;
        }

        max_allocated += MB;
        inc+=MB;
    }

    printf("Maximum memory allocated with sbrk: %lld MB\n", max_allocated / MB);


}*/

int main(int argc, char **argv){


if(argc != 2){
    printf("Argument error!\n");
    return 1;
}


if(strcmp(argv[1],"malloc") == 0){
    allocateMalloc100mb();
}
else if(strcmp(argv[1],"mallocmax") == 0){
    MaxMalloc();
}

/*else if(strcmp(argv[1],"mmap") == 0){
    allocateMmap100mb();
}
else if(strcmp(argv[1],"mmapmax") == 0){
    MaxMmap();
}

else if(strcmp(argv[1],"sbrk") == 0){
    allocateSbrk100mb();
}
else if(strcmp(argv[1],"sbrkmax") == 0){
    MaxSbrk();
}*/


return 0;
}
