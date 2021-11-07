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
    struct GlobalUbo
    {
        alignas(16) glm::mat4 projectionView {1.0f};
        alignas(16) glm::vec3 lightDirection = glm::normalize(glm::vec3{1.0f, -3.0f, -1.0f});
    };

    App::App()
    {
        globalPool =
            DskDescriptorPool::Builder(dskDevice)
                .setMaxSets(DskSwapChain::MAX_FRAMES_IN_FLIGHT + 3)
                .addPoolSize
                (
                    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    DskSwapChain::MAX_FRAMES_IN_FLIGHT + 3
                )
                .addPoolSize
                (
                    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                    10
                )
                .build();

        loadGameObjects(*globalPool);
    }

    App::~App()
    {
        vkDestroyDescriptorPool(dskDevice.device(), imguiPool, nullptr);
    }

    void App::run()
    {
        std::vector<std::unique_ptr<DskBuffer>> uboBuffers(DskSwapChain::MAX_FRAMES_IN_FLIGHT);

        for (int i = 0; i < uboBuffers.size(); i++)
        {
            uboBuffers[i] = std::make_unique<DskBuffer>
            (
                dskDevice,
                sizeof(GlobalUbo),
                1,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
            );
            uboBuffers[i]->map();
        }

        auto globalSetLayout = DskDescriptorSetLayout::Builder(dskDevice)
            .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
            .build();
        
        std::vector<VkDescriptorSet> globalDescriptorSets(DskSwapChain::MAX_FRAMES_IN_FLIGHT);
        
        for (int i = 0; i < globalDescriptorSets.size(); i++)
        {
            auto bufferInfo = uboBuffers[i]->descriptorInfo();
            DskDescriptorWriter(*globalSetLayout, *globalPool)
                .writeBuffer(0, &bufferInfo)
                .build(globalDescriptorSets[i]);
        }

        
        VkDescriptorImageInfo imageInfos[gameObjects.size()];

        for (int i = 0; i < gameObjects.size(); i++)
        {
            imageInfos[i].sampler = nullptr;
            imageInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfos[i].imageView = textures[i].imageView;
        }

        VkSamplerCreateInfo samplerInfo {};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;

        VkSampler sampler;
        vkCreateSampler(dskDevice.device(), &samplerInfo, nullptr, &sampler);

        VkDescriptorImageInfo samplerImageInfo {};
        samplerImageInfo.sampler = sampler;
        samplerImageInfo.imageView = nullptr;
        samplerImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        auto textureSetLayout = DskDescriptorSetLayout::Builder(dskDevice)
            .addBinding
            (
                0,
                VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
                VK_SHADER_STAGE_FRAGMENT_BIT,
                gameObjects.size()
            )
            .addBinding
            (
                1,
                VK_DESCRIPTOR_TYPE_SAMPLER,
                VK_SHADER_STAGE_FRAGMENT_BIT
            )
            .build();
        
        VkDescriptorSet textureDescriptorSet;
        DskDescriptorWriter(*textureSetLayout, *globalPool)
            .writeImage(0, imageInfos, gameObjects.size())
            .writeImage(1, &samplerImageInfo, 1)
            .build(textureDescriptorSet);

        DskRenderSystem dskRenderSystem
        {
            dskDevice,
            dskRenderer.getSwapChainRenderPass(),
            globalSetLayout->getDescriptorSetLayout(),
            textureSetLayout->getDescriptorSetLayout()
        };

        DskCamera camera {};
        camera.setViewTarget
        (
            glm::vec3(-1.0f, -2.0f, 2.0f),
            glm::vec3(0.0f, 0.0f, 2.5f)
        );

        auto viewerObject = DskGameObject::createGameObject();
        DskKeyboardMovementController cameraController {};

        initImGui();

        auto currentTime = std::chrono::high_resolution_clock::now();

        while (!dskWindow.shouldClose())
        {
            glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration
                <float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            cameraController.moveInPlaneXZ
            (
                dskWindow.getGLFWwindow(),
                frameTime,
                viewerObject
            );
            camera.setViewYXZ
            (
                viewerObject.transform3d.translation,
                viewerObject.transform3d.rotation
            );

            float aspect = dskRenderer.getAspectRatio();
            camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 50.0f);

            setupImGui(viewerObject.transform3d.translation);

            if (auto commandBuffer = dskRenderer.beginFrame())
            {
                int frameIndex = dskRenderer.getFrameIndex();
                DskFrameInfo frameInfo
                {
                    frameIndex,
                    frameTime,
                    commandBuffer,
                    camera,
                    globalDescriptorSets[frameIndex],
                    textureDescriptorSet,
                    {
                        globalDescriptorSets[frameIndex],
                        textureDescriptorSet
                    }
                };

                // update
                GlobalUbo ubo {};
                ubo.projectionView = camera.getProjection() * camera.getView();
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                // render
                dskRenderer.beginSwapChainRenderPass(commandBuffer);
                dskRenderSystem.renderGameObjects(frameInfo, gameObjects);
                renderImGui(commandBuffer);
                dskRenderer.endSwapChainRenderPass(commandBuffer);
                dskRenderer.endFrame();
            }

            gameObjects[2].setRotation
            ({
                0.0f,
                gameObjects[2].getRotation().y + glm::radians(0.2f),
                0.0f
            });
        }

        cleanup(sampler, imageInfos);
    }

    void App::loadGameObjects(DskDescriptorPool &globalPool)
    {
        auto cube = DskGameObject::createGameObject()
            .setModel(dskDevice, ".\\resources\\models\\colored_cube.obj")
            .setTexture(dskDevice, ".\\resources\\textures\\Floor.png", textures)
            .setTranslation({0.0f, 0.3f, 2.5f})
            .setScale({10.0f, 0.2f, 10.0f})
            .build(&gameObjects);

        auto vase = DskGameObject::createGameObject()
            .setModel(dskDevice, ".\\resources\\models\\smooth_vase.obj")
            .setTexture(dskDevice, ".\\resources\\textures\\Paper.png", textures)
            .setTranslation({0.3f, 0.1f, 2.5f})
            .setScale(glm::vec3(1.0f))
            .build(&gameObjects);
        
        auto catgirl = DskGameObject::createGameObject()
            .setModel(dskDevice, ".\\resources\\models\\neco-arc\\PBR - Metallic Roughness.obj")
            .setTexture(dskDevice, ".\\resources\\models\\neco-arc\\BaseColor.png", textures)
            .setTranslation({-0.3f, 0.1f, 2.5f})
            .setScale({0.3f, -0.3f, 0.3f})
            .build(&gameObjects);
    }

    void App::initImGui()
    {
        VkDescriptorPoolSize pool_sizes[] =
            {
                {VK_DESCRIPTOR_TYPE_SAMPLER,                1000},
                {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
                {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,          1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,          1000},
                {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,   1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,   1000},
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,         1000},
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
                {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,       1000}
            };

        VkDescriptorPoolCreateInfo pool_info {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets = 1000;
        pool_info.poolSizeCount = std::size(pool_sizes);
        pool_info.pPoolSizes = pool_sizes;

        vkCreateDescriptorPool(dskDevice.device(), &pool_info, nullptr, &imguiPool);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO &io = ImGui::GetIO();

        ImGui_ImplGlfw_InitForVulkan(dskWindow.getGLFWwindow(), true);

        ImGui_ImplVulkan_InitInfo init_info {};
        init_info.Instance = dskDevice.getInstance();
        init_info.PhysicalDevice = dskDevice.getPhysicalDevice();
        init_info.Device = dskDevice.device();
        init_info.Queue = dskDevice.getGraphicsQueue();
        init_info.DescriptorPool = imguiPool;
        init_info.MinImageCount = 3;
        init_info.ImageCount = 3;
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

        ImGui_ImplVulkan_Init(&init_info, dskRenderer.getSwapChainRenderPass());

        ImGui::StyleColorsDark();

        auto textureCommandBuffer = dskDevice.beginSingleTimeCommands();
        ImGui_ImplVulkan_CreateFontsTexture(textureCommandBuffer);
        dskDevice.endSingleTimeCommands(textureCommandBuffer);

        ImGui_ImplVulkan_DestroyFontUploadObjects();
    }

    void App::setupImGui(glm::vec3 viewerPos)
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();

        ImGui::PushStyleColor(ImGuiCol_TitleBg,  {0.0f, 0.0f, 0.0f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_WindowBg, {0.0f, 0.0f, 0.0f, 0.8f});
        ImGui::PushStyleColor(ImGuiCol_Border,   {0.0f, 0.0f, 0.0f, 1.0f});

        ImGui::Begin
        (
            "Frametime",
            nullptr,
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoFocusOnAppearing
        );
            ImGui::Text
            (
                "%.2fms (%.0f FPS)",
                1000.0f / ImGui::GetIO().Framerate,
                ImGui::GetIO().Framerate
            );
        ImGui::End();

        ImGui::Begin
        (
            "Viewer",
            nullptr,
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoFocusOnAppearing
        );
            ImGui::Text("x: %.4f", viewerPos.x);
            ImGui::Text("y: %.4f", -viewerPos.y);
            ImGui::Text("z: %.4f", viewerPos.z);
        ImGui::End();

        ImGui::PopStyleColor(3);

        ImGui::Render();
    }
    
    void App::renderImGui(VkCommandBuffer commandBuffer)
    {
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
    }

    void App::cleanup
    (
        VkSampler sampler,
        VkDescriptorImageInfo *imageInfos
    )
    {
        vkDeviceWaitIdle(dskDevice.device());

        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        vkDestroySampler(dskDevice.device(), sampler, nullptr);
        for (int i = 0; i < gameObjects.size(); i++)
        {
            vkDestroyImageView(dskDevice.device(), imageInfos[i].imageView, nullptr);
        }

        for (int i = 0; i < gameObjects.size(); i++)
        {
            textures[i].cleanup(dskDevice);
        }
    }
}
