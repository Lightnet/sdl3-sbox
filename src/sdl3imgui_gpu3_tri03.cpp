#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlgpu3.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <stdio.h>

/*

triangle.frag
```
#version 450

layout(location = 0) in vec4 inColor;
layout(location = 0) out vec4 outColor;

void main()
{
    outColor = inColor;
}
```
triangle.vert
```
#version 450

layout(location = 0) out vec4 outColor;

void main()
{
    // Hardcoded triangle positions (in clip space)
    const vec2 positions[3] = vec2[](
        vec2( 0.0,  0.5),
        vec2(-0.5, -0.5),
        vec2( 0.5, -0.5)
    );

    const vec3 colors[3] = vec3[](
        vec3(1.0, 0.0, 0.0),  // Red
        vec3(0.0, 1.0, 0.0),  // Green
        vec3(0.0, 0.0, 1.0)   // Blue
    );

    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    outColor = vec4(colors[gl_VertexIndex], 1.0);
}
```
*/


const char* vertex_shader_path = "shaders/triangle.vert.spv";
const char* fragment_shader_path = "shaders/triangle.frag.spv";

int main(int, char**)
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD);

    float scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());

    SDL_Window* window = SDL_CreateWindow(
        "SDL3 GPU + ImGui - Triangle",
        (int)(1280 * scale), (int)(800 * scale),
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);

    SDL_GPUDevice* gpu_device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, nullptr);
    if (!gpu_device) {
        printf("Failed to create GPU device: %s\n", SDL_GetError());
        return -1;
    }

    SDL_ClaimWindowForGPUDevice(gpu_device, window);
    SDL_SetGPUSwapchainParameters(gpu_device, window, 
        SDL_GPU_SWAPCHAINCOMPOSITION_SDR, SDL_GPU_PRESENTMODE_VSYNC);
    SDL_ShowWindow(window);

    // ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(scale);

    ImGui_ImplSDL3_InitForSDLGPU(window);
    ImGui_ImplSDLGPU3_InitInfo init_info = {};
    init_info.Device = gpu_device;
    init_info.ColorTargetFormat = SDL_GetGPUSwapchainTextureFormat(gpu_device, window);
    init_info.MSAASamples = SDL_GPU_SAMPLECOUNT_1;
    ImGui_ImplSDLGPU3_Init(&init_info);

    // ============= Load Shaders from Files =============
    size_t vs_size, fs_size;
    void* vs_data = SDL_LoadFile(vertex_shader_path, &vs_size);
    void* fs_data = SDL_LoadFile(fragment_shader_path, &fs_size);

    if (!vs_data || !fs_data) {
        printf("Failed to load shader files! Make sure shaders/ folder exists.\n");
        return -1;
    }

    SDL_GPUShaderCreateInfo vsInfo = {};
    vsInfo.code = (const uint8_t*)vs_data;
    vsInfo.code_size = vs_size;
    vsInfo.entrypoint = "main";
    vsInfo.stage = SDL_GPU_SHADERSTAGE_VERTEX;
    vsInfo.format = SDL_GPU_SHADERFORMAT_SPIRV;
    SDL_GPUShader* vertexShader = SDL_CreateGPUShader(gpu_device, &vsInfo);

    SDL_GPUShaderCreateInfo fsInfo = {};
    fsInfo.code = (const uint8_t*)fs_data;
    fsInfo.code_size = fs_size;
    fsInfo.entrypoint = "main";
    fsInfo.stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
    fsInfo.format = SDL_GPU_SHADERFORMAT_SPIRV;
    SDL_GPUShader* fragmentShader = SDL_CreateGPUShader(gpu_device, &fsInfo);

    SDL_free(vs_data);
    SDL_free(fs_data);

    // Pipeline
    SDL_GPUColorTargetDescription colorDesc = {};
    colorDesc.format = SDL_GetGPUSwapchainTextureFormat(gpu_device, window);

    SDL_GPUGraphicsPipelineTargetInfo targetInfo = {};
    targetInfo.num_color_targets = 1;
    targetInfo.color_target_descriptions = &colorDesc;

    SDL_GPUGraphicsPipelineCreateInfo pipeInfo = {};
    pipeInfo.vertex_shader = vertexShader;
    pipeInfo.fragment_shader = fragmentShader;
    pipeInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
    pipeInfo.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;
    pipeInfo.target_info = targetInfo;

    SDL_GPUGraphicsPipeline* pipeline = SDL_CreateGPUGraphicsPipeline(gpu_device, &pipeInfo);

    // Main loop
    bool done = false, show_demo = true;

    while (!done)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT) done = true;
        }

        ImGui_ImplSDLGPU3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        if (show_demo) ImGui::ShowDemoWindow(&show_demo);

        ImGui::Begin("Triangle Demo");
        ImGui::Text("FPS: %.1f", io.Framerate);
        ImGui::End();

        ImGui::Render();
        ImDrawData* draw_data = ImGui::GetDrawData();

        SDL_GPUCommandBuffer* cmdbuf = SDL_AcquireGPUCommandBuffer(gpu_device);
        SDL_GPUTexture* swapchainTex = nullptr;
        SDL_WaitAndAcquireGPUSwapchainTexture(cmdbuf, window, &swapchainTex, nullptr, nullptr);

        if (swapchainTex)
        {
            ImGui_ImplSDLGPU3_PrepareDrawData(draw_data, cmdbuf);

            SDL_GPUColorTargetInfo colorInfo = {};
            colorInfo.texture = swapchainTex;
            colorInfo.clear_color = {0.1f, 0.1f, 0.2f, 1.0f};
            colorInfo.load_op = SDL_GPU_LOADOP_CLEAR;

            SDL_GPURenderPass* pass = SDL_BeginGPURenderPass(cmdbuf, &colorInfo, 1, nullptr);

            SDL_BindGPUGraphicsPipeline(pass, pipeline);
            SDL_DrawGPUPrimitives(pass, 3, 1, 0, 0);   // Draw 3 vertices

            ImGui_ImplSDLGPU3_RenderDrawData(draw_data, cmdbuf, pass);
            SDL_EndGPURenderPass(pass);
        }

        SDL_SubmitGPUCommandBuffer(cmdbuf);
    }

    // Cleanup...
    SDL_WaitForGPUIdle(gpu_device);
    SDL_ReleaseGPUGraphicsPipeline(gpu_device, pipeline);
    SDL_ReleaseGPUShader(gpu_device, vertexShader);
    SDL_ReleaseGPUShader(gpu_device, fragmentShader);

    ImGui_ImplSDLGPU3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_ReleaseWindowFromGPUDevice(gpu_device, window);
    SDL_DestroyGPUDevice(gpu_device);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}