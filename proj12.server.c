/*************************
* Project 12 
*
* Server process will interact with a client process to
* copy a text file from one system to another 
*
**************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <fstream>
#include <iostream>
#include <fcntl.h>

#define BSIZE 128

using namespace std;
using std::ifstream;

int main( int argc, char* argv[] )
{
  int nsend = 0;

  // create the socket
  int listen_sd = socket( AF_INET, SOCK_STREAM, 0 );
  if (listen_sd < 0) 
  {
    perror( "socket" );
    exit( 2 );
  }

  struct sockaddr_in saddr;
  
  //setting info for the port 
  bzero( &saddr, sizeof(saddr) );
  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(0);
  saddr.sin_addr.s_addr = htonl( INADDR_ANY );
  
  // bind socket to port
  int bstat = bind( listen_sd, (struct sockaddr *) &saddr, sizeof(saddr) );
  if (bstat < 0)
  {
    perror( "bind" );
    exit( 3 );
  }

  
  // getting server name 
  char name[256];
  gethostname(name,256);
  struct hostent* host_name;
  host_name = gethostbyname(name);

  //getting the  port number
  socklen_t saddr_size = sizeof(saddr);
  getsockname(listen_sd,(struct sockaddr *) &saddr, &saddr_size);
 
  printf("\n%s %d\n\n",host_name->h_name, ntohs(saddr.sin_port)); 

  // listen for connection requests
  int lstat = listen( listen_sd, 5 );
  if (lstat < 0)
  {
    perror( "listen" );
    exit( 4 );
  }

  //printf( "Server listening on fd %d\n\n", listen_sd );
 
  struct sockaddr_in caddr;
  unsigned int clen = sizeof(caddr);

  // accept any connect requests, this will create the socket
  int comm_sd = accept( listen_sd, (struct sockaddr *) &caddr, &clen );
  if (comm_sd < 0)
  {
    perror( "accept" );
    exit( 5 );
  }

  //printf( "Server connected on fd %d\n\n", comm_sd );

  char buffer[BSIZE];

  bzero( buffer, BSIZE );

  // communicate with client, receive data to read/write
  int nrecv = recv( comm_sd, buffer, BSIZE, 0 );
  if (nrecv < 0) 
  {
    perror( "recv" );
    exit( 8 );
  }
  //printf( "Server received %d bytes\n", nrecv );
  //printf( "Message: %s\n", buffer );
     
  //open the file
  int file = open(buffer,O_RDONLY);

  if (file ==-1){
  
    char open_fail[BSIZE] = {"FAILURE"};
    nsend = send(comm_sd, open_fail, strlen(open_fail),0);
    if (nsend<0){
      perror("error");
      exit(9);
    }
    exit(0);
  }
  char open_success[BSIZE] = {"SUCCESS"};

  nsend = send(comm_sd,open_success,strlen(open_success),0);
  if(nsend<0){
    perror("send");
    exit(9);
  }
    
  // receive the data and read file
  bzero(buffer,BSIZE);
  nrecv = recv(comm_sd,buffer,BSIZE,0);
  if (nrecv<0){
    perror("recv");
    exit(8);
  }
  int nread = 0;

  // check if client file sent "PROCEED"
  if (strcmp(buffer, "PROCEED")==0){
    //send contents of file by processing byte by byte through loop
     while(1)
     {
        bzero(buffer,BSIZE); 
        //read from file
        nread = read(file,buffer,BSIZE);
      
        if (nread == 0){
          break;
        }
        //sending the name
        nsend = send(comm_sd,buffer,nread,0);

        if(nsend <0)
        {
         perror("send");
          exit(9);
        } 
     }
   }

  close( comm_sd );
  
}
