// zzz

#pragma once

#include "zzz_window.hpp"
#include "zzz_device.hpp"
#include "zzz_game_object.hpp"
#include "zzz_renderer.hpp"

#include <memory>
#include <vector>

namespace zzz
{
    class SimpleApp
    {
        public:
            static constexpr int WIDTH = 800;
            static constexpr int HEIGHT = 600;

            SimpleApp();
            ~SimpleApp();

            SimpleApp(const SimpleApp &) = delete;
            SimpleApp &operator=(const SimpleApp &) = delete;

            void run();

        private:
            void loadGameObjects();

            ZzzWindow zzzWindow { WIDTH, HEIGHT, "zzz" };
            ZzzDevice zzzDevice { zzzWindow };
            ZzzRenderer zzzRenderer { zzzWindow, zzzDevice };

            std::vector<ZzzGameObject> gameObjects;
    };
}
