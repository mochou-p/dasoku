// ashi

#pragma once

#include "window.hpp"
#include "device.hpp"
#include "game_object.hpp"
#include "renderer.hpp"

#include <memory>
#include <vector>

namespace ashi
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

            void initImgui();

            VkDescriptorPool imguiPool;

            AshiWindow ashiWindow { WIDTH, HEIGHT, "ashi" };
            AshiDevice ashiDevice { ashiWindow };
            AshiRenderer ashiRenderer { ashiWindow, ashiDevice };

            std::vector<AshiGameObject> gameObjects;
    };
}
