#define SDL_MAIN_USE_CALLBACKS 1

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

// =============================================
// Application State Structure
// =============================================
typedef struct AppState
{
    SDL_Window*   window;
    SDL_Renderer* renderer;
    // Add more fields here as needed
    // bool running;
    // int score;
    // etc.
} AppState;


// Forward declarations
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]);
SDL_AppResult SDL_AppIterate(void *appstate);
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event);
void SDL_AppQuit(void *appstate, SDL_AppResult result);


// =============================================
// Init
// =============================================
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // AppState *state = SDL_calloc(1, sizeof(AppState));  // Zero-initialized
    AppState *state = static_cast<AppState*>(SDL_calloc(1, sizeof(AppState)));
    if (!state)
    {
        SDL_Log("Failed to allocate AppState");
        return SDL_APP_FAILURE;
    }

    state->window = SDL_CreateWindow(
        "SDL3 Modern App Style",
        1024, 720,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY
    );

    if (!state->window)
    {
        SDL_Log("Window creation failed: %s", SDL_GetError());
        SDL_free(state);
        return SDL_APP_FAILURE;
    }

    state->renderer = SDL_CreateRenderer(state->window, nullptr);
    if (!state->renderer)
    {
        SDL_Log("Renderer creation failed: %s", SDL_GetError());
        SDL_DestroyWindow(state->window);
        SDL_free(state);
        return SDL_APP_FAILURE;
    }

    *appstate = state;        // ← Important: Pass ownership to SDL

    SDL_Log("SDL3 App initialized successfully!");
    return SDL_APP_CONTINUE;
}


// =============================================
// Main Loop
// =============================================
SDL_AppResult SDL_AppIterate(void *appstate)
{
    if (!appstate) return SDL_APP_CONTINUE;

    AppState *state = static_cast<AppState*>(appstate);   // ← Explicit cast

    SDL_SetRenderDrawColor(state->renderer, 25, 25, 35, 255);
    SDL_RenderClear(state->renderer);

    // TODO: Draw everything using state->renderer

    SDL_RenderPresent(state->renderer);
    return SDL_APP_CONTINUE;
}


// =============================================
// Event Handler
// =============================================
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    AppState *state = static_cast<AppState*>(appstate);   // ← Explicit cast

    if (event->type == SDL_EVENT_QUIT)
        return SDL_APP_SUCCESS;

    // Example: Toggle fullscreen with F
    if (event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_F)
    {
        static bool fullscreen = false;
        fullscreen = !fullscreen;
        SDL_SetWindowFullscreen(state->window, fullscreen);
    }

    return SDL_APP_CONTINUE;
}


// =============================================
// Cleanup
// =============================================
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    AppState *state = static_cast<AppState*>(appstate);   // ← Explicit cast

    if (state)
    {
        if (state->renderer)
            SDL_DestroyRenderer(state->renderer);

        if (state->window)
            SDL_DestroyWindow(state->window);

        SDL_free(state);
    }

    SDL_Quit();
    SDL_Log("SDL3 App shutdown cleanly.");
}