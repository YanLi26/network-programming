//
//  dfc.c
//  distributed file system
//
//  Created by Yan Li on 11/1/16.
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
#include <openssl/md5.h>
#include <time.h>


#define VALUE_SIZE 50

int connectIpAndPort(const char *portnumber);
bool readConfig(char *configFilePath, char *key, char *value);
bool divideFile(char* fileName, char* fileNamePart1, char* fileNamePart2, char* fileNamePart3, char* fileNamePart4);
bool put(char* filePiece1, char* filePiece2, int sockDFS, char* configFilePath);
void fileNameAna(char* fileName, char* fileNamePart1, char* fileNamePart2, char* fileNamePart3, char* fileNamePart4);
bool get(char* fileName, char* fileNamePart, int sockDFS, char* configFilePat);
long toLong(char *buff);
char* list(int sockDFS, char* configFilePath);
int MD5HASH(char * filename);
char* encryption(char* content, char* key);
bool mkdirServer(char* folderName, char* configFilePath, int sockDFS);


int main(){
    char value[VALUE_SIZE];
    int sockDFS1, sockDFS2, sockDFS3, sockDFS4;
    char requestType[10], fileName[50], fileNamePart1[50], fileNamePart2[50],
    fileNamePart3[50], fileNamePart4[50];
    time_t current_time;
    time_t connect_time;

    

    //connect
    if(readConfig("dfc.conf", "ServerDFS1", value)){
        current_time = time(NULL);
        sockDFS1 = connectIpAndPort(value);
        connect_time = time(NULL);
        if(connect_time - current_time > 1){
            printf("%s\n", "server is not response in 1 second!");
            exit(-1);
        }
        printf("%d\n", sockDFS1);
    }
    else{
        perror("Cannot find it in the file!");
        exit(-1);
    }
    if(readConfig("dfc.conf", "ServerDFS2", value)){
        current_time = time(NULL);
        sockDFS2 = connectIpAndPort(value);
        if(connect_time - current_time > 1){
            printf("%s\n", "server is not response in 1 second!");
            exit(-1);
        }
        printf("%d\n", sockDFS2);
    }
    else{
        perror("Cannot find it in the file!");
        exit(-1);
    }
    if(readConfig("dfc.conf", "ServerDFS3", value)){
        current_time = time(NULL);
        sockDFS3 = connectIpAndPort(value);
        if(connect_time - current_time > 1){
            printf("%s\n", "server is not response in 1 second!");
            exit(-1);
        }
        printf("%d\n", sockDFS3);
    }
    else{
        perror("Cannot find it in the file!");
        exit(-1);
    }
    if(readConfig("dfc.conf", "ServerDFS4", value)){
        current_time = time(NULL);
        sockDFS4 = connectIpAndPort(value);
        if(connect_time - current_time > 1){
            printf("%s\n", "server is not response in 1 second!");
            exit(-1);
        }
        printf("%d\n", sockDFS4);
    }
    else{
        perror("Cannot find it in the file!");
        exit(-1);
    }
    
    while(1){
        scanf("%s", requestType);
        printf("%s\n", requestType);
        
        //put or PUT
        if(strcmp(requestType, "PUT") == 0 || strcmp(requestType, "put") == 0){
            scanf("%s", fileName);
            fileNameAna(fileName, fileNamePart1, fileNamePart2, fileNamePart3, fileNamePart4);
            
            printf("%s\n", fileName);
            printf("%s\n", fileNamePart1);
            printf("%s\n", fileNamePart2);
            printf("%s\n", fileNamePart3);
            printf("%s\n", fileNamePart4);

            //divide file
            if(divideFile(fileName, fileNamePart1, fileNamePart2, fileNamePart3, fileNamePart4) == false){
                printf("%s", "Cannot divide this file");
                exit(-1);
            }
            
            //put
            int md5Res = MD5HASH(fileName);
            if(md5Res == 0){
                put(fileNamePart1, fileNamePart2, sockDFS1, "dfc.conf");
                put(fileNamePart2, fileNamePart3, sockDFS2, "dfc.conf");
                put(fileNamePart3, fileNamePart4, sockDFS3, "dfc.conf");
                put(fileNamePart4, fileNamePart1, sockDFS4, "dfc.conf");
            }
            else if(md5Res == 1){
                put(fileNamePart1, fileNamePart2, sockDFS2, "dfc.conf");
                put(fileNamePart2, fileNamePart3, sockDFS3, "dfc.conf");
                put(fileNamePart3, fileNamePart4, sockDFS4, "dfc.conf");
                put(fileNamePart4, fileNamePart1, sockDFS1, "dfc.conf");
            }
            else if(md5Res == 2){
                put(fileNamePart1, fileNamePart2, sockDFS3, "dfc.conf");
                put(fileNamePart2, fileNamePart3, sockDFS4, "dfc.conf");
                put(fileNamePart3, fileNamePart4, sockDFS1, "dfc.conf");
                put(fileNamePart4, fileNamePart1, sockDFS2, "dfc.conf");
            }
            else if(md5Res == 3){
                put(fileNamePart1, fileNamePart2, sockDFS4, "dfc.conf");
                put(fileNamePart2, fileNamePart3, sockDFS1, "dfc.conf");
                put(fileNamePart3, fileNamePart4, sockDFS2, "dfc.conf");
                put(fileNamePart4, fileNamePart1, sockDFS3, "dfc.conf");
            }
            remove(fileNamePart1);
            remove(fileNamePart2);
            remove(fileNamePart3);
            remove(fileNamePart4);
        }
        
        //get or GET
        if(strcmp(requestType, "GET") == 0 || strcmp(requestType, "get") == 0){
            scanf("%s", fileName);
            int tag = 0;

            fileNameAna(fileName, fileNamePart1, fileNamePart2, fileNamePart3, fileNamePart4);
            FILE* fp;
            fp = fopen(fileName, "w");
            fclose(fp);
            if(!get(fileName, fileNamePart1, sockDFS1, "dfc.conf")){
                if(!get(fileName, fileNamePart1, sockDFS2, "dfc.conf")){
                    if(!get(fileName, fileNamePart1, sockDFS3, "dfc.conf")){
                        if(!get(fileName, fileNamePart1, sockDFS4, "dfc.conf")){
                            tag++;
                        }
                    }
                }
            }
            if(!get(fileName, fileNamePart2, sockDFS1, "dfc.conf")){
                if(!get(fileName, fileNamePart2, sockDFS2, "dfc.conf")){
                    if(!get(fileName, fileNamePart2, sockDFS3, "dfc.conf")){
                        if(!get(fileName, fileNamePart2, sockDFS4, "dfc.conf")){
                            tag++;
                        }
                    }
                }
            }
            if(!get(fileName, fileNamePart3, sockDFS1, "dfc.conf")){
                if(!get(fileName, fileNamePart3, sockDFS2, "dfc.conf")){
                    if(!get(fileName, fileNamePart3, sockDFS3, "dfc.conf")){
                        if(!get(fileName, fileNamePart3, sockDFS4, "dfc.conf")){
                            tag++;
                        }
                    }
                }
            }
            if(!get(fileName, fileNamePart4, sockDFS1, "dfc.conf")){
                if(!get(fileName, fileNamePart4, sockDFS2, "dfc.conf")){
                    if(!get(fileName, fileNamePart4, sockDFS3, "dfc.conf")){
                        if(!get(fileName, fileNamePart4, sockDFS4, "dfc.conf")){
                            tag++;
                        }
                    }
                }
            }
            if(tag != 0){
                printf("%s", "File is incomplete!\n");
            }
        }
        
        //list or LIST
        if(strcmp(requestType, "LIST") == 0 || strcmp(requestType, "list") == 0){
            char listJudge[2048];
            memset(listJudge, '\0', 2048);
            strcpy(listJudge, list(sockDFS1, "dfc.conf"));
            strcat(listJudge, list(sockDFS2, "dfc.conf"));
            strcat(listJudge, list(sockDFS3, "dfc.conf"));
            strcat(listJudge, list(sockDFS4, "dfc.conf"));
            printf("%s\n", listJudge);
            printf("%s", "listJudge");
            memset(fileName, '\0', 50);
        }
        
        //mkdir
        if(strcmp(requestType, "MKDIR") == 0 || strcmp(requestType, "mkdir") == 0){
            char folderName[256];
            memset(folderName, '\0', 50);
            scanf("%s", folderName);
            mkdirServer(folderName, "dfc.conf", sockDFS1);
            mkdirServer(folderName, "dfc.conf", sockDFS2);
            mkdirServer(folderName, "dfc.conf", sockDFS3);
            mkdirServer(folderName, "dfc.conf", sockDFS4);

        }
    }
    close(sockDFS1);
    close(sockDFS2);
    close(sockDFS3);
    close(sockDFS4);
    
    return 0;
}

