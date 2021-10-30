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
            camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 20.0f);

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

    void App::loadGameObjects()
    {
        std::shared_ptr<AshiModel> ashiModel;

        // ashiModel =
        //     AshiModel::createModelFromFile(ashiDevice, ".\\models\\smooth_vase.obj");
        // auto smoothVase = AshiGameObject::createGameObject();
        // smoothVase.model = ashiModel;
        // smoothVase.transform3d.translation = {-0.5f, 0.5f, 2.5f};
        // smoothVase.transform3d.scale = glm::vec3(3.0f);
        // gameObjects.push_back(std::move(smoothVase));

        ashiModel =
            AshiModel::createModelFromFile(ashiDevice, ".\\models\\cottage.obj");
        auto cottage = AshiGameObject::createGameObject();
        cottage.model = ashiModel;
        cottage.transform3d.translation = {-0.5f, 0.5f, 2.5f};
        cottage.transform3d.scale = glm::vec3(0.2f, -0.2f, 0.2f);
        gameObjects.push_back(std::move(cottage));

        ashiModel =
            AshiModel::createModelFromFile(ashiDevice, ".\\models\\colored_cube.obj");
        auto coloredCube = AshiGameObject::createGameObject();
        coloredCube.model = ashiModel;
        coloredCube.transform3d.translation = {0.5f, 0.5f, 2.5f};
        coloredCube.transform3d.scale = glm::vec3(5.0f);
        gameObjects.push_back(std::move(coloredCube));
    }
}
