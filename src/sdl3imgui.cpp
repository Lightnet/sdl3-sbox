#include <SDL3/SDL.h>
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include <iostream>

int main(int argc, char* argv[]) {
    // 1. Initialize SDL3 Subsystems
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    // 2. Create an Application Window
    SDL_Window* window = SDL_CreateWindow(
        "SDL3 + Dear ImGui Window", 
        1280, 720, 
        SDL_WINDOW_RESIZABLE
    );
    if (!window) {
        std::cerr << "Window Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // 3. Create the Accelerated Renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cerr << "Renderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // 4. Setup Dear ImGui Context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    // 5. Initialize Platform and Renderer Backends for SDL3
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    // 6. Main Application Loop
    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            // Forward events directly to Dear ImGui
            ImGui_ImplSDL3_ProcessEvent(&event);
            
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        // Start the Dear ImGui Frame
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        // 7. Define the GUI Layout
        ImGui::Begin("Control Panel");
        ImGui::Text("Hello, world! This is SDL3 + Dear ImGui.");
        static float color[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
        ImGui::ColorEdit4("Clear Color", color);
        if (ImGui::Button("Exit Application")) {
            running = false;
        }
        ImGui::End();

        // 8. Rendering Pass
        ImGui::Render();
        SDL_SetRenderDrawColorFloat(renderer, color[0], color[1], color[2], color[3]);
        SDL_RenderClear(renderer);
        
        // Render the ImGui draw data over the background
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }

    // 9. Cleanup and Memory Management
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
