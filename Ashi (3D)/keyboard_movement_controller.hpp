// ashi

#pragma once

#include "game_object.hpp"
#include "window.hpp"

namespace ashi
{
    class AshiKeyboardMovementController
    {
        public:
            struct KeyMappings {
                int moveLeft     = GLFW_KEY_A;
                int moveRight    = GLFW_KEY_D;
                int moveForward  = GLFW_KEY_W;
                int moveBackward = GLFW_KEY_S;
                int moveDown     = GLFW_KEY_LEFT_SHIFT;
                int moveUp       = GLFW_KEY_SPACE;
                int lookLeft     = GLFW_KEY_LEFT;
                int lookRight    = GLFW_KEY_RIGHT;
                int lookUp       = GLFW_KEY_UP;
                int lookDown     = GLFW_KEY_DOWN;
            };

            void moveInPlaneXZ
            (
                GLFWwindow *window,
                float dt,
                AshiGameObject &gameObjects
            );

            KeyMappings keys {};
            float moveSpeed {3.0f};
            float turnSpeed {1.5f};
    };
}
