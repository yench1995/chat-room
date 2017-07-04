# chat-room
This is a project about multiplayer online chat room. It is a little practising project after reading UNPv1 and APUE. 

### Usage of server and client
server: ./server <port>  
client: ./client <IP> <port> (the IP is the address of the server, the port is the listening port of server)

### The supporing command
+ login <yourname>: login with your username
+ look: check the users in the online chat-room
+ logout: logout
+ help: read the help mannul


### Keypoints
+ use epoll to manage the events in the server, set non-blocking for the socket.
