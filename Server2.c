#include <netinet/in.h> //structure for storing address information
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> //for socket APIs
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <string.h> //strlen only
#include <arpa/inet.h> //inet_ntoa

#define MY_PORT		8989
#define MAXBUF		256

int	ft_strcmp(char	*s1, char	*s2); //string compare function prototype

int main(int argc , char *argv[])
{
    int sockfd , clientfd;
    struct sockaddr_in self;
	char buffer[MAXBUF];

	/*---create streaming socket---*/
    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
	{
		perror("Socket");
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

		//net_ntoa() accepts an Internet address expressed as a 32-bit quantity in network byte order and returns a string expressed in dotted-decimal notation
		printf("IP address: %s\n", inet_ntoa(client_addr.sin_addr)); //IP address of the client
		printf("Port number: %d\n",	client_addr.sin_port);//display port number targeted by the client's request

		
	/*---forever... ---*/
	while(1) //handles multiple requests
	{
		recv(clientfd, buffer, MAXBUF, 0); //receive from the client

		if (ft_strcmp(buffer, "exit server") == 0) //terminate connection if the client inputs "exit server" 
		{
			printf("Terminating connection...\n");
			break; 
		}
 
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

		int message_length = strlen(buffer); //calculate the length of the message
		printf("The length of the message: %d\n", message_length);
		printf("\n");

		send(clientfd, buffer, MAXBUF, 0); //send to client
		bzero(buffer, sizeof(buffer)); //set buffer with null values (clear buffer)
	}
	/*---close connection---*/
	close(clientfd);
	/*---clean up (should never get here!)---*/
	close(sockfd);
	return (0);
}

//function to compare the strings
int	ft_strcmp(char	*s1, char	*s2)
{
	int	i;

	i = 0;

	while (s1[i] == s2[i] && s1[i] != '\0' && s2[i] != '\0')
	{
		i++;
	}
	return (s1[i] - s2[i]);
}