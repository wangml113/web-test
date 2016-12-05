#include <stdio.h>  
#include <string.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <arpa/inet.h>  
#include <sys/socket.h>  

#define buffer_size 100

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

class Data
{
public:
	int index;
	double x;
	double y;
	double theta;
};

int main(){  
  
    struct sockaddr_in serv_addr;  
    memset(&serv_addr, 0, sizeof(serv_addr));  
    serv_addr.sin_family = AF_INET;  //IPv4  
    serv_addr.sin_addr.s_addr = inet_addr("169.254.232.64");  //IP  
    serv_addr.sin_port = htons(8080);  //port 

    struct sockaddr_in serv_addr_send;  
    memset(&serv_addr_send, 0, sizeof(serv_addr_send));  
    serv_addr_send.sin_family = AF_INET;  //IPv4  
    serv_addr_send.sin_addr.s_addr = inet_addr("169.254.232.64");  //IP  
    serv_addr_send.sin_port = htons(8000);  //port 
    
    
    // create socket  
    int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 

    int serv_sock_send =  socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
    
    
    // bind socket with ip and port 
    // a socket that was connected may still hang around in the kernel hogging the port. Set allowing it to reuse the port.     
    int yes=1;
    if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
        error("Error in setsockopt");
    }
    if( bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        error("Error on binding");
    }

    if (setsockopt(serv_sock_send, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
        error("Error in setsockopt");
    }
    if( bind(serv_sock_send, (struct sockaddr*)&serv_addr_send, sizeof(serv_addr_send)) < 0) {
        error("Error on binding");
    }

    // listen for request 
    listen(serv_sock, 20);  
    listen(serv_sock_send, 20);
    
    while(1) {

        // receive request from client 
        struct sockaddr_in clnt_addr; 
        socklen_t clnt_addr_size = sizeof(clnt_addr);  

        // receive request from client 
        struct sockaddr_in clnt_addr_send; 
        socklen_t clnt_addr_send_size = sizeof(clnt_addr_send);  

        int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
        int clnt_sock_send = accept(serv_sock_send, (struct sockaddr*)&clnt_addr_send, &clnt_addr_send_size);

        // read from socket and send the same passage back
        char str[] = {0};  
        int leng = read(clnt_sock, str, buffer_size);

        //double array[3];
        //memcpy(array, str, sizeof(array));
		Data hearData;
		memcpy(&hearData, str, sizeof(hearData));

        printf("I hear %s \n", str); 
        //for(int i = 0; i < 3; i++) {
          //  printf("%f ", array[i]);
        //}
        //printf("\n");

        //write(clnt_sock_send, str, sizeof(array));  
		write(clnt_sock_send, str, sizeof(hearData));
        // close socket 
        close(clnt_sock_send);
        close(clnt_sock);  
    }

    close(serv_sock);
    close(serv_sock_send);    


    return 0;  
}  