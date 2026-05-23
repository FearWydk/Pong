#include "game.h"
#include <cmath>
#include <string>

static TTF_Font* gFont = nullptr; // Score Text
static TTF_Font* gFontLarge = nullptr; //WinScreen Text

Game::Game() {}

bool Game::Initialize()
{
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL could not initialize! SDL Error: %s", SDL_GetError());
        return false;
    }

    if (!TTF_Init()) {
        SDL_Log("TTF could not initialize! SDL Error: %s", SDL_GetError());
        return false;
    }

    Window = SDL_CreateWindow( "Pong", SCREEN_WIDTH, SCREEN_HEIGHT, 0);

    if (!Window) {
        SDL_Log("Window could not be created! SDL Error: %s", SDL_GetError());
        return false;
    }

    Renderer = SDL_CreateRenderer(Window, nullptr);

    if (!Renderer) {
        SDL_Log("Renderer could not be created! SDL Error: %s", SDL_GetError());
        return false;
    }

    //Load a system font
    const char* fontPath[] = {
        "C:/Windows/Fonts/arial.ttf",
        nullptr
    };
    for (int i = 0; fontPath[i] != nullptr; i++) {
        gFont = TTF_OpenFont(fontPath[i], 36);
        gFontLarge = TTF_OpenFont(fontPath[i], 64);
        if (gFont && gFontLarge) break;
    }

    Paddle1Pos = { PADDLE_W / 2.0f + 10.0f, SCREEN_HEIGHT / 2.0f };
    Paddle2Pos = { SCREEN_WIDTH - PADDLE_W / 2.0f - 10.0f, SCREEN_HEIGHT / 2.0f };
    ResetBall();

    mTicks = SDL_GetTicks();
    return true;
}


void Game::ProcessInput()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_EVENT_QUIT:
                mIsRunning = false;
                break;
            case SDL_EVENT_KEY_DOWN:
                if (GameOver)
                {
                    if (event.key.key == SDLK_ESCAPE)
                    mIsRunning = false;
                    else
                    {
                        P1Score = P2Score = 0;
                        GameOver = false;
                        ResetBall();
                    }
                }
                break;
        }
    }
    const bool* keyState = SDL_GetKeyboardState(nullptr);

    if (keyState[SDL_SCANCODE_ESCAPE]) mIsRunning = false;

    Paddle1Dir = 0.0f;
    if (keyState[SDL_SCANCODE_W]) Paddle1Dir -= 1.0f;
    if (keyState[SDL_SCANCODE_S]) Paddle1Dir += 1.0f;

    Paddle2Dir = 0.0f;
    if (keyState[SDL_SCANCODE_UP]) Paddle2Dir -= 1.0f;
    if (keyState[SDL_SCANCODE_DOWN]) Paddle2Dir += 1.0f;

}

void Game::GenerateOutput() {
    //Clear to black
    SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
    SDL_RenderClear(Renderer);

    SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);

    //Top Wall
    DrawRect(SCREEN_WIDTH/2.0f, WALL_THICKNESS/2.0f, SCREEN_WIDTH, WALL_THICKNESS);
    //Bottom Wall
    DrawRect(SCREEN_WIDTH/2.0f, SCREEN_HEIGHT - WALL_THICKNESS/2.0f, SCREEN_WIDTH, WALL_THICKNESS);
    //Paddles
    if (Paddle1Hit)
        SDL_SetRenderDrawColor(Renderer, 0, 255, 255, 255);
    else
        SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
    DrawRect(Paddle1Pos.x, Paddle1Pos.y, PADDLE_W,PADDLE_H);

    if (Paddle2Hit)
        SDL_SetRenderDrawColor(Renderer, 0, 255, 255, 255);
    else
        SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
    DrawRect(Paddle2Pos.x, Paddle2Pos.y, PADDLE_W,PADDLE_H);

    //Ball
    SDL_SetRenderDrawColor(Renderer, 255,80,80, 255);
    DrawRect(BallPos.x, BallPos.y, BALL_SIZE, BALL_SIZE);
    //Center Line
    DrawCenterLine();
    //Score
    DrawScore();

    if (GameOver) DrawWinScreen();

    SDL_RenderPresent(Renderer);
}

