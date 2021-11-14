// dsk

#pragma once

#include "window.hpp"
#include "device.hpp"
#include "game_object.hpp"
#include "renderer.hpp"
#include "descriptors.hpp"
#include "texture.hpp"

#include <memory>
#include <vector>

namespace dsk
{
    class App
    {
        public:
            static constexpr int WIDTH = 800;
            static constexpr int HEIGHT = 600;

            App();
            ~App();

            App(const App &) = delete;
            App &operator=(const App &) = delete;

            void run();

        private:
            void loadGameObjects();

            int activeObj = -1;
            int lastActiveObj = -1;

            void initImGui();
            void setupImGui();
            void renderImGui(VkCommandBuffer commandBuffer);

            VkDescriptorPool imguiPool;

            DskTexture textures[8];

            void cleanup
            (
                VkSampler sampler,
                VkDescriptorImageInfo *imageInfos
            );

            DskWindow dskWindow {WIDTH, HEIGHT, "Dasoku"};
            DskDevice dskDevice {dskWindow};
            DskRenderer dskRenderer {dskWindow, dskDevice};

            std::unique_ptr<DskDescriptorPool> globalPool {};
            std::vector<DskGameObject> gameObjects;
    };
}
