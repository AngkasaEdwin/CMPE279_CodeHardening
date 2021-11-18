// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/wait.h>
#define PORT 8080

int main(int argc, char const *argv[])
{
    if(argc == 1){
        //PART 1 - SETTING UP SOCKET
        int server_fd, new_socket;
        struct sockaddr_in address;
        int opt = 1;
        int addrlen = sizeof(address);
        char *hello = "Hello from server";

        // Creating socket file descriptor
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        {
            perror("socket failed");
            exit(EXIT_FAILURE);
        }

        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
        &opt, sizeof(opt)))
        {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons( PORT );

        if (bind(server_fd, (struct sockaddr *)&address,
        sizeof(address)) < 0)
        {
            perror("bind failed");
            exit(EXIT_FAILURE);
        }

        if (listen(server_fd, 3) < 0)
        {
            perror("listen");
            exit(EXIT_FAILURE);
        }

        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                        (socklen_t*)&addrlen))<0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        
        //Fork and Exec
        pid_t childPid = fork();
        if(childPid == 0){ 
            char fd_str[50];
            snprintf(fd_str, sizeof(fd_str), "%d", new_socket);
            char *args[] = { argv[0], fd_str, NULL };
            execv(args[0], args);    
            exit(EXIT_FAILURE);
        }else{
            wait(0);
        }

        send(new_socket, hello, strlen(hello), 0);
        printf("Hello message sent\n");
    } else {
        //PART 2 (CHILD) - READING DATA FROM CLIENT TO A BUFFER AFTER EXEC
        int new_socket = atoi(argv[1]);
        int valread;
        char buffer[1024] = {0};
        struct passwd *nobody_struct;
        nobody_struct = getpwnam("nobody");
        if(nobody_struct == NULL){
            printf("ERROR retrieving Nobody ID");
            exit(1);
        }
        uid_t nobody_id = nobody_struct->pw_uid;
        setuid(nobody_id);
        valread = read(new_socket, buffer, 1024);
        printf("Read %d bytes: %s\n", valread, buffer);
    }
    

    return 0;
}
