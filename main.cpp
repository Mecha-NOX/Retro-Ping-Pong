#include <iostream>
#include <unistd.h>
#include <raylib.h>

using namespace std;

Color Green = Color{38, 185, 154, 255};
Color Dark_Green = Color{20, 160, 133, 255};
Color Light_Green = Color{129, 204, 184, 255};
Color Yellow = Color{243, 213, 91, 255};

int player_score = 0;
int cpu_score = 0;

class Ball
{
public:
    float x, y;
    int speed_x, speed_y;
    int radius;

    void Draw()
    {
        DrawCircle(x, y, 20, Yellow);
    }

    int Update(Sound bounds, Sound topdown)
    {
        x += speed_x;
        y += speed_y;

        if (y + radius >= GetScreenHeight() || y - radius <= 0)
        {
            speed_y *= -1;
            PlaySound(bounds);
        }
        if (x + radius >= GetScreenWidth()) // Cpu Wins
        {
            cpu_score++;
            PlaySound(topdown);
            ResetBall();
            if (cpu_score == 5)
            {
                return 0;
            }
        }
        if (x - radius <= 0) // Player wins
        {
            player_score++;
            PlaySound(topdown);
            ResetBall();
        }
        return 0;
    }

    void ResetBall()
    {
        x = GetScreenWidth() / 2;
        y = GetScreenHeight() / 2;

        int speed_choices[2] = {-1, 1};
        speed_x *= speed_choices[GetRandomValue(0, 1)];
        speed_y *= speed_choices[GetRandomValue(0, 1)];
    }
};

class Paddle
{
protected:
    void LimitMovement()
    {
        if (y <= 0)
        {
            y = 0;
        }
        if (y + height >= GetScreenHeight())
        {
            y = GetScreenHeight() - height;
        }
    }

public:
    float x, y;
    float width, height;
    int speed;

    void Draw()
    {
        DrawRectangleRounded(Rectangle{x, y, width, height}, 0.8, 0, WHITE);
    }

    void Update()
    {
        if (IsKeyDown(KEY_W))
        {
            y = y - speed;
        }

        if (IsKeyDown(KEY_S))
        {
            y = y + speed;
        }

        LimitMovement();
    }
};

class CpuPaddle : public Paddle
{
public:
    void Update(int ball_y)
    {
        if (y + height / 2 > ball_y)
        {
            y = y - speed;
        }

        if (y + height / 2 <= ball_y)
        {
            y = y + speed;
        }

        LimitMovement();
    }
};

bool IsMouseOverRectangle(Vector2 mousePosition, Rectangle rec)
{
    return (mousePosition.x >= rec.x && mousePosition.x <= (rec.x + rec.width) &&
            mousePosition.y >= rec.y && mousePosition.y <= (rec.y + rec.height));
}

bool MainMenu(int screenwidth, int screenheight)
{
    Rectangle play_rec = {static_cast<float>(screenwidth / 2 - 100), static_cast<float>(screenheight / 2 - 50), 200, 100};
    Rectangle exit_rec = {static_cast<float>(screenwidth / 2 - 100), static_cast<float>(screenheight / 2 + 50), 200, 100};

    ClearBackground(Dark_Green); // Clear the background before drawing

    if (IsMouseOverRectangle(GetMousePosition(), play_rec))
    {
        DrawRectangleRec(play_rec, Light_Green);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            return false; // Exit main menu and start the game
        }
    }
    else
    {
        DrawRectangleRec(play_rec, Green);
    }

    if (IsMouseOverRectangle(GetMousePosition(), exit_rec))
    {
        DrawRectangleRec(exit_rec, Light_Green);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            return false; // Exit the game
        }
    }
    else
    {
        DrawRectangleRec(exit_rec, Green);
    }

    DrawText("Play", play_rec.x + play_rec.width / 4, play_rec.y + play_rec.height / 3, 40, DARKGRAY);
    DrawText("Exit", exit_rec.x + exit_rec.width / 4, exit_rec.y + exit_rec.height / 3, 40, DARKGRAY);

    return true; // Stay in main menu
}

Ball ball;
Paddle player;
CpuPaddle cpu;

int main()
{

    const int screen_width = 1200;
    const int screen_height = 800;
    InitWindow(screen_width, screen_height, "My Pong Game!");
    InitAudioDevice();
    bool inMainMenu = true;

    // Import Audio
    Sound paddle = LoadSound("sound_assets/paddle_collision_ sound.mp3");   // Paddle-Ball collision sound
    Sound bounds = LoadSound("sound_assets/out_of_bounds.mp3");             // OutofBounds collision sound
    Sound topdown = LoadSound("sound_assets/topdown_collision_ sound.mp3"); // Boundary-Ball collision sound

    SetTargetFPS(60);

    ball.radius = 20;
    ball.x = screen_width / 2;
    ball.y = screen_height / 2;
    ball.speed_x = 7;
    ball.speed_y = 7;

    player.width = 25;
    player.height = 120;
    player.x = screen_width - player.width - 10;
    player.y = screen_height / 2 - player.height / 2;
    player.speed = 6;

    cpu.height = 120;
    cpu.width = 25;
    cpu.x = 10;
    cpu.y = screen_height / 2 - cpu.height / 2;
    cpu.speed = 6;

    while (!WindowShouldClose())
    {
        if (inMainMenu)
        {
            BeginDrawing();
            ClearBackground(Dark_Green);
            inMainMenu = MainMenu(screen_width, screen_height);
            EndDrawing();
        }
        else
        {
            BeginDrawing();

            // Updating
            ball.Update(bounds, topdown);
            player.Update();
            cpu.Update(ball.y);

            // Checking for collisions
            if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, Rectangle{player.x, player.y, player.width, player.height}))
            {
                ball.speed_x *= -1;
                PlaySound(paddle); // Paddle-ball collision sound
            }
            if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, Rectangle{cpu.x, cpu.y, cpu.width, cpu.height}))
            {
                ball.speed_x *= -1;
                PlaySound(paddle); // Paddle-ball collision sound
            }

            // Drawing
            ClearBackground(Dark_Green);
            DrawRectangle(screen_width / 2, 0, screen_width / 2, screen_height, Green);
            DrawCircle(screen_width / 2, screen_height / 2, 150, Light_Green);
            DrawLine(screen_width / 2, 0, screen_width / 2, screen_height, WHITE);
            cpu.Draw();
            ball.Draw();
            player.Draw();
            DrawText(TextFormat("%i", cpu_score), screen_width / 4 - 20, 20, 80, WHITE);
            DrawText(TextFormat("%i", player_score), 3 * screen_width / 4 - 20, 20, 80, WHITE);

            if (cpu_score == 5)
            {
                DrawText("GAME OVER", screen_width / 4, screen_height / 3, 100, RED);
                EndDrawing();
                sleep(2);
                break;
            }

            if (player_score == 5)
            {
                DrawText("YOU WIN!", screen_width / 4, screen_height / 3, 100, RED);
                EndDrawing();
                sleep(2);
                break;
            }

            EndDrawing();
        }
    }

    CloseAudioDevice();
    CloseWindow();
}