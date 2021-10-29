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
        AshiModel::Data modelData {};
        modelData.vertices =
            {
                // left face (white)
                {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
                {{-.5f,  .5f,  .5f}, {.9f, .9f, .9f}},
                {{-.5f, -.5f,  .5f}, {.9f, .9f, .9f}},
                {{-.5f,  .5f, -.5f}, {.9f, .9f, .9f}},

                // right face (yellow)
                {{ .5f, -.5f, -.5f}, {.8f, .8f, .1f}},
                {{ .5f,  .5f,  .5f}, {.8f, .8f, .1f}},
                {{ .5f, -.5f,  .5f}, {.8f, .8f, .1f}},
                {{ .5f,  .5f, -.5f}, {.8f, .8f, .1f}},

                // top face (orange, remember y axis points down)
                {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
                {{ .5f, -.5f,  .5f}, {.9f, .6f, .1f}},
                {{-.5f, -.5f,  .5f}, {.9f, .6f, .1f}},
                {{ .5f, -.5f, -.5f}, {.9f, .6f, .1f}},

                // bottom face (red)
                {{-.5f,  .5f, -.5f}, {.8f, .1f, .1f}},
                {{ .5f,  .5f,  .5f}, {.8f, .1f, .1f}},
                {{-.5f,  .5f,  .5f}, {.8f, .1f, .1f}},
                {{ .5f,  .5f, -.5f}, {.8f, .1f, .1f}},

                // nose face (blue)
                {{-.5f, -.5f,  0.5f}, {.1f, .1f, .8f}},
                {{ .5f,  .5f,  0.5f}, {.1f, .1f, .8f}},
                {{-.5f,  .5f,  0.5f}, {.1f, .1f, .8f}},
                {{ .5f, -.5f,  0.5f}, {.1f, .1f, .8f}},

                // tail face (green)
                {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
                {{ .5f,  .5f, -0.5f}, {.1f, .8f, .1f}},
                {{-.5f,  .5f, -0.5f}, {.1f, .8f, .1f}},
                {{ .5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            };

        for (auto& v : modelData.vertices) {
            v.position += offset;
        }

        modelData.indices = {0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
            12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21};

        return std::make_unique<AshiModel>(device, modelData);
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
