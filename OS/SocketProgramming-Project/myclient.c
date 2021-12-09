#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <stdbool.h>
#include <signal.h>

#define GIVEN_TIME 60
#define BUFF_LENGTH 1024

void alarm_handler(int sig) {
    printf("#SKIP (please remember that you only have one minute to answer the question)\n");
}

void ask(char* log, int sock, struct sockaddr_in* bc_address, socklen_t size_bc_address, int order, char* first_id, char* second_id){
    memset(log, 0, BUFF_LENGTH);
    strcat(log, "Question: ");
    char buff[BUFF_LENGTH], answer1[BUFF_LENGTH], answer2[BUFF_LENGTH];
    printf("Please ask your question:\n");
    memset(buff, 0, BUFF_LENGTH);
    read(0, buff, BUFF_LENGTH);
    strcat(log, buff); //adding question to log
    printf("Waiting for answers...\n");
    sendto(sock, buff, strlen(buff), 0, (struct sockaddr *)bc_address, size_bc_address);
    recv(sock, buff, BUFF_LENGTH, 0); //recieve its msg
    memset(buff, 0, BUFF_LENGTH);
    recv(sock, buff, BUFF_LENGTH, 0);
    strcat(log, "Answers:\n");
    printf("Answers are as below:\n");
    printf("%s said: %s\n", first_id, buff);
    strcat(log, first_id); //adding answer 1 to log
    strcat(log, " said: ");
    strcat(log, buff);
    memset(buff, 0, BUFF_LENGTH);
    recv(sock, buff, BUFF_LENGTH, 0);
    printf("%s said: %s\n", second_id, buff);
    strcat(log, second_id); //adding answer 2 to log
    strcat(log, " said: ");
    strcat(log, buff);
    printf("Please choose the best answer (by its id):\n");
    memset(buff, 0, BUFF_LENGTH);
    read(0, buff, BUFF_LENGTH);
    strcat(log, "Best answer: ");
    strcat(log, buff);
    printf("You chose answer no. %s\n", buff);
    sendto(sock, buff, strlen(buff), 0, (struct sockaddr *)bc_address, size_bc_address);
    recv(sock, buff, BUFF_LENGTH, 0); //recieve its msg
    memset(buff, 0, BUFF_LENGTH);
    printf("log is:%s", log);
}

void first_answer(int sock, struct sockaddr_in* bc_address, socklen_t size_bc_address, int order, int id, int second_answer){
    signal(SIGALRM, alarm_handler);
    siginterrupt(SIGALRM, 1);
    char buff[BUFF_LENGTH];
    printf("Fetching the question!...\n");
    memset(buff, 0, BUFF_LENGTH);
    recv(sock, buff, BUFF_LENGTH, 0);
    printf("Please answer the following question (or use 'PASS' to skip):\n%s", buff);
    memset(buff, 0, BUFF_LENGTH);
    alarm(GIVEN_TIME);
    read(0, buff, BUFF_LENGTH);
    alarm(0);
    sendto(sock, buff, strlen(buff), 0, (struct sockaddr *)bc_address, size_bc_address);
    recv(sock, buff, BUFF_LENGTH, 0); //recieve its msg
    printf("Please wait for %d to answer the question.\n", second_answer);
    memset(buff, 0, BUFF_LENGTH);
    recv(sock, buff, BUFF_LENGTH, 0);
    printf("%d said: %s\n",second_answer, buff);
    printf("%d is choosing the best answer!\n", (id + 2)%3);
    memset(buff, 0, BUFF_LENGTH);
    recv(sock, buff, BUFF_LENGTH, 0);
    printf("Best answer is: %s", buff);
    memset(buff, 0, BUFF_LENGTH);
}

