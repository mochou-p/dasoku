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

    void App::loadGameObjects()
    {
        std::vector<AshiModel::Vertex> vertices
        {
            {{  0.0f, -0.5f }, { 1.0f, 1.0f, 0.0f }},
            {{  0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f }},
            {{ -0.5f,  0.5f }, { 0.0f, 1.0f, 1.0f }}
        };

        auto ashiModel = std::make_shared<AshiModel>(ashiDevice, vertices);

        auto triangle = AshiGameObject::createGameObject();
        triangle.model = ashiModel;
        triangle.color = { 1.0f, 0.2f, 0.4f };
        triangle.transform2d.translation.x = 0.2f;
        triangle.transform2d.scale = { 2.0f, 0.5f };
        triangle.transform2d.rotation = 0.25f * glm::two_pi<float>();

        gameObjects.push_back(std::move(triangle));
    }
}
