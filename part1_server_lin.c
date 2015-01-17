#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>

#define PORT_NUMBER 30000

#define SEA_SIZE 6
#define NUMBER_OF__USERS 2
 
struct Environment
{
	int sea[SEA_SIZE][SEA_SIZE];
	int users[NUMBER_OF__USERS];
	int number_of_active_users;
};

void init(struct Environment * environment)
{
	int i, j;
	
	srand(time(NULL));
	for (i = 0 ; i < SEA_SIZE ; i++)
	{
		for (j = 0 ; j < SEA_SIZE ; j++)
		{
			if (rand() % 2 && i < SEA_SIZE-1 && j < SEA_SIZE-1)
			{

				if (
					environment->sea[i][j] != 0 && environment->sea[i][j] != 1 && 
					environment->sea[i+1][j] != 0 && environment->sea[i+1][j] != 1 && 
					environment->sea[i][j+1] != 0 && environment->sea[i][j+1] != 1 &&
					environment->sea[i+1][j+1] != 0 && environment->sea[i+1][j+1] != 1 && 
					((i == 0 && j == 0) || environment->sea[i-1][j-1] != 1) &&
					((j == 0) || environment->sea[i][j-1] != 1) &&
					((i == SEA_SIZE-1 && j == 0) || environment->sea[i+1][j-1] != 1) &&
					((i == SEA_SIZE-2 && j == 0) || environment->sea[i+2][j-1] != 1) &&
					((i == SEA_SIZE-2) || environment->sea[i+2][j] != 1) &&
					((i == SEA_SIZE-2 && j == SEA_SIZE-1) || environment->sea[i+2][j+1] != 1) &&
					((i == SEA_SIZE-2 && j == SEA_SIZE-2) || environment->sea[i+2][j+2] != 1) &&
					((i == SEA_SIZE-1 && j == SEA_SIZE-2) || environment->sea[i+1][j+2] != 1) &&
					((j == SEA_SIZE-2) || environment->sea[i][j+2] != 1) &&
					((i == 0 && j == SEA_SIZE-2) || environment->sea[i-1][j+2] != 1) &&
					((i == 0 && j == SEA_SIZE-1) || environment->sea[i-1][j+1] != 1) &&
					((i == 0) || environment->sea[i-1][j] != 1)
					)
				{
					environment->sea[i][j] = 1;
					environment->sea[i][j+1] = 1;
					environment->sea[i+1][j] = 1;
					environment->sea[i+1][j+1] = 1;
				}
				else if (environment->sea[i][j] != 1)
				{
					environment->sea[i][j] = 0;
				}
			}
			else
			{
				if (
					environment->sea[i][j] != 0 && environment->sea[i][j] != 1 && 
					((i == 0 && j == 0) || environment->sea[i-1][j-1] != 1) &&
					((j == 0) || environment->sea[i][j-1] != 1) &&
					((i == SEA_SIZE-1 && j == 0) || environment->sea[i+1][j-1] != 1) &&
					((i == SEA_SIZE-1) || environment->sea[i+1][j] != 1) &&
					((i == SEA_SIZE-1 && j == SEA_SIZE-1) || environment->sea[i+1][j+1] != 1) &&
					((j == SEA_SIZE-1) || environment->sea[i][j+1] != 1) &&
					((i == 0 && j == SEA_SIZE-1) || environment->sea[i-1][j+1] != 1) &&
					((i == 0) || environment->sea[i-1][j] != 1)
					)
				{
					environment->sea[i][j] = 1;
				}
				else if (environment->sea[i][j] != 1)
				{
					environment->sea[i][j] = 0;
				}
			}
		}
	}

	environment->number_of_active_users = 0;
}

void startListening(struct Environment * environment)
{
	int listenfd = 0, connfd = 0, i;
	struct sockaddr_in serv_addr; 

	char sendBuff[1025];

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, '0', sizeof(serv_addr));
	memset(sendBuff, '0', sizeof(sendBuff)); 

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(PORT_NUMBER); 

	bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

	listen(listenfd, 10); 

	while(1)
	{
		if (environment->number_of_active_users < NUMBER_OF__USERS)
		{
			connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);			

			environment->users[environment->number_of_active_users] = connfd;
			environment->number_of_active_users++;

			snprintf(sendBuff, sizeof(sendBuff), "Hello new user!\r\n");
			write(connfd, sendBuff, strlen(sendBuff));

			printf("A new user just logged in with the ID: %d\r\n", connfd);

			if (environment->number_of_active_users > NUMBER_OF__USERS) {
				printf("ERROR: Too many users were logged in!\r\n");
			}
			else if (environment->number_of_active_users == NUMBER_OF__USERS)
			{
				printf("All the users are logged in. Hooray!\r\n");
			}
		}
		else
		{
			for (i = 0 ; i < environment->number_of_active_users ; i++)
			{
				snprintf(sendBuff, sizeof(sendBuff), "Bla bla bla...\r\n");
				write(environment->users[i], sendBuff, strlen(sendBuff));
			}
		}		

		// close(connfd);
		sleep(1);
	}
}

void showSea(struct Environment * environment)
{
	int i, j;

	for (i = 0 ; i < SEA_SIZE ; i++)
	{
		for (j = 0 ; j < SEA_SIZE ; j++)
		{
			printf("%d", environment->sea[i][j]);
		}
		printf("\r\n");
	}
}
 
int main()
{
	struct Environment environment;

	init(& environment);
	startListening(& environment);
	

	showSea(& environment);

	return 0;
}