void Game::ResetBall() { BallPos = {SCREEN_WIDTH/2, SCREEN_HEIGHT/2};
    BallSpeed = BALL_SPEED_INIT;
    Paddle1Hit = false;
    Paddle2Hit = false;
    //Makes beginning angle predictable ball will aim for either players paddle
    float dirX = (rand() %2 ==0) ? 1.0f : -1.0f;
    float dirY = (rand() %2 ==0) ? 1.0f : -0.5f;
    float len = std::sqrt(dirX * dirX + dirY * dirY);
    BallVel = {dirX / len * BallSpeed, dirY / len * BallSpeed};
}

void Game::RunLoop()
{
    while (mIsRunning) {
        ProcessInput();
        UpdateGame();
        GenerateOutput();
    }
}


void Game::Shutdown()
{
    TTF_CloseFont(gFont);
    TTF_CloseFont(gFontLarge);
    TTF_Quit();
    SDL_DestroyRenderer(Renderer);
    SDL_DestroyWindow(Window);
    SDL_Quit();
}

void Game::UpdateGame()
{
    //1. Delta Time (Seconds since last frame)
    Uint64 now = SDL_GetTicks();
    while (now < mTicks + 16)
        now = SDL_GetTicks();
    float deltaTime = (now - mTicks) / 1000.0f;
    if (deltaTime > 0.05f) deltaTime = 0.05f; //Clamp max deltaTime value
    mTicks = now;

    if (GameOver) return;

    //2. Move paddles, clamped to wall boundaries
    const float halfPaddle = PADDLE_H / 2.0f;
    const float topLimit = WALL_THICKNESS + halfPaddle;
    const float bottomLimit = SCREEN_HEIGHT - WALL_THICKNESS - halfPaddle;

    Paddle1Pos.y += Paddle1Dir * PADDLE_SPEED * deltaTime;
    Paddle1Pos.y = std::fmax(topLimit, std::fmin(Paddle1Pos.y, bottomLimit));

    Paddle2Pos.y += Paddle2Dir * PADDLE_SPEED * deltaTime;
    Paddle2Pos.y = std::fmax(topLimit, std::fmin(Paddle2Pos.y, bottomLimit));

    //3. Move ball
    BallPos.x += BallVel.x * deltaTime;
    BallPos.y += BallVel.y * deltaTime;

    const float halfBall = BALL_SIZE / 2.0f;

    // 4. Top and Bottom Wall Bounces
    if (BallPos.y - halfBall <= WALL_THICKNESS && BallVel.y < 0.0f)
        BallVel.y *= -1.0f;
    if (BallPos.y + halfBall >= SCREEN_HEIGHT - WALL_THICKNESS && BallVel.y > 0.0f)
        BallVel.y *= -1.0f;

    // 5. Paddle Collision Detection
    auto hitsPaddle = [&](const Vector2& paddle) -> bool
    {
        float dy = std::abs(BallPos.y - paddle.y);
        float dx = std::abs(BallPos.x - paddle.x);
        return dy <= (PADDLE_H/2.0f+halfBall) && dx <= (PADDLE_W/2.0f+halfBall);
    };

    // 6. Left paddle hit
    if (hitsPaddle(Paddle1Pos) && BallVel.x < 0.0f)
    {
        Paddle1Hit = true;//Toggle hit state for visual feedback
        Paddle2Hit = false;
     BallSpeed = std::fmin(BallSpeed + BALL_ACCEL, BALL_SPEED_MAX);
        float len = std::sqrt(BallVel.x * BallVel.x + BallVel.y * BallVel.y);
        BallVel.x = -(BallVel.x/len) * BallSpeed;
        BallVel.y = (BallVel.y/len) * BallSpeed;
        BallVel.y += (BallPos.y - Paddle1Pos.y) * 2.5f;
    }

    // 7. Right paddle hit
    if (hitsPaddle(Paddle2Pos) && BallVel.x > 0.0f)
    {
        Paddle2Hit = true; //Toggle hit state for visual feedback
        Paddle1Hit = false;

        BallSpeed = std::fmin(BallSpeed + BALL_ACCEL, BALL_SPEED_MAX);
        float len = std::sqrt(BallVel.x * BallVel.x + BallVel.y * BallVel.y);
        BallVel.x = -(BallVel.x/len) * BallSpeed;
        BallVel.y = (BallVel.y/len) * BallSpeed;
        BallVel.y += (BallPos.y - Paddle2Pos.y) * 2.5f;
    }


    // 8. Scoring
    if (BallPos.x < 0.0f)
    {
     ++P2Score;
        if (P2Score >= WIN_SCORE)
        {
            GameOver = true;
            Winner = 2;
        }
        else ResetBall();
    }
    else if (BallPos.x > SCREEN_WIDTH)
    {
        ++P1Score;
        if (P1Score >= WIN_SCORE)
        {
            GameOver = true;
            Winner = 1;
        }
        else ResetBall();

    }
}

