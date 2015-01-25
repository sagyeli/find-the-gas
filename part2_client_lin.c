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
	char * serverIPAddress;
	char * opponentIPAddress = NULL;
	int listenfd = 0, connfd = 0, sockfd = 0, sockfd2 = 0, n = 0, i, j;
	char recvBuff[1024], recvBuff2[1024], sendBuff[1024], sendBuff2[1024], * buffer;
	int input[2], inputOption;
	char messageToOpponent[1024];
	struct sockaddr_in serv_addr;
	struct sockaddr_in serv_addr2;
	struct sockaddr_in serv_addr3;

	if (argc < 2)
	{
		printf("ERROR: no IP address was entered!\r\n");
		return 0;
	}

	serverIPAddress = argv[1];

	memset(recvBuff, '0', sizeof(recvBuff));
	memset(recvBuff2, '0', sizeof(recvBuff2));
	memset(sendBuff, '0', sizeof(sendBuff));
	memset(sendBuff2, '0', sizeof(sendBuff2));

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

    if(inet_pton(AF_INET, serverIPAddress, &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        return 1;
    } 

    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("\n Error : Connect Failed \n");
       return 1;
    }

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr2, '0', sizeof(serv_addr2)); 

    serv_addr2.sin_family = AF_INET;
    serv_addr2.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr2.sin_port = htons(argc > 2 ? atoi(argv[2]) : PORT_NUMBER);

    bind(listenfd, (struct sockaddr*)&serv_addr2, sizeof(serv_addr2)); 

    listen(listenfd, 10);

    while ((n = read(sockfd, recvBuff, sizeof(recvBuff)-1)) > 0)
    {
	recvBuff[n] = 0;
	buffer = replace_str(replace_str(replace_str(replace_str(replace_str(replace_str(replace_str(recvBuff, "'YOUR_TURN'", ""), "'NOT_YOUR_TURN'", ""), "'SPOT_TYPE_0'", ""), "'SPOT_TYPE_1'", ""), "'SPOT_TYPE_2'", ""), "'OPPONENT_IP'", ""), "'NO_MESSAGE'", "");
	if (strstr(recvBuff, "':") != NULL)
	{
		buffer = replace_str(buffer, buffer, "");
	}

	if(fputs(buffer, stdout) == EOF)
	{
		printf("\n Error : Fputs error\n");
	}

	if (strstr(recvBuff, "'OPPONENT_IP'") != NULL)
	{
		opponentIPAddress = replace_str(recvBuff, "'OPPONENT_IP':", "");
		printf("The opponent IP address is: %s\r\n", opponentIPAddress);

		if((sockfd2 = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		{
			printf("\n Error : Could not create socket \n");
			return 1;
		}

		memset(&serv_addr3, '0', sizeof(serv_addr3)); 

		serv_addr3.sin_family = AF_INET;
		serv_addr3.sin_port = htons(argc > 3 ? atoi(argv[3]) : PORT_NUMBER); 

		if(inet_pton(AF_INET, opponentIPAddress, &serv_addr3.sin_addr)<=0)
		{
			printf("\n inet_pton error occured\n");
			return 1;
		}

		if( connect(sockfd2, (struct sockaddr *)&serv_addr3, sizeof(serv_addr3)) < 0)
		{
			printf("\n Error : Connect Failed\n");
			return 1;
		}

		connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
	}

	if (opponentIPAddress)
	{
		snprintf(sendBuff2, sizeof(sendBuff2), "'NO_MESSAGE'");
		write(connfd, sendBuff2, strlen(sendBuff2));
	}

	if (strstr(recvBuff, "'YOUR_TURN'") != NULL)
	{
		isMyTurn = 1;

		printf("Yeah, it's my turn!\r\n");

		printf("Should I make a move (1) or send a message to the opponent (2)? ");
		scanf("%d", & inputOption);
		if (inputOption == 1)
		{
			printf("Enter row: ");
			scanf("%d", & input[0]);

			printf("Enter column: ");
			scanf("%d", & input[1]);

			snprintf(sendBuff, sizeof(sendBuff), "%d,%d\r\n", input[0], input[1]);
			write(sockfd, sendBuff, strlen(sendBuff));
		}
		else
		{
			printf("Enter a message to the opponent: ");
			scanf("%s", messageToOpponent);

			snprintf(sendBuff, sizeof(sendBuff), "'USER_MESSAGE'");
			write(sockfd, sendBuff, strlen(sendBuff));

			snprintf(sendBuff2, sizeof(sendBuff2), "The opponent says: %s\r\n", messageToOpponent);
			write(connfd, sendBuff2, strlen(sendBuff2));
		}
	}
	else if (strstr(recvBuff, "'NOT_YOUR_TURN'") != NULL)
	{
		isMyTurn = 0;

		printf("Bummer, it's the opponent's turn...\r\n");
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

	if (opponentIPAddress)
	{
		n = read(sockfd2, recvBuff2, sizeof(recvBuff)-1);
		recvBuff2[n] = 0;

		if(fputs(replace_str(recvBuff2, "'NO_MESSAGE'", ""), stdout) == EOF)
		{
			printf("\n Error : Fputs error\n");
		}
	}

	sleep(1);
	
    } 

    if(n < 0)
    {
        printf("\n Read error \n");
    } 

    return 0;
}