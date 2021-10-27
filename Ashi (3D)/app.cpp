// ashi

#include "app.hpp"
#include "render_system.hpp"
#include "camera.hpp"
#include "keyboard_movement_controller.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <chrono>

namespace ashi
{
    App::App() { loadGameObjects(); }

    App::~App() {}

    void App::run()
    {
        AshiRenderSystem ashiRenderSystem
        { ashiDevice, ashiRenderer.getSwapChainRenderPass() };

        AshiCamera camera {};
        camera.setViewTarget
        (
            glm::vec3(-1.0f, -2.0f, 2.0f),
            glm::vec3(0.0f, 0.0f, 2.5f)
        );

        auto viewerObject = AshiGameObject::createGameObject();
        AshiKeyboardMovementController cameraController {};

        auto currentTime = std::chrono::high_resolution_clock::now();

        while (!ashiWindow.shouldClose())
        {
            glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration
                <float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            // fixes weird behavior when resizing and using the keyboard at the same time
            // frameTime = glm::min(frameTime, MAX_FRAME_TIME);

            cameraController.moveInPlaneXZ
            (
                ashiWindow.getGLFWwindow(),
                frameTime,
                viewerObject
            );
            camera.setViewYXZ
            (
                viewerObject.transform3d.translation,
                viewerObject.transform3d.rotation
            );

            float aspect = ashiRenderer.getAspectRatio();
            camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10.0f);

            if (auto commandBuffer = ashiRenderer.beginFrame())
            {
                ashiRenderer.beginSwapChainRenderPass(commandBuffer);
                ashiRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
                ashiRenderer.endSwapChainRenderPass(commandBuffer);
                ashiRenderer.endFrame();
            }
        }

        vkDeviceWaitIdle(ashiDevice.device());
    }

    std::unique_ptr<AshiModel> createCubeModel(AshiDevice& device, glm::vec3 offset)
    {
        std::vector<AshiModel::Vertex> vertices
        {
            // left face (white)
            {{ -0.5f, -0.5f, -0.5f }, {0.9f, 0.9f, 0.9f}},
            {{ -0.5f,  0.5f,  0.5f }, {0.9f, 0.9f, 0.9f}},
            {{ -0.5f, -0.5f,  0.5f }, {0.9f, 0.9f, 0.9f}},
            {{ -0.5f, -0.5f, -0.5f }, {0.9f, 0.9f, 0.9f}},
            {{ -0.5f,  0.5f, -0.5f }, {0.9f, 0.9f, 0.9f}},
            {{ -0.5f,  0.5f,  0.5f }, {0.9f, 0.9f, 0.9f}},

            // right face (yellow)
            {{  0.5f, -0.5f, -0.5f }, {0.8f, 0.8f, 0.1f}},
            {{  0.5f,  0.5f,  0.5f }, {0.8f, 0.8f, 0.1f}},
            {{  0.5f, -0.5f,  0.5f }, {0.8f, 0.8f, 0.1f}},
            {{  0.5f, -0.5f, -0.5f }, {0.8f, 0.8f, 0.1f}},
            {{  0.5f,  0.5f, -0.5f }, {0.8f, 0.8f, 0.1f}},
            {{  0.5f,  0.5f,  0.5f }, {0.8f, 0.8f, 0.1f}},

            // top face (orange, remember y axis points down)
            {{ -0.5f, -0.5f, -0.5f }, {0.9f, 0.6f, 0.1f}},
            {{  0.5f, -0.5f,  0.5f }, {0.9f, 0.6f, 0.1f}},
            {{ -0.5f, -0.5f,  0.5f }, {0.9f, 0.6f, 0.1f}},
            {{ -0.5f, -0.5f, -0.5f }, {0.9f, 0.6f, 0.1f}},
            {{  0.5f, -0.5f, -0.5f }, {0.9f, 0.6f, 0.1f}},
            {{  0.5f, -0.5f,  0.5f }, {0.9f, 0.6f, 0.1f}},

            // bottom face (red)
            {{ -0.5f,  0.5f, -0.5f }, {0.8f, 0.1f, 0.1f}},
            {{  0.5f,  0.5f,  0.5f }, {0.8f, 0.1f, 0.1f}},
            {{ -0.5f,  0.5f,  0.5f }, {0.8f, 0.1f, 0.1f}},
            {{ -0.5f,  0.5f, -0.5f }, {0.8f, 0.1f, 0.1f}},
            {{  0.5f,  0.5f, -0.5f }, {0.8f, 0.1f, 0.1f}},
            {{  0.5f,  0.5f,  0.5f }, {0.8f, 0.1f, 0.1f}},

            // nose face (blue)
            {{ -0.5f, -0.5f,  0.5f }, {0.1f, 0.1f, 0.8f}},
            {{  0.5f,  0.5f,  0.5f }, {0.1f, 0.1f, 0.8f}},
            {{ -0.5f,  0.5f,  0.5f }, {0.1f, 0.1f, 0.8f}},
            {{ -0.5f, -0.5f,  0.5f }, {0.1f, 0.1f, 0.8f}},
            {{  0.5f, -0.5f,  0.5f }, {0.1f, 0.1f, 0.8f}},
            {{  0.5f,  0.5f,  0.5f }, {0.1f, 0.1f, 0.8f}},

            // tail face (green)
            {{ -0.5f, -0.5f, -0.5f }, {0.1f, 0.8f, 0.1f}},
            {{  0.5f,  0.5f, -0.5f }, {0.1f, 0.8f, 0.1f}},
            {{ -0.5f,  0.5f, -0.5f }, {0.1f, 0.8f, 0.1f}},
            {{ -0.5f, -0.5f, -0.5f }, {0.1f, 0.8f, 0.1f}},
            {{  0.5f, -0.5f, -0.5f }, {0.1f, 0.8f, 0.1f}},
            {{  0.5f,  0.5f, -0.5f }, {0.1f, 0.8f, 0.1f}},
        };

        for (auto& v : vertices) {
            v.position += offset;
        }

        return std::make_unique<AshiModel>(device, vertices);
    }

    void App::loadGameObjects()
    {
        std::shared_ptr<AshiModel> ashiModel = createCubeModel
        (
            ashiDevice,
            {0.0f, 0.0f, 0.0f}
        );

        auto cube = AshiGameObject::createGameObject();
        cube.model = ashiModel;
        cube.transform3d.translation = {0.0f, 0.0f, 2.5f};
        cube.transform3d.scale = {0.5f, 0.5f, 0.5f};

        gameObjects.push_back(std::move(cube));
    }
}
