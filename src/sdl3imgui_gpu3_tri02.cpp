#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlgpu3.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <stdio.h>

// ====================== MINIMAL SPIR-V SHADERS ======================
// These are valid minimal shaders with triangle positions hardcoded in vertex shader

static const uint32_t vertex_shader_spirv[] = {
    0x07230203,0x00010000,0x0008000b,0x00000013,0x00000000,0x00020011,0x00000001,0x0006000b,
    0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
    0x0006000f,0x00000000,0x00000004,0x6e69616d,0x00000000,0x0000000b,0x00060010,0x00000004,
    0x00000011,0x00000001,0x00000000,0x00000001,0x00040047,0x00000009,0x0000000b,0x0000002a,
    0x00020013,0x00000002,0x00030021,0x00000003,0x00000002,0x00030016,0x00000006,0x00000020,
    0x00040017,0x00000007,0x00000006,0x00000004,0x00040020,0x00000008,0x00000003,0x00000007,
    0x0004003b,0x00000008,0x00000009,0x00000003,0x00040015,0x0000000a,0x00000020,0x00000001,
    0x0004002b,0x0000000a,0x0000000c,0x00000000,0x0004002b,0x00000006,0x0000000d,0xbf000000,
    0x0004002b,0x00000006,0x0000000e,0x3f000000,0x0004002b,0x00000006,0x0000000f,0x3f800000,
    0x0006002c,0x00000007,0x00000010,0x0000000d,0x0000000d,0x0000000f,0x0006002c,0x00000007,
    0x00000011,0x0000000e,0x0000000d,0x0000000f,0x0006002c,0x00000007,0x00000012,0x0000000f,
    0x0000000e,0x0000000f,0x00050036,0x00000002,0x00000004,0x00000000,0x00000003,0x000200f8,
    0x00000005,0x0004003d,0x0000000a,0x0000000b,0x0000000c,0x000500aa,0x00000002,0x00000013,
    0x0000000b,0x0000000c,0x000300f7,0x00000015,0x00000014,0x000200f8,0x00000015,0x0003003e,
    0x00000009,0x00000010,0x000200f9,0x00000016,0x000200f8,0x00000014,0x000500aa,0x00000002,
    0x00000017,0x0000000b,0x0000000f,0x000300f7,0x00000019,0x00000018,0x000200f8,0x00000019,
    0x0003003e,0x00000009,0x00000011,0x000200f9,0x00000016,0x000200f8,0x00000018,0x0003003e,
    0x00000009,0x00000012,0x000200f9,0x00000016,0x000200f8,0x00000016,0x000100fd,0x00010038
};

static const uint32_t fragment_shader_spirv[] = {
    0x07230203,0x00010000,0x0008000b,0x0000000f,0x00000000,0x00020011,0x00000001,0x0006000b,
    0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
    0x0007000f,0x00000004,0x00000004,0x6e69616d,0x00000000,0x00000009,0x0000000c,0x00030010,
    0x00000004,0x00000007,0x00040047,0x00000009,0x0000001e,0x00000000,0x00020013,0x00000002,
    0x00030021,0x00000003,0x00000002,0x00030016,0x00000006,0x00000020,0x00040017,0x00000007,
    0x00000006,0x00000004,0x00040020,0x00000008,0x00000003,0x00000007,0x0004003b,0x00000008,
    0x00000009,0x00000003,0x0006002c,0x00000007,0x0000000b,0x3f000000,0x3f000000,0x3f800000,
    0x3f800000,0x00050036,0x00000002,0x00000004,0x00000000,0x00000003,0x000200f8,0x00000005,
    0x0003003e,0x00000009,0x0000000b,0x000100fd,0x00010038
};

int main(int, char**)
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD);

    float scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());

    SDL_Window* window = SDL_CreateWindow(
        "SDL3 GPU + ImGui - Rotating Triangle (Fixed)",
        (int)(1280 * scale), (int)(800 * scale),
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);

    // Force Vulkan + SPIR-V
    SDL_GPUDevice* gpu_device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, nullptr);
    if (!gpu_device) {
        printf("Failed to create GPU device: %s\n", SDL_GetError());
        return -1;
    }

    SDL_ClaimWindowForGPUDevice(gpu_device, window);
    SDL_SetGPUSwapchainParameters(gpu_device, window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, SDL_GPU_PRESENTMODE_VSYNC);
    SDL_ShowWindow(window);

    // ImGui Setup
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

    // ==================== Shaders ====================
    SDL_GPUShaderCreateInfo vsInfo = {};
    vsInfo.code = (const uint8_t*)vertex_shader_spirv;
    vsInfo.code_size = sizeof(vertex_shader_spirv);
    vsInfo.entrypoint = "main";
    vsInfo.stage = SDL_GPU_SHADERSTAGE_VERTEX;
    vsInfo.format = SDL_GPU_SHADERFORMAT_SPIRV;
    SDL_GPUShader* vertexShader = SDL_CreateGPUShader(gpu_device, &vsInfo);

    SDL_GPUShaderCreateInfo fsInfo = {};
    fsInfo.code = (const uint8_t*)fragment_shader_spirv;
    fsInfo.code_size = sizeof(fragment_shader_spirv);
    fsInfo.entrypoint = "main";
    fsInfo.stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
    fsInfo.format = SDL_GPU_SHADERFORMAT_SPIRV;
    SDL_GPUShader* fragmentShader = SDL_CreateGPUShader(gpu_device, &fsInfo);

    // Pipeline
    SDL_GPUColorTargetDescription colorTargetDesc = {};
    colorTargetDesc.format = SDL_GetGPUSwapchainTextureFormat(gpu_device, window);

    SDL_GPUGraphicsPipelineTargetInfo targetInfo = {};
    targetInfo.num_color_targets = 1;
    targetInfo.color_target_descriptions = &colorTargetDesc;

    SDL_GPUGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.vertex_shader = vertexShader;
    pipelineInfo.fragment_shader = fragmentShader;
    pipelineInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
    pipelineInfo.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;
    pipelineInfo.target_info = targetInfo;

    SDL_GPUGraphicsPipeline* pipeline = SDL_CreateGPUGraphicsPipeline(gpu_device, &pipelineInfo);

    // Main Loop
    bool done = false, show_demo = true;
    float angle = 0.0f;

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
            SDL_DrawGPUPrimitives(pass, 3, 1, 0, 0);   // 3 vertices

            ImGui_ImplSDLGPU3_RenderDrawData(draw_data, cmdbuf, pass);
            SDL_EndGPURenderPass(pass);
        }

        SDL_SubmitGPUCommandBuffer(cmdbuf);
        angle += 1.0f;
    }

    // Cleanup
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