@echo off 
C:/VulkanSDK/1.4.313.0/Bin/glslangValidator.exe -V shaders/triangle.vert -o shaders/triangle.vert.spv
C:/VulkanSDK/1.4.313.0/Bin/glslangValidator.exe -V shaders/triangle.frag -o shaders/triangle.frag.spv
pause