//-------------------------------- connect IP and Port --------------------------------

int connectIpAndPort(const char *portnumber){
    struct sockaddr_in remote;
    int sock;
    
    //create a socket
    if((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        printf("unable to create socket");
        exit(-1);
    }
    
    //initialize remote
    bzero(&remote, sizeof(remote));
    remote.sin_family = AF_INET;
    remote.sin_port = htons(atoi(portnumber));
    remote.sin_addr.s_addr = inet_addr("127.0.0.1");
    bzero(&remote.sin_zero, 8);

    //connect
    if((connect(sock, (struct sockaddr *) &remote, sizeof(struct sockaddr_in))) == -1){
        printf("unable to connect the socket\n");
        exit(-1);
    }
    return sock;
}

//-------------------------------- read configuration file --------------------------------

bool readConfig(char *configFilePath, char *key, char *value){
    memset(value, '\0', VALUE_SIZE);
    unsigned long len;
    len=strlen(key);
    
    char buff[100]="";
    char keyJudge[50]; //judge if it is == key
    
    //open the configuration File
    FILE *fp;
    fp = fopen(configFilePath,"r");
    if (fp == NULL){
        printf("Cannot open configuration file!\n");
        return false;
    }
    
    //read every line from the file
    while(fgets(buff, 100, fp)!=NULL){
        memset(keyJudge, '\0', 50);
        strncpy(keyJudge, buff, len);
        
        if(strcmp(key, keyJudge)==0){
            for(int k=0; k<strlen(buff); k++){
                if(buff[k] == '\n' || buff[k] == '\r') buff[k]='\0';
            }
            int j;
            for(int i=len+1, j=0; i<strlen(buff); i++){
                if(buff[i]!='\n' || buff[i]!='\0' || buff[i]!='\r'){
                    value[j++]=buff[i];
                }
                
            }
        }
        
    }
    fclose(fp);
    return true;
}

//-------------------------------- divide file --------------------------------

bool divideFile(char* fileName, char* fileNamePart1, char* fileNamePart2, char* fileNamePart3, char* fileNamePart4){
    long fileSize;
    long partSize;
    
    FILE *fpPart1;
    FILE *fpPart2;
    FILE *fpPart3;
    FILE *fpPart4;
    FILE *fpOriginal;
    
    fpPart1 = fopen(fileNamePart1, "w");
    fpPart2 = fopen(fileNamePart2, "w");
    fpPart3 = fopen(fileNamePart3, "w");
    fpPart4 = fopen(fileNamePart4, "w");
    fpOriginal = fopen(fileName, "r");

    //if cannot open fpPart1
    if(fpPart1 == NULL ) {
        perror( "Cannot open fpPart1\n" );
        return false;
    }
    
    //if cannot open fpPart2
    if(fpPart2 == NULL ) {
        perror( "Cannot open fpPart2\n" );
        return false;
    }

    //if cannot open fpPart3
    if(fpPart3 == NULL ) {
        perror( "Cannot open fpPart3\n" );
        return false;
    }

    //if cannot open fpPart1
    if(fpPart4 == NULL ) {
        perror( "Cannot open fpPart4\n" );
        return false;
    }
    
    //if cannot open fp
    if(fpOriginal == NULL ) {
        perror( "Cannot open fp\n" );
        return false;
    }

    //fileSize
    fseek(fpOriginal, 0, 2);
    fileSize = ftell(fpOriginal);
    rewind(fpOriginal);
    partSize = fileSize/4;
    
    //write fpPart1
    for(int i = 0; i < partSize; i++){
        int c = fgetc(fpOriginal);
        fputc(c, fpPart1);
    }

    //write fpPart2
    for(int i = 0; i < partSize; i++){
        int c = fgetc(fpOriginal);
        fputc(c, fpPart2);
    }

    //write fpPart3
    for(int i = 0; i < partSize; i++){
        int c = fgetc(fpOriginal);
        fputc(c, fpPart3);
    }

    //write fpPart4
    while(1){
        int c = fgetc(fpOriginal);
        if(feof(fpOriginal)) break;
        fputc(c, fpPart4);
    }
    
    fclose(fpOriginal);
    fclose(fpPart1);
    fclose(fpPart2);
    fclose(fpPart3);
    fclose(fpPart4);
    
    return true;
}

//-------------------------------- put --------------------------------

bool put(char* filePiece1, char* filePiece2, int sockDFS, char* configFilePath){
    char value[VALUE_SIZE];
    long fileSize;
    char buff[256];
    
    //send username
    if(readConfig(configFilePath, "Username", value)){
        send(sockDFS, value, 256, 0);
        printf("%s\n", value);
    }
    else{
        printf("%s\n", "Cannot get the Username!");
        return false;
    }
    
    char * a = calloc(256,1);
    //send password
    if(readConfig(configFilePath, "Password", value)){
        a = encryption(value, "key");
        send(sockDFS, a, 256, 0);
        printf("%s\n", value);

    }
    else {
        printf("%s", "Cannot get the password\n");
        return false;
    }
    //free(a);
    
    //send request type
    send(sockDFS, "PUT", 256, 0);
    
    //open file
    FILE *fpPiece1;
    FILE *fpPiece2;
    
    fpPiece1 = fopen(filePiece1, "r");
    if(fpPiece1 == NULL ) {
        perror( "Cannot open fpPart2\n" );
        return false;
    }
    
    fpPiece2 = fopen(filePiece2, "r");
    if(fpPiece2 == NULL ) {
        perror( "Cannot open fpPart2\n" );
        return false;
    }
    
    //fileSize One
    fseek(fpPiece1, 0, 2);
    fileSize = ftell(fpPiece1);
    rewind(fpPiece1);
    fileSize++;
    sprintf(buff, "%ld", fileSize);
    printf("%s\n", buff);

    send(sockDFS, buff, 256, 0);
    printf("%ld\n", fileSize);
    
    //send fpPiece1
    char buffOne[fileSize];
    memset(buffOne, '\0', fileSize);
    fread(buffOne, 1, fileSize - 1, fpPiece1);
    send(sockDFS, filePiece1, 256, 0);
    printf("%s\n", filePiece1);
    send(sockDFS, buffOne, fileSize, 0);
    printf("%s\n", buffOne);
    
    //fileSize Two
    fseek(fpPiece2, 0, 2);
    fileSize = ftell(fpPiece2);
    rewind(fpPiece2);
    fileSize++;
    sprintf(buff, "%ld", fileSize);
    printf("%s\n", buff);
    send(sockDFS, buff, 256, 0);
    printf("%ld\n", fileSize);

    
    //send fpPiece2
    char buffTwo[fileSize];
    memset(buffTwo, '\0', fileSize);
    fread(buffTwo, 1, fileSize - 1, fpPiece2);
    send(sockDFS, filePiece2, 256, 0);
    send(sockDFS, buffTwo, fileSize, 0);
    
    fclose(fpPiece1);
    fclose(fpPiece2);
    
    return true;
}

//-------------------------------- get --------------------------------

bool get(char* fileName, char* fileNamePart, int sockDFS, char* configFilePath){
    char value[VALUE_SIZE];
    long fileSize;
    char buff[256];
    
    //send username
    if(readConfig(configFilePath, "Username", value)){
        send(sockDFS, value, 256, 0);
        printf("%s\n", value);
    }
    else{
        printf("%s\n", "Cannot get the Username!");
        return false;
    }
    
    //send password
    if(readConfig(configFilePath, "Password", value)){
        send(sockDFS, value, 256, 0);
        printf("%s\n", value);
    }
    else{
        printf("%s\n", "Cannot get the Pasword!");
        return false;
    }
    
    //get fileName
    send(sockDFS, "GET", 256, 0);
    printf("%s\n", "GET");
    send(sockDFS, fileNamePart, 256, 0);
    printf("%s\n", fileNamePart);
    
    //read file size
    read(sockDFS, buff, 256);
    printf("%s\n", buff);
    //if the file is missed
    if(strcmp(buff, "false") == 0){
        printf("%s", "Invalid Username/Password. Please try again.");
        return false;
    }
    fileSize = toLong(buff);
    printf("%ld\n", fileSize);
    
    //read file
    char buffFile[fileSize];
    read(sockDFS, buffFile, fileSize);
    printf("%s\n", buffFile);

    //write it in the file
    FILE* fpReturn;
    fpReturn = fopen(fileName, "a");
    
    if(fpReturn == NULL){
        perror("Cannot open fpOne");
        return false;
    }
    else if(fpReturn != NULL){
        fputs(buffFile, fpReturn);
    }
    fclose(fpReturn);

    return true;
}

//-------------------------------- list --------------------------------

char* list(int sockDFS, char* configFilePath){
    char buffList[1024];
    char value[VALUE_SIZE];
    memset(buffList, '\0', 1024);
    
    //send username
    if(readConfig(configFilePath, "Username", value)){
        send(sockDFS, value, 256, 0);
        printf("%s\n", value);
    }
    else{
        printf("%s\n", "Cannot get the Username!");
        exit(-1);
    }
    
    //send password
    if(readConfig(configFilePath, "Password", value)){
        send(sockDFS, value, 256, 0);
        printf("%s\n", value);
    }
    else{
        printf("%s\n", "Cannot get the Pasword!");
        exit(-1);
    }
    
    //get fileName
    send(sockDFS, "LIST", 256, 0);
    printf("%s\n", "LIST");
   
    read(sockDFS, buffList, 1024);
    printf("%s\n", buffList);

    
//    //delete useless information
//    memset(buffReturn, '\0', 1024);
//    for(int i = 0, j = 0; i < strlen(buffList); i++){
//        if(buffList[i] == '/'){
//            continue;
//        }
//        if(strncmp(buffList + i, "./DFS", 5)){
//            i += 5;
//            continue;
//        }
//        if(strncmp(buffList + i, username, strlen(username))){
//            i += strlen(username);
//            continue;
//        }
//        buffReturn[j] = buffList[i];
//        j++;
//    }
    return buffList;
}

//-------------------------------- fileName --------------------------------

void fileNameAna(char* fileName, char* fileNamePart1, char* fileNamePart2, char* fileNamePart3, char* fileNamePart4){
    memset(fileNamePart1, '\0', 50);
    memset(fileNamePart2, '\0', 50);
    memset(fileNamePart3, '\0', 50);
    memset(fileNamePart4, '\0', 50);

    strcpy(fileNamePart1, fileName);
    strcat(fileNamePart1, ".1");
    printf("%s\n", fileNamePart1);
    strcpy(fileNamePart2, fileName);
    strcat(fileNamePart2, ".2");
    printf("%s\n", fileNamePart2);
    strcpy(fileNamePart3, fileName);
    strcat(fileNamePart3, ".3");
    printf("%s\n", fileNamePart3);
    strcpy(fileNamePart4, fileName);
    strcat(fileNamePart4, ".4");
    printf("%s\n", fileNamePart4);
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

//-------------------------------- MD5HASH --------------------------------

int MD5HASH(char* filename){
    unsigned char buff[1024];
    int md5hash, bytes;
    unsigned char md5[MD5_DIGEST_LENGTH];
    FILE* MD5File = fopen(filename, "rb");
    MD5_CTX md5Context;

    if (MD5File == NULL) {
        printf ("%s can't be opened.\n", filename);
        return 0;
    }
    MD5_Init (&md5Context);
    while ((bytes = fread (buff, 1, 1024, MD5File)) != 0){
        MD5_Update (&md5Context, buff, bytes);
    }
    MD5_Final (md5,&md5Context);
    
    md5hash = md5[MD5_DIGEST_LENGTH - 1] % 4;
    printf ("\n%d\n", md5hash);
    printf ("%s\n", filename);
    fclose (MD5File);
    return md5hash;
}

//-------------------------------- encryption --------------------------------

char* encryption(char *content, char *key){
    char value[256];
    memset(value, '\0', 256);
    unsigned long length = strlen(key);
    for(int i = 0; i < strlen(content); i++){
        value[i] = content[i] ^ key[i % length];
    }
    return value;
}

//-------------------------------- mkdir --------------------------------

bool mkdirServer(char* folderName, char* configFilePath, int sockDFS){
    char buffList[1024];
    char value[VALUE_SIZE];
    memset(buffList, '\0', 1024);
    
    //send username
    if(readConfig(configFilePath, "Username", value)){
        send(sockDFS, value, 256, 0);
        printf("%s\n", value);
    }
    else{
        printf("%s\n", "Cannot get the Username!");
        exit(-1);
    }
    
    //send password
    if(readConfig(configFilePath, "Password", value)){
        send(sockDFS, value, 256, 0);
        printf("%s\n", value);
    }
    else{
        printf("%s\n", "Cannot get the Pasword!");
        exit(-1);
    }
    
    //get fileName
    send(sockDFS, "MKDIR", 256, 0);
    printf("%s\n", "MKDIR");
    
    send(sockDFS, folderName, 256, 0);
    return true;
}

