#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
 
struct Environment
{
   int sea[6][6];
};

void init(struct Environment * environment)
{
	srand(time(NULL));
	for (int i = 0 ; i < 6 ; i++)
	{
		for (int j = 0 ; j < 6 ; j++)
		{
			if (rand() % 2 && i < 5 && j < 5)
			{

				if (
					environment->sea[i][j] != 0 && environment->sea[i][j] != 1 && 
					environment->sea[i+1][j] != 0 && environment->sea[i+1][j] != 1 && 
					environment->sea[i][j+1] != 0 && environment->sea[i][j+1] != 1 &&
					environment->sea[i+1][j+1] != 0 && environment->sea[i+1][j+1] != 1 && 
					((i == 0 && j == 0) || environment->sea[i-1][j-1] != 1) &&
					((j == 0) || environment->sea[i][j-1] != 1) &&
					((i == 5 && j == 0) || environment->sea[i+1][j-1] != 1) &&
					((i == 4 && j == 0) || environment->sea[i+2][j-1] != 1) &&
					((i == 4) || environment->sea[i+2][j] != 1) &&
					((i == 4 && j == 5) || environment->sea[i+2][j+1] != 1) &&
					((i == 4 && j == 4) || environment->sea[i+2][j+2] != 1) &&
					((i == 5 && j == 4) || environment->sea[i+1][j+2] != 1) &&
					((j == 4) || environment->sea[i][j+2] != 1) &&
					((i == 0 && j == 4) || environment->sea[i-1][j+2] != 1) &&
					((i == 0 && j == 5) || environment->sea[i-1][j+1] != 1) &&
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
					((i == 5 && j == 0) || environment->sea[i+1][j-1] != 1) &&
					((i == 5) || environment->sea[i+1][j] != 1) &&
					((i == 5 && j == 5) || environment->sea[i+1][j+1] != 1) &&
					((j == 5) || environment->sea[i][j+1] != 1) &&
					((i == 0 && j == 5) || environment->sea[i-1][j+1] != 1) &&
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
	for (int i = 0 ; i < 6 ; i++)
	{
		for (int j = 0 ; j < 6 ; j++)
		{
			printf("%d", environment->sea[i][j]);
		}
		printf("\n");
	}
}
 
int main()
{
   struct Environment environment;

   init(& environment);
   showSea(& environment);

   return 0;
}