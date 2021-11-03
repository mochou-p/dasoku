// dsk

#pragma once

#include "window.hpp"
#include "device.hpp"
#include "game_object.hpp"
#include "renderer.hpp"
#include "descriptors.hpp"

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

            void initImGui();
            void setupImGui(glm::vec3 viewerPos);
            void renderImGui(VkCommandBuffer commandBuffer);

            VkDescriptorPool imguiPool;

            DskWindow dskWindow {WIDTH, HEIGHT, "dsk"};
            DskDevice dskDevice {dskWindow};
            DskRenderer dskRenderer {dskWindow, dskDevice};

            // order of decls matters
            std::unique_ptr<DskDescriptorPool> globalPool {};
            std::vector<DskGameObject> gameObjects;
    };
}
