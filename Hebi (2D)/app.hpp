// hebi

#pragma once

#include "window.hpp"
#include "device.hpp"
#include "game_object.hpp"
#include "renderer.hpp"

#include <memory>
#include <vector>

namespace hebi
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

            HebiWindow hebiWindow { WIDTH, HEIGHT, "hebi" };
            HebiDevice hebiDevice { hebiWindow };
            HebiRenderer hebiRenderer { hebiWindow, hebiDevice };

            std::vector<HebiGameObject> gameObjects;
    };
}
