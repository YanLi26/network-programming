//
//  webproxy.c
//  Web Proxy Server
//
//  Created by Yan Li on 11/20/16.
//  Copyright © 2016 Yan Li. All rights reserved.
//

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <time.h>
#include <openssl/md5.h>

#define MSG_LEN 1024
#define PAR_LEN 256

bool get(int newSock, char* message);
void parseMsg(char* message, char* value, char* website, char* httpVersion, char* host);
bool findFile(char* fileName, int newSock);
char* md5(char* fileName);

int main(int argc, char *argv[]){
    int sock, newSock;
    char message[MSG_LEN];
    struct sockaddr_in cliAdd, proAdd;
    socklen_t cliAddLen = 0;
    
    //create a socket
    if((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        printf("unable to create socket");
        exit(-1);
    }
    
    memset(&cliAdd, 0, sizeof(struct sockaddr_in));
    memset(&proAdd, 0, sizeof(struct sockaddr_in));
    
    //initialize sin
    cliAdd.sin_family = AF_INET;                   //address family
    cliAdd.sin_addr.s_addr = htonl(INADDR_ANY);
    cliAdd.sin_port = htons(atoi(argv[1]));        //htons() sets the port # to network byte order
    bzero(&cliAdd.sin_zero, 8);
    
    //bind this socket to an address and a port
    if(bind(sock, (const struct sockaddr *) &cliAdd, sizeof(struct sockaddr_in)) == -1){
        printf("Failed to bind socket to an address and a port!");
        exit(-1);
    }
    
    //listen
    if((listen(sock, 10)) == -1){
        perror("unablt to listen");
        exit(-1);
    }
    
    while(1){
        //fork()
        fork();
        bzero(&cliAdd, sizeof(cliAdd));
        if((newSock = accept(sock, (struct sockaddr *)&cliAdd, &cliAddLen)) == -1){
            printf("Failed to accept");
            exit(-1);
        }
        
        while(1){
            read(newSock, message, MSG_LEN);
            printf("%s", "Message: ");
            printf("%s\n", message);
            time_t current_time;
            time_t connect_time;
            current_time = time(NULL);
            get(newSock, message);
            connect_time = time(NULL);
            if(connect_time - current_time > atoi(argv[2])){
                printf("\n%s\n", "Timeout!");
                exit(-1);
            }
            else{
                printf("\n%s\n", "It is not timeout.");
            }
        }
    }
    close(newSock);
    close(sock);
    
    return 0;
}




//----------------------------------------------- GET ------------------------------------------------
bool get(int newSock, char* message){
    char value[PAR_LEN], website[PAR_LEN], httpVersion[PAR_LEN], address[PAR_LEN], fileName[256], host[PAR_LEN];
    memset(address, '\0', PAR_LEN);
    memset(fileName, '\0', 256);
    parseMsg(message, value, website, httpVersion, host);
    printf("%s", "Requst type is: ");
    printf("%s\n", value);
    printf("%s", "Requst page is: ");
    printf("%s\n", website);
    printf("%s", "Requst HTTP version is: ");
    printf("%s\n", httpVersion);
    printf("%s", "Requst host is: ");
    printf("%s\n", host);
    strcpy(fileName, host);
    strcat(fileName, website);
    strcpy(fileName,md5(fileName));
    printf("%s", "File name with MD5 : ");
    printf("%s\n", fileName);
    if(findFile(fileName, newSock) == true){
        return true;
    }
    
    int serSock;
    char buffer[PAR_LEN];
    struct sockaddr_in serAdd;
    struct in_addr **addr_list;
    struct hostent *server;
    
    //create a socket
    if((serSock = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        printf("unable to create socket");
        exit(-1);
    }
    memset(&serAdd, 0, sizeof(struct sockaddr_in));
    if ((server = gethostbyname(host)) == NULL) {  // get the host info
        herror("gethostbyname");
    }
    addr_list = (struct in_addr **)server->h_addr_list;
    sprintf(address, "%s", inet_ntoa(*addr_list[0]));
    //strcmp(address, inet_ntoa(*addr_list[0]));
    printf("%s\n",address);
    
    printf("%s\n", server->h_name);
    
    //initialize sin
    serAdd.sin_family = AF_INET;                   //address family
    serAdd.sin_addr.s_addr = inet_addr(address);
    serAdd.sin_port = htons(80);        //htons() sets the port # to network byte order
    bzero(&serAdd.sin_zero, 8);
    
    //connect this socket to an address and a port
    if(connect(serSock, (struct sockaddr *)&serAdd, sizeof(serAdd))<0){
        printf("Failed to connect socket to an address and a port!");
        exit(-1);
    }
    
    //HTTP GET header
    memset(buffer, '\0', 256);
    strcpy(buffer, value);
    strcat(buffer, " ");
    strcat(buffer, website);
    strcat(buffer, " ");
    strcat(buffer, httpVersion);
    strcat(buffer, "\r\nHost: ");
    strcat(buffer, host);
    strcat(buffer, "\r\n\r\n");
    send(serSock, buffer, 256, 0);
    memset(buffer, '\0', 256);
    int flag = 1, n;
    
    FILE* fp;
    fp = fopen(fileName, "w");
    if(fp == NULL){
        perror("Cannot open fpOne");
        return false;
    }
    
    while((n = read(serSock, buffer, 256) > 0))
    {
        if(flag)
        {
            printf("%s", buffer);
            flag = 0;
        }
        fputs(buffer, fp);
        send(newSock, buffer, strlen(buffer), 0);
        memset(buffer, '\0', 256);
    }
    fclose(fp);
    close(serSock);
    return true;
}


//-------------------------------------------- parse http --------------------------------------------

void parseMsg(char* message, char* value, char* website, char* httpVersion, char* host){
    memset(value, '\0', PAR_LEN);
    memset(website, '\0', PAR_LEN);
    memset(httpVersion, '\0', PAR_LEN);
    memset(host, '\0', PAR_LEN);
    
    int i = 0;
    for(; i < strlen(message); i++){
        if(message[i] == ' ') continue;
        else break;
    }
    
    for(int j = 0; message[i] != ' ' && i<strlen(message); i++, j++){
        value[j] = message[i];
    }
    
    for(; i < strlen(message); i++){
        if(message[i] == ' ') continue;
        else break;
    }
    
    if(strncmp(message + i, "http://", 7) == 0){
        i=i+7;
    }
    
    if(strncmp(message + i, "https://", 8) == 0){
        i = i + 8;
    }
    
    for(int j = 0; message[i] != ' ' && i<strlen(message) && message[i] != '/'; i++, j++){
        host[j] = message[i];
    }
    
    for(int j = 0; message[i] !=' ' && i<strlen(message); i++, j++){
        website[j] = message[i];
    }
    
    for(; i < strlen(message); i++){
        if(message[i] == ' ') continue;
        else break;
    }
    
    for(int j = 0; message[i] != ' ' && i<strlen(message) && message[i] != '\n'; i++, j++){
        httpVersion[j] = message[i];
    }
    
}

//-------------------------------- findFile --------------------------------

bool findFile(char* fileName, int newSock){
    FILE* fp;
    long fileSize;
    fp = fopen(fileName, "r");
    if(fp == NULL){
        return false;
    }
    else{
        //fileSize
        fseek(fp, 0, 2);
        fileSize = ftell(fp);
        rewind(fp);
        fileSize++;
        
        //send file
        for(int i = 0; i<= (fileSize/256); i++){
            char buffer[fileSize];
            memset(buffer, '\0', fileSize);
            fread(buffer, 1, fileSize - 1, fp);
            send(newSock, buffer, fileSize, 0);
        }
    }
    return true;
}


char* md5(char* fileName) {
    int n;
    MD5_CTX c;
    unsigned char digest[16];
    char *out = (char*)malloc(33);
    
    MD5_Init(&c);
    MD5_Update(&c, fileName, strlen(fileName));
    
    MD5_Final(digest, &c);
    
    for (n = 0; n < 16; ++n) {
        snprintf(&(out[n*2]), 16*2, "%02x", (unsigned int)digest[n]);
    }
    
    return out;
}