void second_answer(int sock, struct sockaddr_in* bc_address, socklen_t size_bc_address, int order, int id, int first_answer){
    signal(SIGALRM, alarm_handler);
    siginterrupt(SIGALRM, 1);
    char buff[BUFF_LENGTH];
    printf("Fetching the question!...\n");
    memset(buff, 0, BUFF_LENGTH);
    recv(sock, buff, BUFF_LENGTH, 0);
    printf("The question is as below:\n%s", buff);
    printf("Please wait for %d to answer the question\n", (id+2)%3);
    memset(buff, 0, BUFF_LENGTH);
    recv(sock, buff, BUFF_LENGTH, 0);
    printf("%d said: %s\n", first_answer, buff);
    printf("Please answer the question (or use 'PASS' to skip).\n");
    memset(buff, 0, BUFF_LENGTH);
    alarm(GIVEN_TIME);
    read(0, buff, BUFF_LENGTH);
    alarm(0);
    sendto(sock, buff, strlen(buff), 0, (struct sockaddr *)bc_address, size_bc_address);
    recv(sock, buff, BUFF_LENGTH, 0); //recieve its msg
    memset(buff, 0, BUFF_LENGTH);
    recv(sock, buff, BUFF_LENGTH, 0);
    printf("%d is choosing the best answer\n", (id + 1)%3);
    printf("Best answer is: %s\n", buff);
    memset(buff, 0, BUFF_LENGTH);
}

int connectServer(int port) {
    int fd;
    struct sockaddr_in server_address;
    
    fd = socket(AF_INET, SOCK_STREAM, 0);
    
    server_address.sin_family = AF_INET; 
    server_address.sin_port = htons(port); 
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) { // checking for errors
        printf("Error in connecting to server\n");
    }

    return fd;
}


int main(int argc, char const *argv[]) {
    int fd;
    char buff[BUFF_LENGTH] = {0};
    memset(buff, 0, BUFF_LENGTH);
    fd = connectServer(atoi(argv[1]));
    recv(fd, buff, BUFF_LENGTH, 0);
    printf("%s\n", buff);
    memset(buff, 0, BUFF_LENGTH);
   
    int read_choice = read(0, buff, BUFF_LENGTH);
    
    if (read_choice != 0){
        send(fd, buff, strlen(buff), 0);
    }
    printf("Waiting to find a server...\n");
    memset(buff, 0, BUFF_LENGTH);
    recv(fd, buff, BUFF_LENGTH, 0);
    printf("You are now on port %s.\n", buff);
    int broadcast_port = atoi(buff);
    memset(buff, 0, BUFF_LENGTH);
    recv(fd, buff, BUFF_LENGTH, 0);
    int id = atoi(buff);
    printf("You are person no. %d.\n", id);
    char *id_string = buff;

    struct sockaddr_in bc_address; //build broadcast port
    int sock, broadcast = 1, opt = 1;
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
    setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
    bc_address.sin_family = AF_INET; 
    bc_address.sin_port = htons(broadcast_port); 
    bc_address.sin_addr.s_addr = inet_addr("255.255.255.255");
    bind(sock, (struct sockaddr *)&bc_address, sizeof(bc_address));

    char log[BUFF_LENGTH] = {0};
    memset(log, 0, BUFF_LENGTH);
    switch (id) {
        case 0:
            memset(log, 0, BUFF_LENGTH);
            ask(log, sock, &bc_address, sizeof(bc_address), 0, "1", "2");
            send(fd, log, strlen(log), 0);
            first_answer(sock, &bc_address, sizeof(bc_address), 0, id, 2);
            first_answer(sock, &bc_address, sizeof(bc_address), 0, id, 1);
            break;
        case 1:
            memset(log, 0, BUFF_LENGTH);
            first_answer(sock, &bc_address, sizeof(bc_address), 0, id, 2);
            memset(log, 0, BUFF_LENGTH);
            ask(log, sock, &bc_address, sizeof(bc_address), 0, "0", "2");
            send(fd, log, strlen(log), 0);
            second_answer(sock, &bc_address, sizeof(bc_address), 0, id, 0);
            break;
        case 2:
            memset(log, 0, BUFF_LENGTH);
            second_answer(sock, &bc_address, sizeof(bc_address), 0, id, 1);
            second_answer(sock, &bc_address, sizeof(bc_address), 0, id, 0);
            memset(log, 0, BUFF_LENGTH);
            ask(log, sock, &bc_address, sizeof(bc_address), 0, "0", "1");
            send(fd, log, strlen(log), 0);
            break;
        default:
            printf("Error communicating.\nExiting NOW!\n");
            close(fd);
            return 0;
            break;
    }
    printf("Closing the room now. Have a good day!\n");
    close(fd);
    return 0;
}