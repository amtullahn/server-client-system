/********************************
* Project 12 
* Program where the server process will interact with a client process to
* provide a mechanism for copying a text file from one system to another. 
*
*****************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#include<arpa/inet.h>

#define BSIZE 128

using namespace std;

int main( int argc, char* argv[] )
{
  
  if (argc != 4)
  {
    printf( "Usage: %s <%s> <%s>\n", argv[0],argv[1],argv[2]);
    exit( 1 );
  }
  char * hostname = argv[1];
  unsigned short int port = atoi( argv[2] );

  //printf("port = %d \n", port); 

  //creates the socket
  int sock = socket( AF_INET, SOCK_STREAM, 0 );
  if (sock < 0)
  {
    perror( "socket creation error" );
    exit( 2 );
  }
  
  //takes the hostname and checks if it exists or not
  //printf("hostname: %s \n",hostname);
  struct hostent * server = gethostbyname( hostname );
  if (server == NULL)
  {
    fprintf( stderr, "Error: no such host %s\n", hostname );
    exit( 3 );
  }

  struct sockaddr_in saddr;

  bzero( &saddr, sizeof(saddr) );
  saddr.sin_family = AF_INET;

  //host to network short
  saddr.sin_port = htons(port);
  bcopy( server->h_addr, &saddr.sin_addr.s_addr, server->h_length );
  
  // connect to other process
  int stat = connect( sock, (struct sockaddr *) &saddr, sizeof(saddr) );
  if (stat < 0)
  {
    perror( "failure to connect" );
    exit( 4 );
  }
  
  //printf( "Client connected on fd %d\n\n", sock );

  char buffer[BSIZE];
  //copy contents of the filename to the buffer
  strcpy(buffer,argv[3]);
  
  // communicate with server (Send and receive data)
  //send the name of the desired file to the server 
  int send_var = send( sock, buffer, strlen(buffer), 0 );
 
  if (send_var < 0) 
  {
    perror( "failed sending" );
    exit( 5 );
  } 

  //cout << "Filename sent: " << buffer << endl;

  bzero( buffer, BSIZE );
  //this is what the server replies with
  //nrecv stores bytes 
  int bytes_received = recv( sock, buffer, BSIZE, 0 );
  
  //printf( "Message from server: %s\n", buffer);
  
  if (bytes_received < 0) 
  {
    perror( "failed receiving" );
    exit( 4 );
  }

  //checks for success/fail
  if (strcmp(buffer,"SUCCESS")==0){
    cout << "Filename opened successfully." << endl;
  }  
  else{
    fprintf(stderr, "Unable to open file");
    exit (0);
  }

  // set buffer to zero again
  bzero(buffer, BSIZE); 

  // copy PROCEED into buffer 
  memcpy(buffer,"PROCEED",7);

  //if success, send "PROCEED" 
  send_var = send(sock,buffer,strlen(buffer),0);
  
  if(send_var<0){
    perror("send");
    exit(5);
  }

  //printf("Message sent to server: %d\n,", send_var);

  // Server sends contents of input file to client
  bzero(buffer,BSIZE);
  bytes_received = recv(sock,buffer,BSIZE,0);
  write(1,buffer,bytes_received); 
  
    //have an infinite while loop to write and receive info
    while(bytes_received == BSIZE){

      bytes_received = recv(sock, buffer, BSIZE,0);
      write(1,buffer,bytes_received);
     // printf("buffer content: %d\n",buffer);
    }

  //printf( "Client closed fd %d\n\n", sock );
  
  //close socket
  close(sock);

}

