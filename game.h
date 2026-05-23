#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_ttf.h>

struct Vector2 //Simple Position Container
    {float x,y;};

const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 768;
const float WALL_THICKNESS = 15.0f;
const float PADDLE_W = 15.0f;
const float PADDLE_H = 100.0f;
const float BALL_SIZE = 15.0f;
const float PADDLE_SPEED = 400.0f;
const float BALL_SPEED_INIT = 300.0f;
const float BALL_SPEED_MAX = 800.0f;
const float BALL_ACCEL = 40.0f;
const int WIN_SCORE = 3;

class Game
{
public:
    Game();
    bool Initialize(); // Initializes the game
    void RunLoop(); // Runs Game Loop
    void Shutdown(); // Ends Game

private:

    //The three loop phases
    void ProcessInput();
    void UpdateGame();
    void GenerateOutput();

    //Drawing helpers
    void DrawRect(float x, float y, float w, float h);
    void DrawScore();
    void DrawWinScreen();
    void DrawCenterLine();

    //Resets Ball to center after a point
    void ResetBall();

    //CollisionCheck
    bool Paddle1Hit = false;
    bool Paddle2Hit = false;

    SDL_Window* Window = nullptr;
    SDL_Renderer* Renderer = nullptr;
    bool mIsRunning = true;
    Uint64 mTicks = 0;

    //Paddles - position is their center point
    Vector2  Paddle1Pos; //Player 1, left (W/S)
    Vector2  Paddle2Pos; //Player 2, right (Up/Down)

    float Paddle1Dir = 0.0f;
    float Paddle2Dir = 0.0f;

    //Ball
    Vector2 BallPos;
    Vector2 BallVel;
    float BallSpeed = BALL_SPEED_INIT;

    //Scoring
    int P1Score = 0;
    int P2Score = 0;
    bool GameOver = false;
    int Winner = 0;
};
