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

#define BUFF_LENGTH 1024
int setupServer(int port) {
    struct sockaddr_in address;
    int server_fd;
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    
    listen(server_fd, 4);

    return server_fd;
}

int acceptClient(int server_fd) {
    int client_fd;
    struct sockaddr_in client_address;
    int address_len = sizeof(client_address);
    client_fd = accept(server_fd, (struct sockaddr *)&client_address, (socklen_t*) &address_len);

    return client_fd;
}



int main(int argc, char const *argv[]) {
    int comp[3], elec[3], civ[3], mec[3];
    int comp_cnt = 0;
    int elec_cnt = 0;
    int civ_cnt = 0;
    int mec_cnt = 0;
    int comp_port = 6000;
    int elec_port = 7000;
    int civ_port = 8000;
    int mec_port = 9000;
    int server_fd, new_socket, max_sd;
    char buffer[BUFF_LENGTH] = {0};
    memset(buffer, 0, BUFF_LENGTH);  
    fd_set master_set, working_set;
       
    server_fd = setupServer(atoi(argv[1]));

    FD_ZERO(&master_set);
    max_sd = server_fd;
    FD_SET(server_fd, &master_set);

    printf("Server is running.\n");
    int in_room[BUFF_LENGTH] = {0};
    while (1) {
        working_set = master_set;
        select(max_sd + 1, &working_set, NULL, NULL, NULL);

        for (int i = 0; i <= max_sd; i++) {
            if (FD_ISSET(i, &working_set)) {
                
                if (i == server_fd) {  // new clinet
                    new_socket = acceptClient(server_fd);
                    FD_SET(new_socket, &master_set);
                    if (new_socket > max_sd)
                        max_sd = new_socket;
                    printf("New client connected. fd = %d\n", new_socket);
                    char menu[] = "Select your major (by number):\n1-Computer Engineering\n2-Electrical Engineering\n3-Civil Engineering\n4-Mechanical Engineering\n";
                    send(new_socket, menu, strlen(menu), 0);
                    in_room[new_socket] = false;
                }
                
                else { // client sending msg
                    memset(buffer, 0, BUFF_LENGTH);
                    int bytes_received;
                    bytes_received = recv(i , buffer, BUFF_LENGTH, 0);
                    if (bytes_received == 0) { // EOF
                        printf("client fd = %d closed\n", i);
                        close(i);
                        FD_CLR(i, &master_set);
                        continue;
                    }
                    if (in_room[i] == 0){
                        if (buffer[0] >= '1' && buffer[0] <= '4'){
                            printf("Client %d picked a field!\n", i);
                            int choice = atoi(buffer);
                            switch(choice){
                                case 1:
                                    comp[comp_cnt] = i;
                                    comp_cnt++;
                                    break;
                                case 2:
                                    elec[elec_cnt] = i;
                                    elec_cnt++;
                                    break;
                                case 3:
                                    civ[civ_cnt] = i;
                                    civ_cnt++;
                                    break;
                                case 4:
                                    mec[mec_cnt] = i;
                                    mec_cnt++;
                                    break;
                                default:
                                    printf("Input not valid\n");
                            }
                            if (comp_cnt == 3){
                                char port[4];
                                sprintf(port, "%d", comp_port);
                                send(comp[0], port, BUFF_LENGTH, 0);
                                send(comp[0], "0", BUFF_LENGTH, 0);
                                send(comp[1], port, BUFF_LENGTH, 0);
                                send(comp[1], "1", BUFF_LENGTH, 0);
                                send(comp[2], port, BUFF_LENGTH, 0);
                                send(comp[2], "2", BUFF_LENGTH, 0);
                                in_room[comp[0]] = 1;
                                in_room[comp[1]] = 1;
                                in_room[comp[2]] = 1;
                                comp_port++;
                                comp_cnt = 0;
                            }
                            if (elec_cnt == 3){
                                char port[4];
                                sprintf(port, "%d", elec_port);
                                send(elec[0], port, BUFF_LENGTH, 0);
                                send(elec[0], "0", BUFF_LENGTH, 0);
                                send(elec[1], port, BUFF_LENGTH, 0);
                                send(elec[1], "1", BUFF_LENGTH, 0);
                                send(elec[2], port, BUFF_LENGTH, 0);
                                send(elec[2], "2", BUFF_LENGTH, 0);
                                in_room[elec[0]] = 2;
                                in_room[elec[1]] = 2;
                                in_room[elec[2]] = 2;
                                elec_port++;
                                elec_cnt = 0;
                            }
                            if (civ_cnt == 3){
                                char port[4];
                                sprintf(port, "%d", civ_port);
                                send(civ[0], port, BUFF_LENGTH, 0);
                                send(civ[0], "0", BUFF_LENGTH, 0);
                                send(civ[1], port, BUFF_LENGTH, 0);
                                send(civ[1], "1", BUFF_LENGTH, 0);
                                send(civ[2], port, BUFF_LENGTH, 0);
                                send(civ[2], "2", BUFF_LENGTH, 0);
                                in_room[civ[0]] = 3;
                                in_room[civ[1]] = 3;
                                in_room[civ[2]] = 3;
                                civ_port++;
                                civ_cnt = 0;
                            }
                            if (mec_cnt == 3){
                                char port[4];
                                sprintf(port, "%d", mec_port);
                                send(mec[0], port, BUFF_LENGTH, 0);
                                send(mec[0], "0", BUFF_LENGTH, 0);
                                send(mec[1], port, BUFF_LENGTH, 0);
                                send(mec[1], "1", BUFF_LENGTH, 0);
                                send(mec[2], port, BUFF_LENGTH, 0);
                                send(mec[2], "2", BUFF_LENGTH, 0);
                                in_room[mec[0]] = 4;
                                in_room[mec[1]] = 4;
                                in_room[mec[2]] = 4;
                                mec_port++;
                                mec_cnt = 0;
                            }
                        }
                    }
                    else{
                        int new_fd;
                        switch (in_room[i]) {
                            case 1:
                                new_fd = open("comp.txt", O_CREAT | O_APPEND | O_RDWR, 0664);
                                write(new_fd, buffer, strlen(buffer));
                                close(new_fd);
                                break;
                            case 2:
                                new_fd = open("elec.txt", O_CREAT | O_APPEND | O_RDWR, 0664);
                                write(new_fd, buffer, strlen(buffer));
                                close(new_fd);
                                break;
                            case 3:
                                new_fd = open("civ.txt", O_CREAT | O_APPEND | O_RDWR, 0664);
                                write(new_fd, buffer, strlen(buffer));
                                close(new_fd);
                                break;
                            case 4:
                                new_fd = open("mec.txt", O_CREAT | O_APPEND | O_RDWR, 0664);
                                write(new_fd, buffer, strlen(buffer));
                                close(new_fd);
                                break;
                            default:
                                break;
                        }
                    }
                    memset(buffer, 0, BUFF_LENGTH);
                }
            }
        }

    }
    return 0;
}