#include <netinet/in.h> //structure for storing address information
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> //for socket APIs
#include <sys/types.h>
#include <unistd.h>
#include <string.h> //bzero

#define MY_PORT		8989
#define MAXBUF		256

int	ft_strcmp(char	*s1, char	*s2); //string compare function prototype

int main(int argc, char const* argv[])
{
    //client socket
    int sockD = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in servAddr;

    servAddr.sin_family = AF_INET;
    servAddr.sin_port= htons(MY_PORT); // use some unused port number
    servAddr.sin_addr.s_addr = INADDR_ANY;

    int connectStatus= connect(sockD, (struct sockaddr*)&servAddr, sizeof(servAddr));

    if (connectStatus == -1) { //connecttion failed
        printf("Error...\n");
    }

    else {
        printf("Connection established...\n"); //connection success
        while(1)
        {
            char strData[MAXBUF];

            printf("Enter your message here: ");
            bzero(strData, sizeof(strData)); //set strData with null values (clear strData)
            scanf(" %[^\n]s", strData); //read characters from the console

            //if "exit client" are read, terminate client
            if (ft_strcmp(strData, "exit client") == 0)
            {
                printf("Terminating client...\n");
                break;
            }
            send(sockD, strData, sizeof(strData), 0); //send inputs to server
            recv(sockD, strData, sizeof(strData), 0); //receive feedback from server
            printf("Server response: %s\n", strData); //server response
            printf("\n");

        }
         close(sockD);//terminate socket
    }
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
