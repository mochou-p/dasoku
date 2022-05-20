// dsk

#pragma once

#include "window.hpp"
#include "device.hpp"
#include "game_object.hpp"
#include "renderer.hpp"
#include "descriptors.hpp"
#include "texture.hpp"
#include "render_system.hpp"
#include "camera.hpp"
#include "keyboard_movement_controller.hpp"

#include <memory>
#include <vector>

namespace dsk
{
    class App
    {
        public:
            static constexpr int WIDTH  = 800;
            static constexpr int HEIGHT = 600;

            App();
            ~App();

            App(const App &) = delete;
            App &operator=(const App &) = delete;

            void Run();

        private:
            DskWindow dskWindow {WIDTH, HEIGHT, "Dasoku"};
            DskDevice dskDevice {dskWindow};
            DskRenderer dskRenderer {dskWindow, dskDevice};
            DskRenderSystem dskRenderSystem;

            std::unique_ptr<DskDescriptorPool> globalPool {};
            std::vector<DskGameObject> gameObjects;

            void LoadGameObjects();

            int activeObj     = -1;
            int lastActiveObj = -1;

            VkDescriptorPool imguiPool;
            void InitImGui();

            int renderFlags = 0;
            bool rfLight = true;
            bool rfTex   = true;
            void SetupImGui();

            void RenderImGui(VkCommandBuffer commandBuffer);

            std::vector<std::unique_ptr<DskBuffer>> uboBuffers =
                std::vector<std::unique_ptr<DskBuffer>>(DskSwapChain::MAX_FRAMES_IN_FLIGHT);
            void InitUniformBuffers();

            std::unique_ptr<dsk::DskDescriptorSetLayout> globalSetLayout;
            std::vector<VkDescriptorSet> globalDescriptorSets =
                std::vector<VkDescriptorSet>(DskSwapChain::MAX_FRAMES_IN_FLIGHT);
            void DescriptorSets();

            VkDescriptorImageInfo *imageInfos;
            DskTexture textures[32];  // hereherehere
            VkSampler sampler;
            std::unique_ptr<dsk::DskDescriptorSetLayout> textureSetLayout;
            VkDescriptorSet textureDescriptorSet;
            void Textures();

            void RenderSystem();

            DskCamera camera {};
            DskGameObject viewerObject;
            void InitCamera();

            DskKeyboardMovementController cameraController {};
            void UpdateCamera(float frameTime);

            void Billboards();

            int frameIndex;
            DskFrameInfo frameInfo;
            void FrameInfo(float frameTime, VkCommandBuffer commandBuffer);

            void UpdateUniformBuffers();

            void Cleanup(VkSampler sampler, VkDescriptorImageInfo *imageInfos);
    };
}