void Game::DrawCenterLine() {
    SDL_SetRenderDrawColor(Renderer, 80, 80, 80, 255);
    int segments = 20;
    float segH = (SCREEN_HEIGHT - 2.0f * WALL_THICKNESS) / (segments * 2.0f - 1.0f);
    for (int i = 0; i < segments; i++)
    {
        SDL_FRect r
          {
              SCREEN_WIDTH / 2.0f - 2.0f,
                 WALL_THICKNESS + i * 2.0f * segH,
                 4.0f,
                 segH
             };
        SDL_RenderFillRect(Renderer, &r);
    }
}
void Game::DrawRect(float x, float y, float w, float h) {
    SDL_FRect r{x - w/2.0f, y - h/2.0f, w, h};
    SDL_RenderFillRect(Renderer, &r);
}

void Game::DrawScore()
{
    SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);


    if (gFont)
    {
        auto renderText = [&](const std::string& text, float x, float y)
        {
            SDL_Color white = {255, 255, 255, 255};
            SDL_Surface* surf = TTF_RenderText_Solid(gFont, text.c_str() ,0, white);
            SDL_Texture* tex = SDL_CreateTextureFromSurface(Renderer, surf);
            SDL_DestroySurface(surf);

            float w, h;
            SDL_GetTextureSize(tex, &w, &h);
            SDL_FRect dst {x - w/2, y - h/2, w, h};
            SDL_RenderTexture(Renderer, tex, nullptr, &dst);
            SDL_DestroyTexture(tex);
        };


        renderText(std::to_string(P1Score), SCREEN_WIDTH/2 - 60, 40);
        renderText(std::to_string(P2Score), SCREEN_WIDTH/2 + 60, 40);
    }
}

void Game::DrawWinScreen() {

    //Semi-Transparent Overlay
    SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(Renderer, 0,0,0, 160);
     SDL_FRect overlay{0.0f, 0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT};
    SDL_RenderFillRect(Renderer, &overlay);
    SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_NONE);

    if (!gFontLarge || !gFont) return;

    auto renderText = [&](TTF_Font* font, const std::string& text, int x, int y, SDL_Color color)
    {
        SDL_Surface* surf = TTF_RenderText_Solid(gFont, text.c_str(),0, color);
        SDL_Texture* tex = SDL_CreateTextureFromSurface(Renderer, surf);
        SDL_DestroySurface(surf);

        float w, h;
        SDL_GetTextureSize(tex, &w, &h);

        SDL_FRect dstRect {x - w/2, y - h/2, w, h};
        SDL_RenderTexture(Renderer, tex, nullptr, &dstRect);
        SDL_DestroyTexture(tex);
    };

    SDL_Color gold {255,215,0,255};
    SDL_Color white{255,255,255,255};
    SDL_Color red{200,0,0,255};

    std::string msg = "PLAYER " + std::to_string(Winner) + " WINS!";
    renderText(gFontLarge, msg, SCREEN_WIDTH/2, SCREEN_HEIGHT/2 - 50, gold);
    renderText(gFont, "Press any key to play again", SCREEN_WIDTH/2, SCREEN_HEIGHT/2 - 10, white);
    renderText(gFont, "Press ESC to quit", SCREEN_WIDTH/2, SCREEN_HEIGHT/2 + 40, red);
}

