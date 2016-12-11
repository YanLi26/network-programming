//
//  dfs.c
//  distributed file system
//
//  Created by Yan Li on 11/1/16.
//  Copyright © 2016 Yan Li. All rights reserved.
//

#include <stdio.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>


#define VALUE_SIZE 50
#define MAXBUFFSIZE 1024

bool readConfig(char *configFilePath, char *username, char *password);
bool put(int newSock, char* serverFolder, char* username);
bool get(int newSock, char* serverFolder, char* username);
bool list(int newSock, char* serverFolder, char* username);
long toLong(char *buff);
char* encryption(char* content, char* key);
bool mkdirServer(int newSock, char* serverFolder, char* username);


int main(int argc, char *argv[]){
    int sock, newSock;
    struct sockaddr_in serAdd, cliAdd;
    socklen_t cliAddLen=0;
    char username[256];
    char password[256];
    char requestType[256];
    char* serverFolder = argv[1];
    char* portNumber = argv[2];
    
    //create a socket
    if((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        printf("unable to create socket");
        exit(-1);
    }

    memset(&serAdd, 0, sizeof(struct sockaddr_in));
    memset(&cliAdd, 0, sizeof(struct sockaddr_in));
    
    //initialize sin
    //bzero(&serAdd,sizeof(serAdd));                    //zero the struct
    serAdd.sin_family = AF_INET;                   //address family
    serAdd.sin_addr.s_addr = htonl(INADDR_ANY);
    serAdd.sin_port = htons(atoi(argv[2]));        //htons() sets the port # to network byte order
    bzero(&serAdd.sin_zero, 8);

    //bind this socket to an address and a port
    if(bind(sock, (const struct sockaddr *) &serAdd, sizeof(struct sockaddr_in)) == -1){
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
                read(newSock, username, 256);
                printf("%s\n", username);
                read(newSock, password, 256);
                printf("%s\n", password);
                if(readConfig("dfs.conf", username, password) == true){
                    printf("%s", "This user is exsit");
                }
                else{
                    perror("This user is not exist or the password is wrong!\n");
                    exit(-1);
                }
                read(newSock, requestType, 256);
                printf("%s\n", requestType);
                //PUT
                if(strcmp(requestType, "PUT") == 0){
                    printf("%s", "PUT");
                    if(readConfig("dfs.conf", username, password) == true){
                        printf("%s\n", "put");
                        put(newSock, serverFolder, username);
                    }
                    else{
                        perror("This user is not exist or the password is wrong!\n");
                        exit(-1);
                    }
                }
                
                //GET
                if(strcmp(requestType, "GET") == 0){
                    printf("%s", "GET");
                    if(readConfig("dfs.conf", username, password) == true){
                        printf("%s\n", "get");
                        get(newSock, serverFolder, username);
                        printf("%s\n", "get");
                    }
                    else{
                        perror("This user is not exist or the password is wrong!\n");
                        exit(-1);
                    }
                }
                
                //LIST
                if(strcmp(requestType, "LIST") == 0){
                    if(readConfig("dfs.conf", username, password) == true){
                        printf("%s\n", "list");
                        list(newSock, serverFolder, username);
                        printf("%s\n", "list");
                    }
                    else{
                        perror("This user is not exist or the password is wrong!\n");
                        exit(-1);
                    }
                }
                
                //MKDIR
                if(strcmp(requestType, "MKDIR") == 0){
                    printf("%s\n", "MKDIR");
                    if(readConfig("dfs.conf", username, password) == true){
                        mkdirServer(newSock, serverFolder, username);
                    }
                    else{
                        perror("This user is not exist or the password is wrong!\n");
                        exit(-1);
                    }
                }
                
            }
        }
    close(newSock);
    close(sock);
//    }
//    int pid;
//    do{
//        pid = wait(NULL);
//    }while(pid != -1);
//    return 0;
        //if(!fork()){
          //  close(sock);
            //}
}

//-------------------------------- read configuration file --------------------------------

bool readConfig(char *configFilePath, char *username, char *password){
    printf("%s\n", username);
    printf("%s\n", password);

    unsigned long usernameLen = strlen(username);
    unsigned long passwordLen = strlen(password);
    char buff[100]="";
    char keyJudge[100];
    
    //open the configuration File
    FILE *fp;
    fp = fopen(configFilePath,"r");
    printf("%s\n", "open");
    if (fp == NULL){
        printf("Cannot open configuration file!\n");
        return false;
    }
    
    //read every line from the file
    rewind(fp);
    while(fgets(buff, 100, fp)!=NULL){
        strncpy(keyJudge, buff, usernameLen);
        printf("%s\n", keyJudge);

        //compare the username
        if(strcmp(username, keyJudge) == 0){
            //skip space
            int i;
            for(i = usernameLen; i < strlen(buff); i++){
                if(buff[i] != ' ') break;
            }
            //compare the password
            strncpy(keyJudge, buff + i, passwordLen);
            printf("%s\n", keyJudge);

            if(strcmp(password, keyJudge) == 0 || strcmp(password, encryption(keyJudge, "key")) == 0){
                printf("%s\n", "true");
                fclose(fp);
                printf("%s\n", "true");

                return true;
            }
        }
    }
    fclose(fp);
    return false;
}

