#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <windows.h>

//Arrow key scan codes
#define UP_KEY 72
#define LEFT_KEY 75
#define DOWN_KEY 80
#define RIGHT_KEY 77

//Snake movement direction
#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

//Reason for gameover
#define HITSELF 1
#define HITWALL 2

#define POINT(x,y) ((y)*80+(x)+1)
#define GETX(x) (((x)-1)%80)
#define GETY(x) (((x)-1)/80)

HANDLE Console;

int GameLoop();

void SetPos(int x, int y)
{
	COORD coors = {x, y};
	
	SetConsoleCursorPosition(Console, coors);
}

void PrintCenteredText(char * text)
{
	int i, len = strlen(text);
	SetPos(40 - (len/2) - 2, 11);
	printf("%c", 201);
	for(i = 0; i < len + 2; i++)
		printf("%c", 205);
	printf("%c", 187);
	SetPos(40 - (len/2) - 2, 12);
	printf("%c ", 186);
	printf(text);
	printf(" %c", 186);
	SetPos(40 - (len/2) - 2, 13);
	printf("%c", 200);
	for(i = 0; i < len + 2; i++)
		printf("%c", 205);
	printf("%c", 188);
}

int main(int argc, char * argv[])
{
	COORD BufCoors = {80, 25};
	CONSOLE_CURSOR_INFO CurrInfo = {sizeof(CONSOLE_CURSOR_INFO), 0};
	
	Console = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleScreenBufferSize(Console, BufCoors);
	SetConsoleCursorInfo(Console, &CurrInfo);
	SetConsoleTitle("Snake");

	PrintCenteredText("Press [enter] to play");
	
	while(GameLoop());
		;
	
	return 0;
}

//==============================================//

char play = 0;
char pause = 0;
char direction;

int snake[500]; //Snake piece locations. [0] is head.
int pieces;

int newpieces;
int pointtoadd;
int pointtoerase;
int food = 0;

void FreshDraw();
void DrawFood();

void InitSnake()
{
	int x, y;
	
	for(x = 39, y = 0; y < 2; y++)
		snake[y] = POINT(x,7-y);	
		
	pieces = 2;
	newpieces = 6;
	pointtoadd = pointtoerase = 0;
	direction = DOWN;
}

void GameOver(int Type)
{
	switch(Type)
	{
		case HITSELF:
			PrintCenteredText("Game Over! You hit yourself!");
			break;
		
		case HITWALL:
			PrintCenteredText("Game Over! You hit a wall!");
	}
	
	food = 0;
	play = 0;
	memset(snake, 0, sizeof(int) * 500);
}

int UpdateSnake()
{
	int x, y;

	memcpy(snake + 1, snake, sizeof(int)*pieces);
	if(!newpieces)
	{
		pointtoerase = snake[pieces];
		snake[pieces] = 0;
	}
		
	else
	{
		pieces++;
		newpieces--;
	}
	
	x = GETX(snake[0]);
	y = GETY(snake[0]);
	
	switch(direction)
	{
		case UP:
			y -= 1;
			snake[0] = POINT(x,y);
			break;
			
		case DOWN:
			y += 1;
			snake[0] = POINT(x,y);
			break;
			
		case LEFT:
			x -= 1;
			snake[0] = POINT(x,y);
			break;
		
		case RIGHT:
			x += 1;
			snake[0] = POINT(x,y);
			break;
	}
	
	pointtoadd = snake[0];
	
	if(x > 79 || x < 0 ||
	   y > 24 || y < 0 )
	{
		GameOver(HITWALL);
		return 1;
	}
	
	int i;
	for(i = 1; i < pieces; i++)
		if(snake[0] == snake[i])
		{
			GameOver(HITSELF);
			return 1;
		}
		
	if(snake[0] == food)
	{
		while(1) //Find a location for the food not within the snake
		{
			food = (rand() % (80 * 25)) + 1;
			for(i = 0; snake[i]; i++)
				if(food == snake[i])
					food = 0;
					
			if(food)
				break;
		}
		DrawFood();
		newpieces = (rand() % 6) + 1;
	}
	
	return 0;
}

void DrawFood()
{
	SetPos(GETX(food),GETY(food));
	printf("O");
}

void FreshDraw()
{
	system("cls");
	int i;
	for(i = 0; snake[i]; i++)
	{
		SetPos(GETX(snake[i]),GETY(snake[i]));
		printf("#");
	}
	
	DrawFood();
}

void UpdateScreen()
{		
	if(pointtoerase)
	{
		SetPos(GETX(pointtoerase), GETY(pointtoerase));
		printf(" ");
	}
	
	if(pointtoadd)
	{
		SetPos(GETX(pointtoadd), GETY(pointtoadd));
		printf("#");
	}
	
	pointtoadd = pointtoerase = 0;
}

int GameLoop()
{
	int KeyPress;
	
	Sleep(50);
	
	if(kbhit())
	{
		KeyPress = getch();
		
		if(KeyPress == 0 || KeyPress == 0xE0) // 00 or E0 precedes scan code read
		{
			KeyPress = getch();
			
			switch(KeyPress)
			{
				case UP_KEY:
					if(direction != DOWN) //Can't reverse
						direction = UP;
					break;
				
				case DOWN_KEY:
					if(direction != UP)
						direction = DOWN;
					break;
					
				case LEFT_KEY:
					if(direction != RIGHT)
						direction = LEFT;
					break;
					
				case RIGHT_KEY:
					if(direction != LEFT)
						direction = RIGHT;
					break;
			}
		}
		
		if(KeyPress == '\r') //enter
		{
			if(!play)
			{
				play = 1;
				if(pause)
				{
					FreshDraw();
					pause = 0;
				}
			}
				
			else
			{
				PrintCenteredText("Game paused");
				play = 0;
				pause = 1;
			}
		}
		
		else if(KeyPress == 0x1B) //escape
			return 0;
	}
	
	if(play)
	{
		if(!food) //New game starting
		{
			srand(clock());
			food = (rand() % (80 * 25)) + 1;
			
			InitSnake();
			FreshDraw(); //initial draw
		}
		
		else
		{
			if(!UpdateSnake())
				UpdateScreen(); //Only draw if the game should continue
		}
	}
	
	return 1;
}
