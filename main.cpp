#include <iostream>
#include <SDL3/SDL.h>

#include <chrono>
#include <thread>

struct Window
{
    SDL_Window *sdl_window;
    SDL_Renderer *sdl_renderer;
    SDL_Event event;
    bool active = false;
};

struct Color
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

struct Size
{
    float w;
    float h;
};

struct Pos
{
    float x;
    float y;
};

struct Rect
{
    Pos pos;
    Size size;
    Color color;
};

struct Input
{
    bool top = false;
    bool down = false;
    bool space = false;
};

static bool WIN_EVENT(Window *window)
{
    return SDL_PollEvent(&window->event);
}

/// WINDOWING ///
static Window WIN_INIT(const char *title, int width, int height, SDL_WindowFlags flags)
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *sdl_window = SDL_CreateWindow(title, width, height, flags);
    SDL_Renderer *sdl_renderer = SDL_CreateRenderer(sdl_window, nullptr);
    return Window{sdl_window, sdl_renderer, {}, true};
}

static void WIN_END(Window &window)
{
    SDL_DestroyRenderer(window.sdl_renderer);
    SDL_DestroyWindow(window.sdl_window);
    SDL_Quit();
}

static void UPDATE_INPUT(Window &window, Input &input)
{
    while (WIN_EVENT(&window))
    {
        if (window.event.type == SDL_EVENT_QUIT)
        {
            window.active = false;
        }

        if (window.event.type == SDL_EVENT_KEY_DOWN)
        {
            if (window.event.key.scancode == SDL_SCANCODE_W)
            {
                std::cout << "W PRESSED" << std::endl;
                input.top = true;
            }
            if (window.event.key.scancode == SDL_SCANCODE_S)
            {
                std::cout << "S PRESSED" << std::endl;
                input.down = true;
            }
            if (window.event.key.scancode == SDL_SCANCODE_SPACE)
            {
                std::cout << "S PRESSED" << std::endl;
                input.space = true;
            }
        }
        if (window.event.type == SDL_EVENT_KEY_UP)
        {
            if (window.event.key.scancode == SDL_SCANCODE_W)
            {
                std::cout << "W PRESSED" << std::endl;
                input.top = false;
            }
            if (window.event.key.scancode == SDL_SCANCODE_S)
            {
                std::cout << "S PRESSED" << std::endl;
                input.down = false;
            }
            if (window.event.key.scancode == SDL_SCANCODE_SPACE)
            {
                std::cout << "S PRESSED" << std::endl;
                input.space = false;
            }
        }
    }
}

static void CLEAR(Window &window)
{
    SDL_SetRenderDrawColor(window.sdl_renderer, 0, 0, 0, 255);
    SDL_RenderClear(window.sdl_renderer);
}

static void DRAW_RECT(Window &window, Rect &rect)
{
    SDL_FRect sdl_frect{rect.pos.x, rect.pos.y, rect.size.w, rect.size.h};
    SDL_SetRenderDrawColor(window.sdl_renderer, rect.color.r, rect.color.g, rect.color.b, rect.color.a);
    SDL_RenderFillRect(window.sdl_renderer, &sdl_frect);
}

static void PRESENT(Window &window)
{
    SDL_RenderPresent(window.sdl_renderer);
}

/// GAME LOGIC
static void MOVE_RECT(Input input, Rect &rect)
{
    if (input.top)
    {
        if (rect.pos.y <= 10.0f)
        {
            return;
        }
        rect.pos.y += -5.0f;
    }
    if (input.down)
    {
        if (rect.pos.y >= 290.0f)
        {
            return;
        }
        rect.pos.y += 5.0f;
    }
}
static void ENEMY_INPUT(Rect &rect, Rect &ball, Input &enemyInput)
{
    float center = rect.pos.y + rect.size.h / 2;
    if (center >= ball.pos.y + ball.size.h / 2)
    {
        enemyInput.down = false;
        enemyInput.top = true;
    };
    if (center <= ball.pos.y + ball.size.h / 2)
    {
        enemyInput.top = false;
        enemyInput.down = true;
    };
}

