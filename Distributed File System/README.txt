1. MD5: I use MD5 to know which file piece to which server.
2. LIST: type LIST or list, the server will send the file list to client
3. GET: type GET filename or get filename to get file from server.
4. PUT: type PUT filename or put filename to put file into server.
5. If the password or username is wrong, it will print some message to user.
6. mkdir folder: if it doesn’t have a folder with username, server will use mkdir to create a new folder named username.
7. file piece name: ./DFS1/Alice/.1.txt.3
8. timeout: if client use more than 1 second to link to sever, it will exit.
9. fork(): I use fork() to handle another connection.
10. kill server: If the server is closed, client will send requirement to other server for file pieces.
11. encryption: I use encryption function to encrypt password when client put some file to server.
12. mkdir subfolder: type MKDIR or mkdir and folder name, client can create any folder in its folder stored in server.
13. upgraded GET command: when user cannot find a file in one server, it will find it in another.