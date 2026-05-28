#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlgpu3.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <stdio.h>

// ====================== EMBEDDED SPIR-V SHADERS ======================

// Vertex Shader (SPIR-V)
static const uint32_t vertex_shader_spirv[] = {
    0x07230203,0x00010000,0x0008000b,0x0000002a,0x00000000,0x00020011,0x00000001,0x0006000b,
    0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
    0x0006000f,0x00000000,0x00000004,0x6e69616d,0x00000000,0x0000000a,0x00060010,0x00000004,
    0x00000011,0x00000001,0x00000000,0x00000001,0x00040047,0x00000009,0x0000000b,0x0000002a,
    0x00040047,0x0000000f,0x0000000b,0x0000002a,0x00040047,0x0000001e,0x0000000b,0x0000002a,
    0x00050048,0x0000001e,0x00000000,0x0000000b,0x00000000,0x00050048,0x0000001e,0x00000001,
    0x0000000b,0x00000010,0x00030047,0x0000001e,0x00000002,0x00040047,0x00000029,0x00000022,
    0x00000000,0x00020013,0x00000002,0x00030021,0x00000003,0x00000002,0x00030016,0x00000006,
    0x00000020,0x00040017,0x00000007,0x00000006,0x00000004,0x00040017,0x00000008,0x00000006,
    0x00000003,0x00040020,0x00000009,0x00000001,0x00000008,0x0004003b,0x00000009,0x0000000a,
    0x00000001,0x00040020,0x0000000b,0x00000001,0x00000007,0x0004003b,0x0000000b,0x0000000c,
    0x00000001,0x00040015,0x0000000d,0x00000020,0x00000001,0x0004002b,0x0000000d,0x0000000e,
    0x00000000,0x00040020,0x0000000f,0x00000001,0x00000006,0x0004003b,0x0000000f,0x00000010,
    0x00000001,0x0004002b,0x00000006,0x00000012,0x3f000000,0x0004002b,0x00000006,0x00000013,
    0xbf000000,0x0004002b,0x00000006,0x00000014,0x3f800000,0x0006002c,0x00000008,0x00000015,
    0x00000013,0x00000013,0x00000014,0x0006002c,0x00000008,0x00000016,0x00000014,0x00000013,
    0x00000014,0x0006002c,0x00000008,0x00000017,0x00000012,0x00000014,0x00000014,0x00040020,
    0x0000001c,0x00000003,0x00000007,0x0004003b,0x0000001c,0x0000001d,0x00000003,0x0005001e,
    0x0000001e,0x00000007,0x00000007,0x00000006,0x00040020,0x0000001f,0x00000001,0x0000001e,
    0x0004003b,0x0000001f,0x00000020,0x00000001,0x0004002b,0x0000000d,0x00000021,0x00000001,
    0x00040020,0x00000022,0x00000001,0x00000007,0x0004002b,0x0000000d,0x00000027,0x00000002,
    0x00040020,0x00000028,0x00000003,0x00000007,0x0006002c,0x00000007,0x0000002b,0x00000014,
    0x00000014,0x00000014,0x0004003b,0x00000028,0x00000029,0x00000003,0x00050036,0x00000002,
    0x00000004,0x00000000,0x00000003,0x000200f8,0x00000005,0x0004003d,0x00000008,0x0000000b,
    0x0000000a,0x0003003e,0x0000000c,0x0000000b,0x0004003d,0x00000007,0x00000011,0x0000000c,
    0x00050041,0x00000022,0x00000023,0x00000020,0x0000000e,0x0004003d,0x00000007,0x00000024,
    0x00000023,0x00050041,0x00000022,0x00000025,0x00000020,0x00000021,0x0004003d,0x00000007,
    0x00000026,0x00000025,0x00050041,0x00000022,0x0000002a,0x00000020,0x00000027,0x0004003d,
    0x00000006,0x0000002c,0x0000002a,0x0007004f,0x00000007,0x0000002d,0x00000026,0x0000002c,
    0x00000002,0x00000003,0x00050051,0x00000006,0x0000002f,0x00000024,0x00000000,0x00050051,
    0x00000006,0x00000030,0x00000024,0x00000001,0x00050051,0x00000006,0x00000031,0x00000024,
    0x00000002,0x00070050,0x00000007,0x00000032,0x0000002f,0x00000030,0x00000031,0x00000014,
    0x00050041,0x00000028,0x00000033,0x0000001d,0x0000000e,0x0003003e,0x00000033,0x00000032,
    0x00050041,0x00000028,0x00000034,0x0000001d,0x00000021,0x0003003e,0x00000034,0x0000002d,
    0x0003003e,0x00000029,0x0000002b,0x000100fd,0x00010038
};

static const uint32_t fragment_shader_spirv[] = {
    0x07230203,0x00010000,0x0008000b,0x00000013,0x00000000,0x00020011,0x00000001,0x0006000b,
    0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
    0x0007000f,0x00000004,0x00000004,0x6e69616d,0x00000000,0x00000009,0x0000000c,0x00030010,
    0x00000004,0x00000007,0x00040047,0x00000009,0x0000001e,0x00000000,0x00040047,0x0000000c,
    0x0000001e,0x00000000,0x00020013,0x00000002,0x00030021,0x00000003,0x00000002,0x00030016,
    0x00000006,0x00000020,0x00040017,0x00000007,0x00000006,0x00000004,0x00040020,0x00000008,
    0x00000001,0x00000007,0x0004003b,0x00000008,0x00000009,0x00000001,0x00040020,0x0000000a,
    0x00000003,0x00000007,0x0004003b,0x0000000a,0x0000000b,0x00000003,0x0004003b,0x0000000a,
    0x0000000c,0x00000003,0x00050036,0x00000002,0x00000004,0x00000000,0x00000003,0x000200f8,
    0x00000005,0x0004003d,0x00000007,0x0000000d,0x00000009,0x0003003e,0x0000000c,0x0000000d,
    0x000100fd,0x00010038
};


