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
#define MAX_WORDS_NO 1000
#define NAMED_PIPE "NAMED_PIPEEE"
#define NAMED_PIPE_LEN 10000

struct PAIR {
    int len;
    const char* word[MAX_WORDS_NO];
    int occurance[MAX_WORDS_NO];
};

void cast_to_string(struct PAIR* pair, char* data){
    for (int i = 0; i < pair->len; i++){
        const char* curr_word = pair->word[i];
        int length = 0, j;
        while (data[length] != '\0') {
            ++length;
        }

        // concatenate s2 to s1
        for (j = 0; curr_word[j] != '\0'; ++j, ++length) {
            data[length] = curr_word[j];
        }

        // terminating the s1 string
        data[length] = '\0';
        
        strcat(data, ":");
        char str[MSGSIZE];
        sprintf(str, "%d", pair->occurance[i]);
        strcat(data, str);
        strcat(data, ",");
    }
}
struct PAIR* decast_from_string(char* data){
    struct PAIR* output = malloc(sizeof(*output));
    int len = 0;
    int i = 0;
    while (data[i] != '\0'){
        char* word = "";
        int occurance = 0;
        while (data[i] != ':'){
            word += data[i];
            i++;
        }
        i++;
        while (data[i] != ','){
            occurance = occurance * 10 + (data[i] - '0');
        }
        i++;
        output->occurance[output->len] = occurance;
        output->word[output->len] = word;
        output->len++;
        printf("seen word is: %s\n", output->word[output->len - 1]);
    }
    return output;
}

int main()
{
    struct PAIR* map_output = malloc(sizeof(*map_output));
    char inbuf[MSGSIZE], data[NAMED_PIPE_LEN];
    int p[2], fd[MAX_FILE_NO][2], pid[MAX_FILE_NO], nbytes;
    if (pipe(p) < 0)
        exit(1);
    pid[0] = fork();
    int MAIN_PROCESS_ID = getpid();
    
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
                    
                    printf("Found a new .csv file: %s\n", dir->d_name);
                    int file_index = type[0] - '0';
                    pipe(fd[file_index]);
                    pid[file_index] = fork();
                    if (pid[file_index] > 0){ //main
                        close(fd[file_index][0]); //close read descriptor
                        char str[MSGSIZE + 1];
                        sprintf(str, "%d", file_index);
                        write(fd[file_index][1], str, MSGSIZE);
                        printf("Parent(%d) send childID: %s\n", getpid(), str);
                        close(fd[file_index][1]); //close write descriptor
                    }
                    else{ //child
                        close(fd[file_index][1]); //close write descriptor
                        char id[MSGSIZE + 1];
                        read(fd[file_index][0], id, MSGSIZE);
                        printf("%d Child(%d) received childID: %s\n", file_index, getpid(), id);
                        close(fd[file_index][0]); //close the read-descriptor

                        struct PAIR* map_input = malloc(sizeof(*map_input));
                        map_input->word[0] = "hi";
                        map_input->word[1] = "bye";
                        map_input->occurance[0] = 1;
                        map_input->occurance[1] = 1;
                        map_input->len = 2;
                        //TODO: CALL FUNCTION TO CALCULATE 
                        //char pipe_name[11];
                        //pipe_name[0] = type[0];
                        //strcat(pipe_name, NAMED_PIPE);
                        printf("sooo\n");
                        char* my_fifo;
                        printf("nnn\n");
                        *my_fifo = type[0];
                        printf("my fifo %s_", my_fifo);
                        mkfifo(my_fifo, 0666);
                        
                        strcpy(data, "salam");
                        //cast_to_string(map_input, data);
                        int new_fd = open(my_fifo, O_WRONLY);
                        write(new_fd, data, strlen(data));
                        close(new_fd);
                    }
                }
            }
            closedir(d);
        }
        //write(p[1], "This should be the answer", MSGSIZE);
        // while ((nbytes = read(p[0], inbuf, MSGSIZE)) > 0)
        //     printf("ANS: %s\n", inbuf);
        // if (nbytes != 0)
        //     exit(2);
        //TODO: ADD TO OUT.TXT
        close(p[0]); //close the read descriptor
    }    
    else { //reduce process
        close(p[0]); //close the read descriptor
        sleep(5);
        //char pipe_name[11];
        //pipe_name[0] = '1';
        //strcat(pipe_name, NAMED_PIPE);
        char* my_fifo;
        *my_fifo = '1';
        printf("my fifo %s_\n", my_fifo);
        mkfifo(my_fifo, 0666);
        printf("tda\n");
        int new_fd = open(my_fifo, O_RDONLY);
        strcpy(data, "");
        printf("huzza");
        read(new_fd, data, strlen(data));
        printf("Gotten string is: %s___\n", data);
        //map_output = decast_from_string(data);
        //printf("reduce got data from map and: %s___\n", map_output->word[0]);
        close(new_fd);
        //CALCULATE SUM
        //SEND SUM TO MAIN
        close(p[1]); //close the write descriptor
        printf("Sent data from reduce process\n");
    }
    return 0;
}