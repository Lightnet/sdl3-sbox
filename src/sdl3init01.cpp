#define SDL_MAIN_USE_CALLBACKS 1   // ← This is important for the new SDL3 style

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

// Forward declarations
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]);
SDL_AppResult SDL_AppIterate(void *appstate);
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event);
void SDL_AppQuit(void *appstate, SDL_AppResult result);

// ------------------------------------------------------------------

SDL_Window*   gWindow   = nullptr;
SDL_Renderer* gRenderer = nullptr;

// Called once at startup
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    gWindow = SDL_CreateWindow(
        "SDL3 Modern App Style",
        1024, 720,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY
    );

    if (!gWindow)
    {
        SDL_Log("Window creation failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    gRenderer = SDL_CreateRenderer(gWindow, nullptr);
    if (!gRenderer)
    {
        SDL_Log("Renderer creation failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_Log("SDL3 Modern App initialized successfully!");
    return SDL_APP_CONTINUE;
}

// Called every frame
SDL_AppResult SDL_AppIterate(void *appstate)
{
    // Clear screen with dark color
    SDL_SetRenderDrawColor(gRenderer, 25, 25, 35, 255);
    SDL_RenderClear(gRenderer);

    // TODO: Draw your game / UI here

    SDL_RenderPresent(gRenderer);
    return SDL_APP_CONTINUE;
}

// Called for every event
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT)
        return SDL_APP_SUCCESS;

    // Example: Press F for fullscreen
    if (event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_F)
    {
        static bool fullscreen = false;
        fullscreen = !fullscreen;
        SDL_SetWindowFullscreen(gWindow, fullscreen);
    }

    return SDL_APP_CONTINUE;
}

// Called before shutdown
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    if (gRenderer)
        SDL_DestroyRenderer(gRenderer);
    if (gWindow)
        SDL_DestroyWindow(gWindow);

    SDL_Quit();
    SDL_Log("SDL3 App shutdown cleanly.");
}