typedef struct {
    float x, y, z;
    float r, g, b, a;
} Vertex;

int main(int, char**)
{
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
    {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return -1;
    }

    float scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());

    SDL_Window* window = SDL_CreateWindow(
        "SDL3 GPU - Rotating Triangle + ImGui",
        (int)(1280 * scale), (int)(800 * scale),
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);

    if (!window) { printf("Window Error: %s\n", SDL_GetError()); return -1; }

    SDL_GPUDevice* gpu_device = SDL_CreateGPUDevice(
        SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_MSL,
        true, nullptr);

    if (!gpu_device) { printf("GPU Device Error: %s\n", SDL_GetError()); return -1; }

    if (!SDL_ClaimWindowForGPUDevice(gpu_device, window))
    {
        printf("Claim Window Error: %s\n", SDL_GetError());
        return -1;
    }

    SDL_SetGPUSwapchainParameters(gpu_device, window,
        SDL_GPU_SWAPCHAINCOMPOSITION_SDR, SDL_GPU_PRESENTMODE_VSYNC);

    SDL_ShowWindow(window);

    // ==================== ImGui ====================
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

    // ==================== Triangle Data ====================
    Vertex vertices[3] = {
        {  0.0f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f },
        { -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 1.0f },
        {  0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f, 1.0f }
    };

    // Create vertex buffer
    SDL_GPUBufferCreateInfo bufferInfo = {};
    bufferInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    bufferInfo.size = sizeof(vertices);
    SDL_GPUBuffer* vertexBuffer = SDL_CreateGPUBuffer(gpu_device, &bufferInfo);

    // Upload data
    SDL_GPUTransferBufferCreateInfo transferInfo = {};
    transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    transferInfo.size = sizeof(vertices);
    SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(gpu_device, &transferInfo);

    void* mapped = SDL_MapGPUTransferBuffer(gpu_device, transferBuffer, false);
    memcpy(mapped, vertices, sizeof(vertices));
    SDL_UnmapGPUTransferBuffer(gpu_device, transferBuffer);

    SDL_GPUCommandBuffer* uploadCmd = SDL_AcquireGPUCommandBuffer(gpu_device);
    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(uploadCmd);

    SDL_GPUTransferBufferLocation src = {};
    src.transfer_buffer = transferBuffer;
    src.offset = 0;

    SDL_GPUBufferRegion dst = {};
    dst.buffer = vertexBuffer;
    dst.offset = 0;
    dst.size = sizeof(vertices);

    SDL_UploadToGPUBuffer(copyPass, &src, &dst, false);
    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(uploadCmd);
    SDL_ReleaseGPUTransferBuffer(gpu_device, transferBuffer);

    bool show_demo = true;
    bool done = false;
    float angle = 0.0f;

    while (!done)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT)
                done = true;
        }

        // ImGui
        ImGui_ImplSDLGPU3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        if (show_demo)
            ImGui::ShowDemoWindow(&show_demo);

        ImGui::Begin("Triangle Controls");
        ImGui::Text("FPS: %.1f", io.Framerate);
        ImGui::Text("Rotation: %.1f", angle);
        ImGui::End();

        ImGui::Render();
        ImDrawData* draw_data = ImGui::GetDrawData();

        // ==================== Rendering ====================
        SDL_GPUCommandBuffer* cmdbuf = SDL_AcquireGPUCommandBuffer(gpu_device);
        SDL_GPUTexture* swapchainTex = nullptr;
        SDL_WaitAndAcquireGPUSwapchainTexture(cmdbuf, window, &swapchainTex, nullptr, nullptr);

        if (swapchainTex)
        {
            ImGui_ImplSDLGPU3_PrepareDrawData(draw_data, cmdbuf);

            SDL_GPUColorTargetInfo colorTarget = {};
            colorTarget.texture = swapchainTex;
            colorTarget.clear_color.r = 0.1f;
            colorTarget.clear_color.g = 0.1f;
            colorTarget.clear_color.b = 0.2f;
            colorTarget.clear_color.a = 1.0f;
            colorTarget.load_op = SDL_GPU_LOADOP_CLEAR;

            SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(cmdbuf, &colorTarget, 1, nullptr);

            // TODO: Bind pipeline and draw triangle here (next step)

            ImGui_ImplSDLGPU3_RenderDrawData(draw_data, cmdbuf, renderPass);
            SDL_EndGPURenderPass(renderPass);
        }

        SDL_SubmitGPUCommandBuffer(cmdbuf);
        angle += 0.8f;   // rotation speed
    }

    // Cleanup
    SDL_WaitForGPUIdle(gpu_device);
    ImGui_ImplSDLGPU3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_ReleaseGPUBuffer(gpu_device, vertexBuffer);
    SDL_ReleaseWindowFromGPUDevice(gpu_device, window);
    SDL_DestroyGPUDevice(gpu_device);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}