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

int main(int argc, char *argv[])
{
	int sockfd = 0, n = 0;
	char recvBuff[1024], sendBuff[1024];
	int input[2];
	struct sockaddr_in serv_addr; 

	memset(recvBuff, '0', sizeof(recvBuff));
	memset(sendBuff, '0', sizeof(sendBuff)); 

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
	if(fputs(replace_str(replace_str(recvBuff, "'YOUR_TURN'", ""), "'NOT_YOUR_TURN'", ""), stdout) == EOF)
	{
		printf("\n Error : Fputs error\n");
	}

	if (strstr(recvBuff, "'YOUR_TURN'") != NULL)
	{
		printf("Yeah, it's my turn!\r\n");

		printf("Enter latitude: ");
		scanf("%d", & input[0]);

		printf("Enter longitude: ");
		scanf("%d", & input[1]);

		snprintf(sendBuff, sizeof(sendBuff), "%d,%d\r\n", input[0], input[1]);
		write(sockfd, sendBuff, strlen(sendBuff));
	}
	else if (strstr(recvBuff, "'NOT_YOUR_TURN'") != NULL)
	{
		printf("Bummer, it's not my turn...\r\n");
	}

	sleep(1);
	
    } 

    if(n < 0)
    {
        printf("\n Read error \n");
    } 

    return 0;
}