static void PHYSICS_BALL(Rect &ball, Rect &paddle, Rect &enemyPaddle, float &velocityX, float &velocityY)
{

    // if (ball.pos.x >= 800)
    // {
    //     velocityX = -velocityX;
    // };
    // if (ball.pos.x <= 0)
    // {
    //     velocityX = -velocityX;
    // };
    if (ball.pos.y + ball.size.h >= 400)
    {
        velocityY = -velocityY;
    };
    if (ball.pos.y <= 0)
    {
        velocityY = -velocityY;
    };

    if (ball.pos.x <= paddle.pos.x + paddle.size.w && ball.pos.x + ball.size.w >= paddle.pos.x && ball.pos.y <= paddle.pos.y + paddle.size.h && ball.pos.y + ball.size.h >= paddle.pos.y)
    {
        if (velocityX < 0)
        {
            ball.pos.x = paddle.pos.x + paddle.size.w;
            velocityX = -velocityX;
        };
    };
    if (ball.pos.x <= enemyPaddle.pos.x + enemyPaddle.size.w && ball.pos.x + ball.size.w >= enemyPaddle.pos.x && ball.pos.y <= enemyPaddle.pos.y + enemyPaddle.size.h && ball.pos.y + ball.size.h >= enemyPaddle.pos.y)
    {
        if (velocityX > 0)
        {
            ball.pos.x = enemyPaddle.pos.x - ball.size.w;
            velocityX = -velocityX;
        }
    };
    ball.pos.x += velocityX;
    ball.pos.y += velocityY;
}

enum GameState
{
    GAMEPLAY,
    WIN_CONDITION,
    LOSS_CONDITION,
};

struct GameData
{
    Rect paddle{50.0f, 150.0f, 20.0f, 100.0f, 255, 255, 255, 255};
    Rect enemyPaddle{700.0f, 150.0f, 20.0f, 100.0f, 255, 255, 255, 255};
    Input input;
    Input enemyInput;
    Rect ballPlane{400.0f, 200.0f, 25.0f, 25.0f, 177, 177, 177, 255};
    float velocityX = 5.0f;
    float velocityY = 5.0f;
};

int main()
{
    const auto FPS = std::chrono::duration<float>(1.0f / 60.0f);

    Window win = WIN_INIT("PONG", 800, 400, SDL_WINDOW_MAXIMIZED);
    GameData data;

    Rect paddle{50.0f, 150.0f, 20.0f, 100.0f, 255, 255, 255, 255};
    Rect enemyPaddle{700.0f, 150.0f, 20.0f, 100.0f, 255, 255, 255, 255};
    Input input;
    Input enemyInput;
    Rect ballPlane{400.0f, 200.0f, 25.0f, 25.0f, 177, 177, 177, 255};

    float velocityX = 5.0f;
    float velocityY = 5.0f;

    GameState state = GameState::GAMEPLAY;

    while (win.active)
    {
        auto frameStart = std::chrono::steady_clock::now();
        CLEAR(win);
        UPDATE_INPUT(win, input);
        if (state == GameState::GAMEPLAY)
        {
            ENEMY_INPUT(enemyPaddle, ballPlane, enemyInput);
            DRAW_RECT(win, paddle);
            DRAW_RECT(win, ballPlane);
            DRAW_RECT(win, enemyPaddle);
            MOVE_RECT(input, paddle);
            MOVE_RECT(enemyInput, enemyPaddle);
            PHYSICS_BALL(ballPlane, paddle, enemyPaddle, velocityX, velocityY);

            if (ballPlane.pos.x + ballPlane.size.w < 0)
            {
                std::cout << "LOSS" << std::endl;
                state = GameState::LOSS_CONDITION;
            };
            if (ballPlane.pos.x + ballPlane.size.w > 800)
            {
                std::cout << "LOSS" << std::endl;
                state = GameState::WIN_CONDITION;
            };
        }
        if (state == GameState::LOSS_CONDITION || state == GameState::WIN_CONDITION)
        {
            if (input.space)
            {
                paddle = data.paddle;
                enemyPaddle = data.enemyPaddle;
                enemyInput = data.enemyInput;
                input = data.input;
                velocityX = data.velocityX;
                velocityY = data.velocityY;
                ballPlane = data.ballPlane;
                state = GameState::GAMEPLAY;
            };
        }
        PRESENT(win);
        auto frameElapsed = std::chrono::steady_clock::now() - frameStart;
        if (frameElapsed < FPS)
        {
            std::this_thread::sleep_for(FPS - frameElapsed);
        };
    }

    WIN_END(win);
}