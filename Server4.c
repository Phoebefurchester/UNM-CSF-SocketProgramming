#include <netinet/in.h> //structure for storing address information
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> //for socket APIs
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <string.h> //strlen only
#include <arpa/inet.h> //inet_ntoa
#include <time.h>

#define MY_PORT		8989
#define MAXBUF		256

int	ft_strcmp(char	*s1, char	*s2); //string compare function prototype
char	*ft_strcat(char	*dest, char *src); //string concatenate function prototype
char	*ft_strcpy(char *dest, char *src); //string copy function prototype
void timeconvert(int x, int y, struct tm *ptm, char* buffer, int size); //function prototype to convert local time to other timezone

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
		printf("Port number: %d\n", MY_PORT);//display port number targeted by the client's request

		
	/*---forever... ---*/
	while(1) //handles multiple requests
	{
		recv(clientfd, buffer, MAXBUF, 0); //receive from the client

        time_t rawtime = time(NULL); //returns the time since 00:00:00 UTC, January 1, 1970 (Unix timestamp) in seconds
		struct tm *ptm = localtime(&rawtime);  //to fill the tm structure with the values that represent the corresponding local time


		if (ft_strcmp(buffer, "exit server") == 0) //terminate connection if the client inputs "exit server" 
		{
			printf("Terminating connection...\n");
			break; 
		}
		if (ft_strcmp(buffer, "DATE") == 0) //respond with current day, date and time
		{
			bzero(buffer, sizeof(buffer)); //clears off buffer which contains the string "DATE"
			strftime(buffer, MAXBUF, "%a %b %d %T %Y %Z",ptm); //formats the time represented in the structure ptm according to the formatting rules defined in format and stored into buffer
			printf("Current Day, Date and Time: %s\n", buffer);
			printf("\n");
		}
        else if (ft_strcmp(buffer, "TIME") == 0) //respond with local time
		{
			bzero(buffer, sizeof(buffer)); //clears off buffer which contains the string "TIME"
			strftime(buffer, MAXBUF, "%T %Z",ptm); //formats the time represented in the structure ptm according to the formatting rules defined in format and stored into buffer
			printf("Current Time: %s\n", buffer);
			printf("\n");
		}
        else if (ft_strcmp(buffer, "TIME PST") == 0) //respond with PST time
		{
			timeconvert(-16, -8, ptm, buffer, sizeof(buffer)); //16 hours behind local time and 8 hours behind GMT
		}
        else if (ft_strcmp(buffer, "TIME MST") == 0) //respond with MST time
		{
			timeconvert(-15, -7, ptm, buffer, sizeof(buffer)); //15 hours behind local time and 7 hours behind GMT
		}
        else if (ft_strcmp(buffer, "TIME CST") == 0) //respond with CST time
		{
			timeconvert(-14, -6, ptm, buffer, sizeof(buffer)); //14 hours behind local time and 6 hours behind GMT
		}
        else if (ft_strcmp(buffer, "TIME EST") == 0) //respond with EST time
		{
			timeconvert(-13, -5, ptm, buffer, sizeof(buffer));//13 hours behind local time and 5 hours behind GMT
		}
        else if (ft_strcmp(buffer, "TIME GMT") == 0) //respond with GMT time
		{
			timeconvert(-8, +0, ptm, buffer, sizeof(buffer));//8 hours behind local time and 0 hours behind GMT
		}
        else if (ft_strcmp(buffer, "TIME CET") == 0) //respond with CET time
		{
			timeconvert(-7, +1, ptm, buffer, sizeof(buffer));//7 hours behind local time and 1 hours ahead GMT
		}
        else if (ft_strcmp(buffer, "TIME MSK") == 0) //respond with MSK time
		{
			timeconvert(-5, +3, ptm, buffer, sizeof(buffer)); //5 hours behind local time and 3 hours ahead GMT
		}
         else if (ft_strcmp(buffer, "TIME JST") == 0) //respond with JST time
		{
			timeconvert(+1, +9, ptm, buffer, sizeof(buffer));//1 hours ahead local time and 9 hours ahead GMT
		}
         else if (ft_strcmp(buffer, "TIME AEDT") == 0) //respond with AEDT time
		{
			timeconvert(+3, +11, ptm, buffer, sizeof(buffer));//3 hours ahead local time and 11 hours ahead GMT
		}
		else
		{
			bzero(buffer, sizeof(buffer)); //set buffer with null values (clear buffer)
			strcpy(buffer, "ERROR"); //copy "ERROR" string into buffer
			printf("%s\n",buffer);
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
//function to concatenate string
char	*ft_strcat(char	*dest, char *src)
{
	int i;
	int j;

	i = 0;
	while (dest[i] != '\0')
	{
		i++;
	}
	j = 0;
	while (src[j] != '\0')
	{
		dest[i + j] = src[j];
		j++;
	}
	dest[i+j] = '\0';
	return (dest);
}
//function to copy string to array
char	*ft_strcpy(char *dest, char *src)
{
	char *dest_init;
	dest_init = dest;
	while (*src != '\0')
	{
		*dest = *src;
		dest++;
		src++;
	}
	*dest = '\0';
	return(dest_init);
}

void timeconvert(int x, int y, struct tm *ptm, char* buffer, int size)
{
	char hour[100] ={0};
	char min[100] ={0};
	char sec[100] ={0};
	char* string1;
	char* string2;

	int hr = ptm->tm_hour + x; //hr = local time + hours before (-x) or ahead local time (x)

	if (hr < 0) // if hr is a negative value
	{
		hr += 24; // add 24 hours 
	}

	bzero(buffer, size); //set buffer with null values (clear buffer)
	sprintf(hour, "%02d:", hr); //convert integer hr into a string and store into array hour
	sprintf(min, "%02d:", ptm->tm_min); //convert tm_min into a string and store into array min
	sprintf(sec, "%02d %-+20d", ptm->tm_sec, y); //convert tm_sec and y into a string and store into array sec
	string1 = ft_strcat(hour, min); // concatenate hour and min and store into string1
	string2 = ft_strcat(string1, sec); // concatenate string1 and sec and store into string2
	ft_strcpy(buffer, string2);//copy string2 into buffer
	printf("Current Time: %02d:%02d:%02d %-+20d\n", hr, ptm->tm_min, ptm->tm_sec, y);
	printf("\n");
}