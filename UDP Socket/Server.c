#include <sys/types.h>
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
#include <string.h>
#include<dirent.h>
#include<sys/stat.h>
/* You will have to modify the program below */

#define MAXBUFSIZE 100

int main (int argc, char * argv[] )
{
    
    
    int sock;                           //This will be our socket
    struct sockaddr_in sin, remote;     //"Internet socket address structure"
    socklen_t remote_length;         //length of the sockaddr_in structure
    long nbytes;                        //number of bytes we receive in our message
    char buffer[MAXBUFSIZE];             //a buffer to store our received message
    const struct sockaddr_in* s = &remote;
    if (argc != 2)
    {
        printf ("USAGE:  <port>\n");
        exit(1);
    }
 //   scanf("%s", argv[1]);
    /******************
     This code populates the sockaddr_in struct with
     the information about our socket
     ******************/
    bzero(&sin,sizeof(sin));                    //zero the struct
    sin.sin_family = AF_INET;                   //address family
    sin.sin_port = htons(atoi(argv[1]));        //htons() sets the port # to network byte order
    printf("%hu %d", sin.sin_port, atoi(argv[1]));
    sin.sin_addr.s_addr = INADDR_ANY;           //supplies the IP address of the local machine
    
    
    //Causes the system to create a generic socket of type UDP (datagram)
    if ((sock=socket(PF_INET, SOCK_DGRAM, 0)) < 0)
    {
        printf("unable to create socket");
    }
    
    
    /******************
     Once we've created a socket, we must bind that socket to the
     local address and port we've supplied in the sockaddr_in struct
     ******************/
    if (bind(sock, (struct sockaddr *)&sin, sizeof(sin)) == -1)
    {
        printf("unable to bind socket\n");
    }
    
    remote_length = (socklen_t)sizeof(remote);
    
    //waits for an incoming message
    
    while(1){
        
        //command
        recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)(&remote), &remote_length);
        
        //exit
        if(strcmp(buffer,"exit") == 0){
            bzero(buffer,sizeof(buffer));
            exit(0);
        }
        
        //get
        if(strcmp(buffer,"get")==0){
            bzero(buffer,sizeof(buffer));
            //receive fileName
            recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)(&remote), &remote_length);
           // strcpy(buffer, fileName);
            FILE *fp;
            fp=fopen(buffer, "r");
            //copy file to buffer
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
        
        //put
        if(strcmp(buffer, "put")==0){
            bzero(buffer,sizeof(buffer));
            //receive fileName
            recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)(&remote), &remote_length);
            char fileName[100]="receive_";
            strcat(fileName,buffer);
            bzero(buffer,sizeof(buffer));
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
        
        //ls
        if(strcmp(buffer,"ls")==0){
            bzero(buffer,sizeof(buffer));
            //list
            DIR *dir_ptr;
            dir_ptr = opendir(".");
            struct dirent *dirent_ptr;
            struct stat my_stat;
            while((dirent_ptr = readdir(dir_ptr)) !=NULL)
            {
                stat(dirent_ptr->d_name, &my_stat);
                strcat(buffer, dirent_ptr->d_name);
                strcat(buffer," ");
            }
            sendto(sock, buffer, (int)sizeof(buffer), 0, (struct sockaddr *)s, sizeof(remote));
            closedir(dir_ptr);
            bzero(buffer,sizeof(buffer));
            continue;
        }
    }
    close(sock);
}

