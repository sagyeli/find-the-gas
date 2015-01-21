#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#define PORT_NUMBER 30000

#define SEA_SIZE 6

int knownSea[SEA_SIZE][SEA_SIZE];
int isMyTurn;

char *replace_str(char *str, char *orig, char *rep)
{
	static char buffer[9096];
	char *p;

	if(!(p = (char *)strstr(str, orig)))  // Is 'orig' even in 'str'?
		return str;

	strncpy(buffer, str, p-str); // Copy characters from 'str' start to 'orig' st$
	buffer[p-str] = '\0';

	sprintf(buffer+(p-str), "%s%s", rep, p+strlen(orig));

	return buffer;
}

void showKnownSea()
{
	int i, j;

	for (i = 0 ; i < SEA_SIZE ; i++)
	{
		for (j = 0 ; j < SEA_SIZE ; j++)
		{
			printf("%d", knownSea[i][j]);
		}
		printf("\r\n");
	}

	isMyTurn = 0;
}

int main(int argc, char *argv[])
{
	int sockfd = 0, n = 0, i, j;
	char recvBuff[1024], sendBuff[1024], * buffer;
	int input[2];
	struct sockaddr_in serv_addr;

	memset(recvBuff, '0', sizeof(recvBuff));
	memset(sendBuff, '0', sizeof(sendBuff));

	for (i = 0 ; i < SEA_SIZE ; i++)
	{
		for (j = 0 ; j < SEA_SIZE ; j++)
		{
			knownSea[i][j] = -1;
		}
	}

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    } 

    memset(&serv_addr, '0', sizeof(serv_addr)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT_NUMBER); 

    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        return 1;
    } 

    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("\n Error : Connect Failed \n");
       return 1;
    } 

    while ( (n = read(sockfd, recvBuff, sizeof(recvBuff)-1)) > 0)
    {
	recvBuff[n] = 0;
	buffer = replace_str(replace_str(replace_str(replace_str(replace_str(recvBuff, "'YOUR_TURN'", ""), "'NOT_YOUR_TURN'", ""), "'SPOT_TYPE_0'", ""), "'SPOT_TYPE_1'", ""), "'SPOT_TYPE_2'", "");
	if (strstr(recvBuff, "':") != NULL)
	{
		buffer = replace_str(buffer, buffer, "");
	}

	if(fputs(buffer, stdout) == EOF)
	{
		printf("\n Error : Fputs error\n");
	}

	if (strstr(recvBuff, "'YOUR_TURN'") != NULL)
	{
		isMyTurn = 1;

		printf("Yeah, it's my turn!\r\n");

		printf("Enter row: ");
		scanf("%d", & input[0]);

		printf("Enter column: ");
		scanf("%d", & input[1]);

		snprintf(sendBuff, sizeof(sendBuff), "%d,%d\r\n", input[0], input[1]);
		write(sockfd, sendBuff, strlen(sendBuff));
	}
	else if (strstr(recvBuff, "'NOT_YOUR_TURN'") != NULL)
	{
		isMyTurn = 0;

		printf("Bummer, it's not my turn...\r\n");
	}
	else if (strstr(recvBuff, "'SPOT_TYPE_0'") != NULL)
	{
		if (isMyTurn)
		{
			printf("I didn't find anything. What a waste of time!\r\n");
		}
		else
		{
			printf("Yes! the other guy did not find anything. Good for me!\r\n");
		}

		input[0] = atoi((const char *)strtok(replace_str(recvBuff, "'SPOT_TYPE_0':", ""), ","));
		input[1] = atoi((const char *)strtok(NULL, ","));
		knownSea[input[0]][input[1]] = 0;

		printf("This is what I know about the sea:\r\n");
		showKnownSea();
	}
	else if (strstr(recvBuff, "'SPOT_TYPE_1'") != NULL)
	{
		if (isMyTurn)
		{
			printf("I found a small gas pocket. Nice!\r\n");
		}
		else
		{
			printf("The other guy found something... :(!\r\n");
		}

		input[0] = atoi((const char *)strtok(replace_str(recvBuff, "'SPOT_TYPE_1':", ""), ","));
		input[1] = atoi((const char *)strtok(NULL, ","));
		knownSea[input[0]][input[1]] = 1;

		printf("This is what I know about the sea:\r\n");
		showKnownSea();
	}
	else if (strstr(recvBuff, "'SPOT_TYPE_2'") != NULL)
	{
		if (isMyTurn)
		{
			printf("Awsome! It looks like I hit the jackpot!!!\r\n");
		}
		else
		{
			printf("Oh no, the other guy found something big!!\r\n");
		}

		input[0] = atoi((const char *)strtok(replace_str(recvBuff, "'SPOT_TYPE_2':", ""), ","));
		input[1] = atoi((const char *)strtok(NULL, ","));
		knownSea[input[0]][input[1]] = 2;

		printf("This is what I know about the sea:\r\n");
		showKnownSea();
	}

	sleep(1);
	
    } 

    if(n < 0)
    {
        printf("\n Read error \n");
    } 

    return 0;
}