//-------------------------------- put --------------------------------
bool put(int newSock, char* serverFolder, char* username){
    char fileName1[256], newFileName1[256];
    char fileName2[256], newFileName2[256];
    long fileSize1, fileSize2;
    char buff[256];
    
    //read file One
    read(newSock, buff, 256);
    fileSize1 = toLong(buff);
    printf("%ld\n", fileSize1);

    read(newSock, fileName1, 256);
    printf("%s\n", fileName1);
    char buffOne[fileSize1];
    read(newSock, buffOne, fileSize1);
    printf("%s\n", buffOne);
    
    memset(newFileName1, '\0', 256);
    memset(newFileName2, '\0', 256);

    //change file path one
    strcat(newFileName1, ".");
    strcat(newFileName1, serverFolder);
    strcat(newFileName1, "/");
    strcat(newFileName1, username);
    int status;
    status = mkdir(newFileName1,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    strcat(newFileName1, "/");
    strcat(newFileName1, fileName1);
    printf("%s", newFileName1);
    
    //read file Two
    read(newSock, buff, 256);
    fileSize2 = toLong(buff);
    printf("%ld\n", fileSize2);
    read(newSock, fileName2, 256);
    printf("%s\n", fileName2);
    char buffTwo[fileSize2];
    read(newSock, buffTwo, fileSize2);
    printf("%s\n", buffTwo);
    
    //change file path two
    strcat(newFileName2, ".");
    strcat(newFileName2, serverFolder);
    strcat(newFileName2, "/");
    strcat(newFileName2, username);
    strcat(newFileName2, "/");
    strcat(newFileName2, fileName2);
    printf("%s", newFileName2);

    FILE* fpOne;
    FILE* fpTwo;
    
    fpOne = fopen(newFileName1, "w");
    fpTwo = fopen(newFileName2, "w");
    
    //write in fpOne
    if(fpOne == NULL){
        perror("Cannot open fpOne");
        return false;
    }
    else if(fpOne != NULL){
        fputs(buffOne, fpOne);
    }
    fclose(fpOne);
    
    //write in fpTwo
    if(fpTwo == NULL){
        perror("Cannot open fpTwo");
        return false;
    }
    else if(fpTwo != NULL){
        fputs(buffTwo, fpTwo);
    }
    fclose(fpTwo);
    return true;
}

//-------------------------------- get or GET --------------------------------
bool get(int newSock, char* serverFolder, char* username){
    char fileName[256], buff[256], newFileName[256];
    long fileSize;
    
    memset(newFileName, '\0', 256);
    
    //fileName
    read(newSock, fileName, 256);
    printf("%s", fileName);
    
    //add path
    strcat(newFileName, ".");
    strcat(newFileName, serverFolder);
    strcat(newFileName, "/");
    strcat(newFileName, username);
    strcat(newFileName, "/");
    strcat(newFileName, fileName);
    printf("%s", newFileName);
    
    //open file
    FILE* fpPart;
    fpPart = fopen(newFileName, "r");
    if(fpPart == NULL){
        perror("The file is not exist!");
        char buff[256] = "false";
        send(newSock, buff, 256, 0);
        return false;
    }
    
    //file size
    fseek(fpPart, 0, 2);
    fileSize = ftell(fpPart);
    rewind(fpPart);
    fileSize++;
    sprintf(buff, "%ld", fileSize);
    printf("%s\n", buff);
    
    //send fileSize
    send(newSock, buff, 256, 0);
    printf("%ld\n", fileSize);
    
    //send file
    char buffFile[fileSize];
    memset(buffFile, '\0', fileSize);
    fread(buffFile, 1, fileSize - 1, fpPart);
    send(newSock, buffFile, fileSize, 0);
    printf("%s\n", buffFile);

    fclose(fpPart);
    return true;
}

//-------------------------------- list or LIST --------------------------------
bool list(int newSock, char* serverFolder, char* username){
    char buffList[1024], filePath[256];
    memset(buffList, '\0', 256);
    memset(filePath, '\0', 256);
    
    strcat(filePath, ".");
    strcat(filePath, serverFolder);
    strcat(filePath, "/");
    strcat(filePath, username);
    printf("%s", filePath);
    
    //list
    char buffFileName[50];
    char buffPart[50];
    int tag = 0;
    DIR *dir_ptr;
    dir_ptr = opendir(filePath);
    struct dirent *dirent_ptr;
    //struct stat my_stat;
    while((dirent_ptr = readdir(dir_ptr)) !=NULL)
    {
        //stat(dirent_ptr->d_name, &my_stat);
        if(dirent_ptr->d_name[0] == '.')
            continue;
        strcat(buffList, filePath);
        strcat(buffList, "/");
        strcat(buffList, dirent_ptr->d_name);
        strcat(buffList," ");
    }
    send(newSock, buffList, 1024, 0);
    closedir(dir_ptr);
    return true;
}

//-------------------------------- string to long --------------------------------
long toLong(char *buff) {
    long result = 0;
    int sign, offset;
    
    //symbol and offset
    if(buff[0] == '-'){
        sign = -1;
        offset = 1;
    }
    else{
        sign = 1;
        offset = 0;
    }
    
    for(int i = offset; buff[i] != '\0'; i++){
        result = result * 10 + buff[i] - '0';
    }
    
    if(sign == -1){
        result = -result;
    }

    return result;
}

//-------------------------------- encryption --------------------------------

char* encryption(char *content, char *key){
    unsigned long length = strlen(key);
    for(int i = 0; i < strlen(content); i++){
        content[i] = content[i] ^ key[i % length];
    }
    return content;
}

//-------------------------------- mkdir --------------------------------
bool mkdirServer(int newSock, char* serverFolder, char* username){
    char buff[256];
    char newFileName[256];
    
    //read file One
    read(newSock, buff, 256);
    memset(newFileName, '\0', 256);
    
    //change file path one
    strcat(newFileName, ".");
    strcat(newFileName, serverFolder);
    strcat(newFileName, "/");
    strcat(newFileName, username);
    strcat(newFileName, "/");
    strcat(newFileName, buff);
    int status;
    status = mkdir(newFileName,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    return true;
}
