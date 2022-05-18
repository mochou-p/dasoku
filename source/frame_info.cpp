// dsk

#include "app.hpp"
#include "render_system.hpp"
#include "camera.hpp"
#include "keyboard_movement_controller.hpp"
#include "buffer.hpp"

#include "imgui.h"
#include "imgui_impl_vulkan.h"
#include "imgui_impl_glfw.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <chrono>

namespace dsk
{
    void DskFrameInfo::init
    (
        int frameIndex,
        float frameTime,
        VkCommandBuffer commandBuffer,
        DskCamera &camera,
        VkDescriptorSet globalDescriptorSet,
        VkDescriptorSet textureDescriptorSet
    )
    {
        this->frameIndex = frameIndex;
        this->frameTime = frameTime;
        this->commandBuffer = commandBuffer;
        this->camera = camera;
        this->globalDescriptorSet = globalDescriptorSet;
        this->textureDescriptorSet = textureDescriptorSet;
        this->sets[0] = globalDescriptorSet;   // hereherehere
        this->sets[1] = textureDescriptorSet;  // hereherehere
    }
}
