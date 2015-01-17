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
#define NUMBER_OF_USERS 2
 
struct Environment
{
	int sea[SEA_SIZE][SEA_SIZE];
	int users[NUMBER_OF_USERS];
	int number_of_active_users;
	int user_who_has_the_turn;
};

void init(struct Environment * environment)
{
	environment->number_of_active_users = 0;
}

void generateSea(struct Environment * environment)
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

void startListening(struct Environment * environment)
{
	int listenfd = 0, connfd = 0, i;
	struct sockaddr_in serv_addr; 

	char sendBuff[1025];

	srand(time(NULL));

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
		if (environment->number_of_active_users < NUMBER_OF_USERS)
		{
			connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);			

			environment->users[environment->number_of_active_users] = connfd;
			environment->number_of_active_users++;

			snprintf(sendBuff, sizeof(sendBuff), "Hello new user!\r\n");
			write(connfd, sendBuff, strlen(sendBuff));

			printf("A new user just logged in with the ID: %d\r\n", connfd);

			if (environment->number_of_active_users > NUMBER_OF_USERS) {
				printf("ERROR: Too many users were logged in!\r\n");
			}
			else if (environment->number_of_active_users == NUMBER_OF_USERS)
			{
				printf("All the users are logged in. Hooray!\r\n\r\n");
				break;
			}
		}

		sleep(1);
	}

	generateSea(environment);

	printf("I just created a sea map and here is how it looks like:\r\n");
	showSea(environment);
	printf("\r\n");

	environment->user_who_has_the_turn = rand() % environment->number_of_active_users;
	printf("I also picked up the user who will have the first turn. It will be user no. %d which ahs the ID %d", environment->user_who_has_the_turn, environment->users[environment->user_who_has_the_turn]);
	printf("\r\n");

	while(1)
	{
		for (i = 0 ; i < environment->number_of_active_users ; i++)
		{
			if (i == environment->user_who_has_the_turn)
			{
				snprintf(sendBuff, sizeof(sendBuff), "It's your turn bro, do your thing...\r\n");
			}
			else
			{
				snprintf(sendBuff, sizeof(sendBuff), "Its user ID %d's turn. Waiting for him to make a move...\r\n", environment->users[environment->user_who_has_the_turn]);
			}
			write(environment->users[i], sendBuff, strlen(sendBuff));
		}	

		// close(connfd);
		sleep(1);
	}
}
 
int main()
{
	struct Environment environment;

	init(& environment);
	startListening(& environment);

	return 0;
}