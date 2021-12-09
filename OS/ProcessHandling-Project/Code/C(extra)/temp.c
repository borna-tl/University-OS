#include <stdio.h>
#include <dirent.h> 
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>


#define MSGSIZE 1024
#define MAX_FILE_NO 9
  
int main()
{
    char inbuf[MSGSIZE];
    int p[2], pid[MAX_FILE_NO], nbytes;
    //char* fd[MAX_FILE_NO]; 
    if (pipe(p) < 0)
        exit(1);
    pid[0] = fork();
    int MAIN_PROCESS_ID = getpid();
    //printf("MAIN P ID IS: %d", MAIN_PROCESS_ID);
    if (pid[0] > 0){ //map processes
        close(p[1]); //close the write descriptor
        DIR *d;
        struct dirent *dir;
        d = opendir("./testcases");
        if (d) {
            while ((dir = readdir(d)) != NULL) {//iterating on all files
                char type[5];
                char* file_name = dir->d_name;
                int position = strlen(dir->d_name) - 5;
                if (position < 0)
                    continue;
                for (int i = 0; i < 5; i++)
                    type[i] = file_name[position + i];
                if (type[1] == '.' && type[2] == 'c' && type[3] == 's' && type[4] == 'v'){
                    if (getpid() != MAIN_PROCESS_ID)
                        continue;
                    //printf("222 MAIN P ID IS: %d AND %d", MAIN_PROCESS_ID, getpid());
                    char * my_fifo = "/tmp/my_fifo";
                    printf("Found a new .csv file: %s\n", dir->d_name);
                    int file_index = type[0] - '0';
                    mkfifo(my_fifo, 0666);
                    pid[file_index] = fork();
                    if (pid[file_index] > 0){ //main
                        // Open FIFO for write only
                        int new_fd = open(my_fifo, O_WRONLY);
                    
                        // Write the id on FIFO and close it
                        char str[MSGSIZE + 1];
                        sprintf(str, "%d", file_index);
                       
                        write(new_fd, str, MSGSIZE);
                        close(new_fd);
                        
                        // close(fd[file_index][0]); //close read descriptor
                        // write(fd[file_index][1], &type[0], 1);
                        printf("Parent(%d) send childID: %s\n", getpid(), str);
                        // close(fd[file_index][1]); //close write descriptor
                    }
                    else{ //child
                        //close(fd[file_index][1]); //close write descriptor
                        // First open in read only and read
                        char id[MSGSIZE + 1];
                        int new_fd = open(my_fifo, O_RDONLY);
                        read(new_fd, id, MSGSIZE);
                        close(new_fd);
                        //read(fd[file_index][0], id, 1);
                        printf("%d Child(%d) received childID: %s\n", file_index, getpid(), id);
                        //close(fd[file_index][0]); //close the read-descriptor
                        //TODO: CALL FUNCTION TO CALCULATE AND SEND DATA TO REDUCE WITH NAMED PIPE
                    }
                }
            }
            closedir(d);
        }
        //write(p[1], "This should be the answer", MSGSIZE);
        while ((nbytes = read(p[0], inbuf, MSGSIZE)) > 0)
            printf("ANS: %s\n", inbuf);
        if (nbytes != 0)
            exit(2);
        //TODO: ADD TO OUT.TXT
        close(p[0]); //close the read descriptor
    }    
    else { //reduce process
        close(p[1]); //close the read descriptor
        //GET DATA FROM MAP PROCESSES
        //CALCULATE SUM
        //SEND SUM TO MAIN
        close(p[0]); //close the write descriptor
        printf("Sent data from reduce process\n");
    }
    return 0;
}