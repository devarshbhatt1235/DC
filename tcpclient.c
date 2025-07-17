#include <sys/types.h> 
#include <sys/socket.h> 
#include <stdio.h> 
#include <netinet/in.h> 
#include <string.h> 
#define MAXLINESIZE 100 
#define SERV_PORT 5555 
int main(int argc, char** argv){ int connectsd; 
char sendBuffer[MAXLINESIZE+1]; char recvBuffer[MAXLINESIZE+1]; struct sockaddr_in servaddr; int noBytesRead=0; 

fprintf(stderr, "Usage: %s IP-Address\n", argv[0]); 
if(argc!=2){ 
exit(-1); 
} 
/*Create socket*/ 
if((connectsd=socket(AF_INET,SOCK_STREAM,0))<0){ 
fprintf(stderr, "Cannot create socket\n"); 
exit(-1); 
} 
/*Initialize socket address structure*/ 
bzero(&servaddr,sizeof(servaddr)); 
servaddr.sin_family=AF_INET; 
servaddr.sin_port=htons(SERV_PORT); 

/*assign server address in socket address structure*/ 
if(inet_pton(PF_INET, argv[1],&servaddr.sin_addr)<=0){ 
fprintf(stderr, "Error in inet_pton\n"); 
exit(-1); 
} 
/*Get connected with the server*/ 
if(connect(connectsd, (struct sockaddr*)&servaddr,sizeof(servaddr))<0){ 
fprintf(stderr, "Error in connect\n"); 
exit(-1); 
} 
/*Read message from user through keyboard*/ 
for(;gets(send Buffer)!= NULL;){ 
/*Send the message to the server*/ 
write(connectsd,send Buffer, strlen(send Buffer)+1); 
if(noBytesRead=read (connectsd,recvBuffer,sizeof(recvBuffer))<0) 
exit(0); 
/*Display what the server sent in reply*/ 
fprintf(stdout, "%s\n",recvBuffer); 
} 
return 0; 
} 
