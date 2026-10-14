#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

#define BUFFERLEN 100

int main(int argc, char* argv[]){

    if (argc < 2){
        printf("Must have at least 2 arguments");
    }

    int fd = open("test.txt", O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG);
    
    if (fd == -1){
        perror("Unable to open test.txt");
    }

    char buffer[BUFFERLEN];

    ssize_t bytes = read(fd, buffer, 10);

    if (bytes == 0){
        printf("Encountered EOF");
    }

    for (int i = 1; i < argc; i++){
        ssize_t bytes_written = write(fd, argv[i], strlen(argv[i]));

        if (bytes_written == -1){
            perror("Error in writing to test.txt");
        }
        printf("Wrote %zd bytes", bytes_written);
    }

    close(fd);
    return 0;
}
