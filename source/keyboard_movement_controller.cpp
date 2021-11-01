// dsk

#include "keyboard_movement_controller.hpp"

namespace dsk
{
    void DskKeyboardMovementController::moveInPlaneXZ
    (
        GLFWwindow *window,
        float dt,
        DskGameObject &gameObjects
    )
    {
        glm::vec3 rotate {0.0f};

        if (glfwGetKey(window, keys.lookRight) == GLFW_PRESS) rotate.y += 1.0f;
        if (glfwGetKey(window, keys.lookLeft ) == GLFW_PRESS) rotate.y -= 1.0f;
        if (glfwGetKey(window, keys.lookUp   ) == GLFW_PRESS) rotate.x += 1.0f;
        if (glfwGetKey(window, keys.lookDown ) == GLFW_PRESS) rotate.x -= 1.0f;

        if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon())
        {
            gameObjects.transform3d.rotation += turnSpeed * dt * glm::normalize(rotate);
        }

        // limit pitch (x)
        gameObjects.transform3d.rotation.x = glm::clamp(gameObjects.transform3d.rotation.x, -1.5f, 1.5f);
        gameObjects.transform3d.rotation.y = glm::mod(gameObjects.transform3d.rotation.y, glm::two_pi<float>());

        float yaw = gameObjects.transform3d.rotation.y;
        const glm::vec3 forwardDir {sin(yaw), 0.0f, cos(yaw)};
        const glm::vec3 rightDir {forwardDir.z, 0.0f, -forwardDir.x};
        const glm::vec3 upDir {0.0f, -1.0f, 0.0f};

        glm::vec3 moveDir {0.0f};
        if (glfwGetKey(window, keys.moveForward ) == GLFW_PRESS) moveDir += forwardDir;
        if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS) moveDir -= forwardDir;
        if (glfwGetKey(window, keys.moveRight   ) == GLFW_PRESS) moveDir += rightDir;
        if (glfwGetKey(window, keys.moveLeft    ) == GLFW_PRESS) moveDir -= rightDir;
        if (glfwGetKey(window, keys.moveUp      ) == GLFW_PRESS) moveDir += upDir;
        if (glfwGetKey(window, keys.moveDown    ) == GLFW_PRESS) moveDir -= upDir;

        if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon())
        {
            gameObjects.transform3d.translation += moveSpeed * dt * glm::normalize(moveDir);
        }
    }
}
