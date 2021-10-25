// ashi

#include "app.hpp"
#include "render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>

namespace ashi
{
    App::App() { loadGameObjects(); }

    App::~App() {}

    void App::run()
    {
        AshiRenderSystem ashiRenderSystem
        { ashiDevice, ashiRenderer.getSwapChainRenderPass() };

        while (!ashiWindow.shouldClose())
        {
            glfwPollEvents();
            
            if (auto commandBuffer = ashiRenderer.beginFrame())
            {
                ashiRenderer.beginSwapChainRenderPass(commandBuffer);
                ashiRenderSystem.renderGameObjects(commandBuffer, gameObjects);
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
            {{ -.5f, -.5f, -.5f }, {.9f, .9f, .9f}},
            {{ -.5f,  .5f,  .5f }, {.9f, .9f, .9f}},
            {{ -.5f, -.5f,  .5f }, {.9f, .9f, .9f}},
            {{ -.5f, -.5f, -.5f }, {.9f, .9f, .9f}},
            {{ -.5f,  .5f, -.5f }, {.9f, .9f, .9f}},
            {{ -.5f,  .5f,  .5f }, {.9f, .9f, .9f}},

            // right face (yellow)
            {{  .5f, -.5f, -.5f }, {.8f, .8f, .1f}},
            {{  .5f,  .5f,  .5f }, {.8f, .8f, .1f}},
            {{  .5f, -.5f,  .5f }, {.8f, .8f, .1f}},
            {{  .5f, -.5f, -.5f }, {.8f, .8f, .1f}},
            {{  .5f,  .5f, -.5f }, {.8f, .8f, .1f}},
            {{  .5f,  .5f,  .5f }, {.8f, .8f, .1f}},

            // top face (orange, remember y axis points down)
            {{ -.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{  .5f, -.5f,  .5f}, {.9f, .6f, .1f}},
            {{ -.5f, -.5f,  .5f}, {.9f, .6f, .1f}},
            {{ -.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{  .5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{  .5f, -.5f,  .5f}, {.9f, .6f, .1f}},

            // bottom face (red)
            {{ -.5f,  .5f, -.5f }, {.8f, .1f, .1f}},
            {{  .5f,  .5f,  .5f }, {.8f, .1f, .1f}},
            {{ -.5f,  .5f,  .5f }, {.8f, .1f, .1f}},
            {{ -.5f,  .5f, -.5f }, {.8f, .1f, .1f}},
            {{  .5f,  .5f, -.5f }, {.8f, .1f, .1f}},
            {{  .5f,  .5f,  .5f }, {.8f, .1f, .1f}},

            // nose face (blue)
            {{ -.5f, -.5f,  .5f }, {.1f, .1f, .8f}},
            {{  .5f,  .5f,  .5f }, {.1f, .1f, .8f}},
            {{ -.5f,  .5f,  .5f }, {.1f, .1f, .8f}},
            {{ -.5f, -.5f,  .5f }, {.1f, .1f, .8f}},
            {{  .5f, -.5f,  .5f }, {.1f, .1f, .8f}},
            {{  .5f,  .5f,  .5f }, {.1f, .1f, .8f}},

            // tail face (green)
            {{ -.5f, -.5f, -.5f }, {.1f, .8f, .1f}},
            {{  .5f,  .5f, -.5f }, {.1f, .8f, .1f}},
            {{ -.5f,  .5f, -.5f }, {.1f, .8f, .1f}},
            {{ -.5f, -.5f, -.5f }, {.1f, .8f, .1f}},
            {{  .5f, -.5f, -.5f }, {.1f, .8f, .1f}},
            {{  .5f,  .5f, -.5f }, {.1f, .8f, .1f}},
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
            {.0f, .0f, .0f}
        );

        auto cube = AshiGameObject::createGameObject();
        cube.model = ashiModel;
        cube.transform3d.translation = {.0f, .0f, .5f};
        cube.transform3d.scale = {.5f, .5f, .5f};

        gameObjects.push_back(std::move(cube));
    }
}
