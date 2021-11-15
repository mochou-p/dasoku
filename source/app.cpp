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

    App::~App() {}

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

        //Fix for MSVC.... but you later have to delete `imageInfos` var
        VkDescriptorImageInfo *imageInfos = new VkDescriptorImageInfo[gameObjects.size()];

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
            glm::vec3(-1.0f, -2.5f, 2.0f),
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
                viewerObject.transform.translation,
                viewerObject.transform.rotation
            );

            float aspect = dskRenderer.getAspectRatio();
            camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 50.0f);

            setupImGui();

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

                GlobalUbo ubo {};
                ubo.projectionView = camera.getProjection() * camera.getView();
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                dskRenderer.beginSwapChainRenderPass(commandBuffer);
                dskRenderSystem.renderGameObjects(frameInfo, gameObjects);
                renderImGui(commandBuffer);
                dskRenderer.endSwapChainRenderPass(commandBuffer);
                dskRenderer.endFrame();
            }
        }

        cleanup(sampler, imageInfos);
    }

    void App::loadGameObjects(DskDescriptorPool &globalPool)
    {
        // will be setting a default texture
        // instead of a color for now, make
        // sure to change cleanup loops when
        // changing this

        auto cube = DskGameObject::createGameObject()
            .setTag("Cube")
            .setModel("colored_cube.obj", dskDevice)
            .setTexture("default.png", textures, dskDevice)
            .setTranslation({0.0f, 0.3f, 2.5f})
            .setScale({10.0f, 0.2f, 10.0f})
            .build(&gameObjects);

        auto vase = DskGameObject::createGameObject()
            .setTag("Vase")
            .setModel("smooth_vase.obj", dskDevice)
            .setTexture("default.png", textures, dskDevice)
            .setTranslation({0.3f, 0.1f, 2.5f})
            .setScale(glm::vec3(1.0f))
            .build(&gameObjects);
        
        auto catgirl = DskGameObject::createGameObject()
            .setTag("Catgirl")
            .setModel("neco_arc.obj", dskDevice)
            .setTexture("BaseColor.png", textures, dskDevice)
            .setTranslation({-0.3f, 0.1f, 2.5f})
            .setScale({0.3f, -0.3f, 0.3f})
            .setRotation({0.0f, glm::radians(75.0f), 0.0f})
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

    void App::setupImGui()
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();

        ImGui::PushStyleColor(ImGuiCol_TitleBg,       {0.0f, 0.0f, 0.0f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_TitleBgActive, {0.0f, 0.0f, 0.0f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_WindowBg,      {0.0f, 0.0f, 0.0f, 0.7f});
        ImGui::PushStyleColor(ImGuiCol_Border,        {0.0f, 0.0f, 0.0f, 0.0f});
        ImGui::PushStyleColor(ImGuiCol_Button,        {0.0f, 0.0f, 0.0f, 0.0f});
        ImGui::PushStyleColor(ImGuiCol_TabActive,     {0.0f, 0.0f, 0.0f, 1.0f});

        ImGuiWindowFlags windowFlags =
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoFocusOnAppearing;

        ImGui::Begin
        (
            "Frametime",
            nullptr,
            windowFlags
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
            "Hierarchy",
            nullptr,
            windowFlags
        );
            for (auto &obj : gameObjects)
            {
                auto active = obj.getId() == activeObj;

                if (active)
                { ImGui::PushStyleColor(ImGuiCol_Button, {0.3f, 0.3f, 0.3f, 0.3f}); }
                if (ImGui::Button(obj.getTag().c_str())) { activeObj = obj.getId(); }
                if (active) { ImGui::PopStyleColor(1); }
            }
        ImGui::End();

        if (activeObj > -1)
        {
            auto obj = &gameObjects[activeObj];

            static glm::vec3 translation {obj->transform.translation};
            static glm::vec3 scale       {obj->transform.scale      };
            static glm::vec3 rotation    {obj->transform.rotation   };

            if (activeObj != lastActiveObj)
            {
                translation = {obj->transform.translation};
                scale       = {obj->transform.scale      };
                rotation    = {obj->transform.rotation   };

                lastActiveObj = activeObj;
            }

            ImGui::Begin
            (
                "Inspector",
                nullptr,
                windowFlags
            );
                ImGui::Text("tag: %s", obj->getTag().c_str());
                ImGui::Dummy(ImVec2(0.0f, 20.0f));
                ImGui::Text("transform:");
                ImGui::Dummy(ImVec2(0.0f, 10.0f));
                ImGui::Text("translation");
                translation.y *= -1;
                ImGui::InputFloat("x", &translation.x, 0.01f, 0.2f, "%.4f");
                ImGui::InputFloat("y", &translation.y, 0.01f, 0.2f, "%.4f");
                ImGui::InputFloat("z", &translation.z, 0.01f, 0.2f, "%.4f");
                translation.y *= -1;
                ImGui::Dummy(ImVec2(0.0f, 5.0f));
                ImGui::Text("scale");
                ImGui::InputFloat("x ", &scale.x, 0.01f, 0.2f, "%.4f");
                ImGui::InputFloat("y ", &scale.y, 0.01f, 0.2f, "%.4f");
                ImGui::InputFloat("z ", &scale.z, 0.01f, 0.2f, "%.4f");
                ImGui::Dummy(ImVec2(0.0f, 5.0f));
                ImGui::Text("rotation");
                rotation.x = glm::degrees(rotation.x);
                rotation.y = glm::degrees(rotation.y);
                rotation.z = glm::degrees(rotation.z);
                if (rotation.x < 0)   { rotation.x = fmod(rotation.x, 360) + 360; }
                if (rotation.y < 0)   { rotation.y = fmod(rotation.y, 360) + 360; }
                if (rotation.z < 0)   { rotation.z = fmod(rotation.z, 360) + 360; }
                if (rotation.x >= 360) { rotation.x = fmod(rotation.x, 360); }
                if (rotation.y >= 360) { rotation.y = fmod(rotation.y, 360); }
                if (rotation.z >= 360) { rotation.z = fmod(rotation.z, 360); }
                ImGui::InputFloat("x  ", &rotation.x, 1.0f, 10.0f, "%.0f");
                ImGui::InputFloat("y  ", &rotation.y, 1.0f, 10.0f, "%.0f");
                ImGui::InputFloat("z  ", &rotation.z, 1.0f, 10.0f, "%.0f");
                rotation.x = glm::radians(rotation.x);
                rotation.y = glm::radians(rotation.y);
                rotation.z = glm::radians(rotation.z);
            ImGui::End();

            if (obj->transform.translation != translation) { obj->setTranslation(translation); }
            if (obj->transform.scale       != scale      ) { obj->setScale(scale);             }
            if (obj->transform.rotation    != rotation   ) { obj->setRotation(rotation);       }
        }

        ImGui::PopStyleColor(6);

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

        vkDestroyDescriptorPool(dskDevice.device(), imguiPool, nullptr);

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
