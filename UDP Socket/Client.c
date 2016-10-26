#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>
#include <memory.h>
#include <errno.h>
#include<dirent.h>
#include<sys/stat.h>

#define MAXBUFSIZE 100

/* You will have to modify the program below */

int main (int argc, char * argv[])
{
    
    long nbytes;                            // number of bytes send by sendto()
    int sock;                               //this will be our socket
    char buffer[MAXBUFSIZE];
    socklen_t remote_length; 
    struct sockaddr_in remote;              //"Internet socket address structure"
    
    if (argc < 3)
    {
        printf("USAGE:  <server_ip> <server_port>\n");
        exit(1);
    }
    /******************
     Here we populate a sockaddr_in struct with
     information regarding where we'd like to send our packet
     i.e the Server.
     ******************/
    bzero(&remote,sizeof(remote));               //zero the struct
    remote.sin_family = AF_INET;                 //address family
    remote.sin_port = htons(atoi(argv[2]));      //sets port to network byte order
    remote.sin_addr.s_addr = inet_addr(argv[1]); //sets remote IP address
    const struct sockaddr_in* s = &remote;
    //Causes the system to create a generic socket of type UDP (datagram)
    if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
    {
        printf("unable to create socket");
        exit(1);
    }
    remote_length = (socklen_t)sizeof(remote);
    /******************
     sendto() sends immediately.
     it will report an error if the message fails to leave the computer
     however, with UDP, there is no error if the message is lost in the network once it leaves the computer.
     ******************/
    char command[10];
    //input command
    while(gets(command)){
        
        //exit
        if(strcmp(command, "exit") == 0){
            bzero(buffer,sizeof(buffer));
            strcpy(buffer, "exit");
            sendto(sock, buffer, (int)sizeof(buffer), 0, (struct sockaddr *)s, sizeof(remote));
        }
        
        //get
        if(strcmp(command, "get") == 0){
            char fileName[100]="return_";
            bzero(buffer,sizeof(buffer));
            strcpy(buffer,"get");
            //send get
            sendto(sock, buffer, (int)sizeof(buffer), 0, (struct sockaddr *)s, sizeof(remote));
            bzero(buffer,sizeof(buffer));
            scanf("%s", buffer);
            strcat(fileName, buffer);
            //send fileName
            sendto(sock, buffer, (int)sizeof(buffer), 0, (struct sockaddr *)s, sizeof(remote));
            //receive file
            recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)(&remote), &remote_length);
            //storage file
            FILE *fp;
            fp=fopen(fileName, "w");
            
           //copy buffer to fp
            fwrite(buffer, sizeof(buffer), sizeof(char), fp);
            fclose(fp);
            bzero(buffer,sizeof(buffer));
            continue;
        }
        
        //put
        if(strcmp(command, "put")==0){
            char fileName[100];
            bzero(buffer,sizeof(buffer));
            //send put
            strcpy(buffer,"put");
            sendto(sock, buffer, (int)sizeof(buffer), 0, (struct sockaddr *)s, sizeof(remote));
            bzero(buffer,sizeof(buffer));
            //send fileName
            scanf("%s", fileName);
            strcpy(buffer,fileName);
            sendto(sock, buffer, (int)sizeof(buffer), 0, (struct sockaddr *)s, sizeof(remote));
            //copy file to buffer
            FILE *fp;
            fp=fopen(fileName, "r");
            bzero(buffer,sizeof(buffer));
            for(int i = 0; !feof(fp); i++)
                fscanf(fp, "%s", &buffer[i]);
            nbytes=sendto(sock, buffer, (int)sizeof(buffer), 0, (struct sockaddr *)s, sizeof(remote));
            if(nbytes<0){
                printf("cannot send the file\n");
                exit(1);
            }
            fclose(fp);
            bzero(buffer,sizeof(buffer));
            continue;
        }
        
        //ls
        if(strcmp(command, "ls")==0){
            //send ls
            bzero(buffer,sizeof(buffer));
            strcpy(buffer, "ls");
            sendto(sock, buffer, (int)sizeof(buffer), 0, (struct sockaddr *)s, sizeof(remote));
            bzero(buffer,sizeof(buffer));
            recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)(&remote), &remote_length);
            printf("%s",buffer);
            bzero(buffer,sizeof(buffer));
            continue;
        }
    }
    close(sock);
}

