#include "raylib.h"
#include "deque"
#include "raymath.h"
using namespace std;

Color SkyBlue{ 102, 191, 255, 255 };
Color Brown{ 127, 106, 79, 255 };
Color DarkGreen{ 0, 117, 44, 255 };

int CellSize = 30;
int CellCount = 25;
int offset = 75;
double LastUpdateTime = 0;
const char* Text = nullptr;
bool EventTriggered(double Interval)
{
	double CurrentTime = GetTime();
	if (CurrentTime - LastUpdateTime >= Interval)
	{
		LastUpdateTime = CurrentTime;
		return true;
	}
	return false;
}
bool ElementInDeque(Vector2 Element, deque<Vector2> deque)
{
	for (unsigned int i = 0; i < deque.size(); i++)
	{
		if (Vector2Equals(deque[i], Element))
		{
			return true;
		}
	}
	return false;
}

class Snake {
public:
	deque<Vector2>body = { Vector2{6,9}, Vector2{5,9}, Vector2{4,9} };
	Vector2 Direction = { 1,0 };
	bool addSegment = false;
	void Draw()
	{
		for (unsigned int i = 0; i < body.size(); i++)
		{
			float x = body[i].x;
			float y = body[i].y;
			Rectangle segment = Rectangle{offset + x * CellSize, offset + y * CellSize, (float)CellSize, (float)CellSize };
			DrawRectangleRounded(segment, 0.5, 6, Brown);
		}
	}
	void Update()
	{
		body.push_front(Vector2Add(body[0], Direction));
		if (addSegment == true)
		{
			addSegment = false;
		}
		else
		{
			body.pop_back();
		}
	}
	void Reset()
	{
			body = { Vector2{6,9}, Vector2{5,9}, Vector2{4,9} };
			Direction = { 1,0 };
	}

};

class Food {
public:
	Vector2 position;
	Texture2D texture;
	Food(deque<Vector2> snakeBody)
	{
		Image image = LoadImage("Photos/food.png");
		texture = LoadTextureFromImage(image);
		UnloadImage(image);
		position = GenerateRandomPos(snakeBody);
	}
	~Food()
	{
		UnloadTexture(texture);
	}
	void Draw()
	{
		DrawTexture(texture, offset + position.x * CellSize, offset + position.y * CellSize, DarkGreen);
	}
	Vector2 GenerateRandomCell()
	{
		float x = GetRandomValue(0, CellCount - 1);
		float y = GetRandomValue(0, CellCount - 1);
		return Vector2{ x,y };
	}
	Vector2 GenerateRandomPos(deque<Vector2>snakeBody)
	{
		Vector2 position = GenerateRandomCell();
		while (ElementInDeque(position, snakeBody))
		{
			position = GenerateRandomCell();
		}
		return position;
	}

};
class Game {
public:

	Snake snake = Snake();
	Food food = Food(snake.body);
	bool running = true;
	int score = 0;
	Sound EatSound;
	Sound WallSound;
	Game()
	{
		InitAudioDevice();
		EatSound = LoadSound("Sounds/eat.mp3");
		WallSound = LoadSound("Sounds/wall.mp3");
	}
	~Game()
	{
		UnloadSound(EatSound);
		UnloadSound(WallSound);
		CloseAudioDevice();
	}
	void Draw()
	{
		food.Draw();
		snake.Draw();
	}
	void Update()
	{
		if (running)
		{
			snake.Update();
			CheckCollisionWithFood();
			CheckCollisionWithEdges();
			CheckCollisionWithTail();
		}
	}
	void CheckCollisionWithFood()
	{
		if (Vector2Equals(snake.body[0], food.position))
		{
			food.position = food.GenerateRandomPos(snake.body);
			snake.addSegment = true;
			score++;
			PlaySound(EatSound);
		}
	}
	void CheckCollisionWithEdges()
	{
		if (snake.body[0].x == CellCount || snake.body[0].x == -1)
		{
			GameOver();
		}
		if (snake.body[0].y == CellCount || snake.body[0].y == -1)
		{
			GameOver();
		}

	}
	void GameOver()
	{
		Text = "Game Over!\nPress Space to Restart!\nPress Esc to Close the Game!";
		snake.Reset();
		food.position = food.GenerateRandomPos(snake.body);
		running = false;
		score = 0;
		PlaySound(WallSound);
	}
	void CheckCollisionWithTail()
	{
		deque<Vector2> headlessBody = snake.body;
		headlessBody.pop_front();
		if (ElementInDeque(snake.body[0], headlessBody))
		{
			GameOver();
		}
	}
};
int main()
{
	InitWindow(2*offset + CellSize * CellCount, 2*offset + CellSize * CellCount, "Snake");
	SetTargetFPS(60);
	Game game = Game();
	while (!WindowShouldClose())
	{
		BeginDrawing();

		ClearBackground(SkyBlue);
		DrawRectangleLinesEx(Rectangle{ (float)offset - 5, (float)offset - 5, (float)CellSize * CellCount + 10, (float)CellSize * CellCount + 10 }, 5, WHITE);
		DrawText("Snake: The Game", offset - 5, 20, 40, WHITE);
		DrawText(TextFormat("%i", game.score),offset-5, offset+CellSize*CellCount+10, 40, WHITE);
		game.Draw();
		if (EventTriggered(0.2))
		{
			game.Update();
		}
		if (IsKeyPressed(KEY_W) && game.snake.Direction.y != 1)
		{
			game.snake.Direction = { 0,-1 };
		}
		if (IsKeyPressed(KEY_S) && game.snake.Direction.y != -1)
		{
			game.snake.Direction = { 0,1 };
		}
		if (IsKeyPressed(KEY_A) && game.snake.Direction.x != 1)
		{
			game.snake.Direction = { -1,0 };
		}
		if (IsKeyPressed(KEY_D) && game.snake.Direction.x != -1)
		{
			game.snake.Direction = { 1,0 };
		}
		if (IsKeyPressed(KEY_UP) && game.snake.Direction.y != 1)
		{
			game.snake.Direction = { 0,-1 };
		}
		if (IsKeyPressed(KEY_DOWN) && game.snake.Direction.y != -1)
		{
			game.snake.Direction = { 0,1 };
		}
		if (IsKeyPressed(KEY_LEFT) && game.snake.Direction.x != 1)
		{
			game.snake.Direction = { -1,0 };
		}
		if (IsKeyPressed(KEY_RIGHT) && game.snake.Direction.x != -1)
		{
			game.snake.Direction = { 1,0 };
		}
		if (IsKeyPressed(KEY_SPACE))
		{
			game.running = true;
			Text = nullptr;
		}
		if (Text)
		{
			int TextWidth = MeasureText(Text, 40);
			DrawText(Text, GetScreenWidth() / 12, GetScreenHeight() / 2-30, 40, BLACK);
		}
		EndDrawing();

	}
	CloseWindow();
	return 0;
}