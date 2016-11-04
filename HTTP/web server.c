#include <sys/socket.h> //socket(), bind(), listen()
#include <stdio.h>
#include <netinet/in.h> //sockaddr_in
#include <string.h> //memset()
#include <arpa/inet.h> //htons()
#include <stdbool.h> //bool
#include <netdb.h>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>




//static bool readConfig(const char * cfgfile, char* value);

bool readConfig(char *configFilePath, char *key, char *value);

int main (int argc, char * argv[] )
{
    socklen_t cliAddLen=0;
    char buffer[1000];
    char tmp[200];
    char extension[100];
    char protocol[100];
    
    //create a socket
    int sock, newSock;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == -1)
    {
        printf("Failed to create a socket!");
        return -1;
    }
    
    struct sockaddr_in serAdd, cliAdd;
    
    //clear the struct
    memset(&serAdd, 0, sizeof(struct sockaddr_in));
    memset(&cliAdd, 0, sizeof(struct sockaddr_in));
    
    //initialize sin
    serAdd.sin_family=AF_INET; //Address family
    serAdd.sin_addr.s_addr = htonl(INADDR_ANY); //supply the IP address as the local machine
    
    char value[100]="0";
    
    if(readConfig("ws.conf", "listen port", value)){
        
        serAdd.sin_port=htons(atoi(value));
    }
    
    //bind this socket to an address and a port
    if(bind(sock, (const struct sockaddr *) &serAdd, sizeof(serAdd)) == -1){
        printf("Failed to bind socket to an address and a port!");
        return -1;
    }
    
    
    //listen
    listen(sock, 10);
    
    while (1) {
        char response[1024];
        //accept
        cliAddLen=sizeof(cliAdd);
        newSock=accept(sock, (struct sockaddr *)&cliAdd, &cliAddLen);
        memset(buffer, '\0', sizeof(buffer));
        int i=0, j=0;
        
        //tmp is the address
        memset(tmp, '\0', sizeof(tmp));
        recvfrom(newSock, buffer, sizeof(buffer), 0, (struct sockaddr *)(&cliAdd), &cliAddLen);
        printf("%s", buffer);
        
        //GET
        strncpy(tmp, buffer, 3);
        if(strcmp(tmp, "GET")!=0){
            sprintf(response, "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\n\r\n<html><head><title>400 Bad Request</title></head>\r\n<body><p>400 Bad Request</p></body></html>");
            send(newSock, response, strlen(response), 0 );
            memset(response, '\0', 1024);
            close(newSock);
            continue;
        }
        if(strcmp(tmp, "GET")==0){
            memset(tmp, '\0', sizeof(tmp));
           
            //skip space
            for(i=4; i<sizeof(buffer); i++){
                if(buffer[i]==' ' || buffer[i]=='\r' || buffer[i]=='\n' )
                    continue;
                else break;
            }
            
            

            
            //Document Root
            if(readConfig("ws.conf", "DocumentRoot", value)){
                strcpy(tmp, value);
            }
        
            
            //specific address
            for(j=3, i; i<sizeof(buffer); ++i){
                if(buffer[i]==' '|| buffer[i]=='\r' || buffer[i]=='\n')
                    break;
                else{
                    tmp[j]=buffer[i];
                    j++;
                }
            }
            for(j=0, i++; i<sizeof(buffer); i++){
                if(buffer[i]=='\r' || buffer[i]=='\n')
                    break;
                else{
                    protocol[j++]=buffer[i];
                }

            }
            
            //extension
            memset(extension, '\0', sizeof(extension));
            int j=0;
            for(int i=0; buffer[i]!='\n'; i++){
                if(buffer[i]=='.'){
                    for(i; buffer[i]!=' ';i++){
                        extension[j++]=buffer[i];
                    }
                    break;
                }
            }
            printf("%s", extension);
            if(readConfig("ws.conf", extension, value)==false){
                sprintf(response, "HTTP/1.1 501 Not Implemented\r\nContent-Type: text/html\r\n\r\n<html><head><title>501 Not Implemented</title></head>\r\n<body><p>501 Not Implemented</p></body></html>");
                send(newSock, response, strlen(response), 0 );
                memset(response, '\0', 1024);
                close(newSock);
                continue;

            }
            printf("%s", value);
            
            //HTTP response
            long file_size ;
            //char *content;
            
            //memset(response, '\0', sizeof(response));
            
            FILE *fp;
            fp=fopen(tmp, "r");
            if(fp == NULL){
                sprintf(response, "%s 404 NOT Found\r\nContent-Type: text/html\r\n\r\n<html><head><title>404 Not Found</title></head>\r\n<body><p>404 Not Found</p></body></html>", protocol);
                send(newSock, response, strlen(response), 0 );
                memset(response, '\0', 1024);
                close(newSock);
                continue;
            }
            fseek( fp, 0, SEEK_END );
            file_size =ftell( fp );
            fseek( fp, 0, SEEK_SET );

           
            sprintf(response, "%s 200 OK\r\n"
                    "Content-Type: %s\r\n"
                    "Content-Length: %ld\r\n\r\n"
                    , protocol, value, file_size);
            printf("%s",response);
            printf("%s", value);
            
            send(newSock, response, strlen(response), 0 );
            memset(response, '\0', 1024);
            
            while(fgets(response, 1024, fp)!=NULL){
                send(newSock, response, strlen(response), 0);
                printf("%ld\n", strlen(response));
            }
            fclose(fp);
            printf("end\n");
            close(newSock);
        }
        
            
    }
    close(sock);
  }


    

//read Configuration File
bool readConfig(char *configFilePath, char *key, char *value){
    memset(value, '\0', 100);
    int len;
    len=strlen(key);
    
    char buff[100]="";
    char keyJudge[50];
    
    
    //open the configuration File
    FILE *fp;
    fp=fopen(configFilePath,"r");
    if (fp == NULL){
        printf("Cannot open configuration file!\n");
        return false;
    }
    while(fgets(buff, 100, fp)!=NULL){
        if(buff[0]=='#'){
            memset(buff, '\0', 100);
            continue;
        }
        
        memset(keyJudge, '\0', 50);
        strncpy(keyJudge, buff, len);
        
        if(strcmp(key, keyJudge)==0){
            for(int k=0; k<strlen(buff); k++){
                if(buff[k]=='\n') buff[k]='\0';
                if(buff[k]=='\r') buff[k]='\0';
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
