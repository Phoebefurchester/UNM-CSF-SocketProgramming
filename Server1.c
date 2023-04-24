#include <netinet/in.h> //structure for storing address information
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> //for socket APIs
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <string.h> //strlen only

#define MY_PORT		8989
#define MAXBUF		256

int main(int argc , char *argv[])
{
    int sockfd , clientfd;
    struct sockaddr_in self;
	char buffer[MAXBUF];

	/*---create streaming socket---*/
    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
	{
		perror("Socket"); //print a system error message
		exit(errno);
	}
	int option = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)); //set the socket to be able to reused immediately
    printf("Socket created.\n");

	/*---initialize address/port structure---*/
	/* bzero(&self, sizeof(self));*/
	self.sin_family = AF_INET;
	self.sin_port = htons(MY_PORT);
	self.sin_addr.s_addr = INADDR_ANY;

	/*---assign a port number to the socket---*/
    if ( bind(sockfd, (struct sockaddr*)&self, sizeof(self)) != 0 )
	{
		perror("socket--bind");
		exit(errno);
	}

        puts("Bind done");

	/*---make it a "listening socket"---*/
	if ( listen(sockfd, 20) != 0 )
	{
		perror("socket--listen");
		exit(errno);
	}
        
        puts("Waiting for incoming connections...");

        struct sockaddr_in client_addr;
		socklen_t addrlen = sizeof(client_addr); //an unsigned opaque integer type of length of at least 32-bits

		/*---accept a connection (creating a data pipe)---*/
		clientfd = accept(sockfd, (struct sockaddr*)&client_addr, &addrlen);

	/*---forever... ---*/
	while (1)
	{	
		recv(clientfd, buffer, MAXBUF, 0); //receive from the client

		//convert to lowercase
		for(int i = 0; buffer[i] != '\0'; i++)
		{
			if (buffer[i] >= 'A' && buffer[i] <= 'Z') //if the character is in uppercase
			{
				buffer[i] = buffer[i] + 32; //add 32 in ascii decimal to convert into lowercase
			}
		}

		int length = strlen(buffer); //calculate the length of string

		int i = 0; //first index
		int j = length - 1; //last index
		char temp; //temporary variable

		//reverse the string
		while(i < j)
		{
			temp = buffer[i];
			buffer[i] = buffer[j];
			buffer[j] = temp;
			j--;
			i++;
		}

		send(clientfd, buffer, MAXBUF, 0); //send to client
		bzero(buffer, sizeof(buffer)); //set buffer with null values (clear buffer)
	}
	/*---close connection---*/
		close(clientfd);

	/*---clean up (should never get here!)---*/
	close(sockfd);
	return (0);
}
