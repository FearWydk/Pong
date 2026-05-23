Pong — C++ / SDL3
A fully functional Pong clone built from scratch in C++ using SDL3.
Developed as a foundational C++ game programming exercise based on
Game Programming in C++ by Sanjay Madhav.

Overview
This project demonstrates core game programming fundamentals implemented
without a game engine — raw C++, SDL3, and a hand-rolled game loop.
Every system from input handling to collision response was written manually.

Features

Game loop architecture — fixed delta time, input, update, and render phases
modeled after industry-standard patterns
Two player local multiplayer — independent input handling for both players
Ball acceleration — velocity increases on every paddle contact, capped at
a maximum speed to keep gameplay balanced
Angle influence — ball deflection angle is influenced by where it contacts
the paddle, rewarding precise play
Collision response — velocity vector renormalization preserves direction
while applying new speed after each hit
Score tracking — first to 7 points wins
Paddle color feedback — paddles switch color on ball contact giving
clear visual feedback on each hit
Win screen — displays winner with replay and quit prompts
SDL3_ttf text rendering — score and UI rendered with a TTF font


Controls
ActionPlayer 1Player 2Move UpW↑Move DownS↓QuitESCReplay (win screen)Any key

Technical Details
TopicImplementationLanguageC++17RenderingSDL3 hardware-accelerated rendererTextSDL3_ttfBuild systemCMake + NinjaIDECLionDelta timeUint64 tick-based, capped at 50msCollisionAABB overlap check with position correctionBall physicsVector normalization with per-frame integration

Architecture
main.cpp        — entry point, creates and runs Game
Game.h          — class declaration, constants, Vector2 struct
Game.cpp        — full game loop implementation
CMakeLists.txt  — SDL3 and SDL3_ttf linkage
The Game class follows Madhav's three-phase loop pattern:
Initialize
↓
┌─ ProcessInput  ←──────────────┐
│   UpdateGame                  │  ← RunLoop
└─ GenerateOutput ──────────────┘
↓
Shutdown

How to Build
Prerequisites

SDL3 — grab the VC .zip
SDL3_ttf — grab the VC .zip
CMake 3.16+
CLion or any CMake-compatible IDE

Steps
bashgit clone https://github.com/YOUR_USERNAME/Pong.git
cd Pong
Open in CLion and add the following to your CMake options under
Settings → Build, Execution, Deployment → CMake:
-DSDL3_DIR=path/to/SDL/cmake -DSDL3_ttf_DIR=path/to/SDL_ttf/cmake
Then hit Build and Run.

SDL3 vs SDL2 — Key Differences Applied
This project was intentionally built on SDL3 rather than SDL2 to work
with the current SDL API. Notable differences encountered and applied:
SDL2SDL3SDL_RectSDL_FRect (float-based)SDL_RenderCopy()SDL_RenderTexture()SDL_FreeSurface()SDL_DestroySurface()SDL_QueryTexture()SDL_GetTextureSize()SDL_KEYDOWNSDL_EVENT_KEY_DOWNevent.key.keysym.symevent.key.keyconst Uint8* keyStateconst bool* keyStateSDL_TICKS_PASSED macroManual Uint64 comparisonSDL_CreateRenderer(w, -1, flags)SDL_CreateRenderer(w, nullptr)

Part of a Larger Portfolio
This project is one of several C++ and Unreal Engine 5 projects in active
development. See the full portfolio at:
fearwydk.github.io/portfolio

Based on Chapter 1 of Game Programming in C++ by Sanjay Madhav (Addison-Wesley, 2019)