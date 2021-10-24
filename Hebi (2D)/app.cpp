// hebi

#include "app.hpp"
#include "render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>

namespace hebi
{
    App::App() { loadGameObjects(); }

    App::~App() {}

    void App::run()
    {
        HebiRenderSystem hebiRenderSystem
        { hebiDevice, hebiRenderer.getSwapChainRenderPass() };

        while (!hebiWindow.shouldClose())
        {
            glfwPollEvents();
            
            if (auto commandBuffer = hebiRenderer.beginFrame())
            {
                hebiRenderer.beginSwapChainRenderPass(commandBuffer);
                hebiRenderSystem.renderGameObjects(commandBuffer, gameObjects);
                hebiRenderer.endSwapChainRenderPass(commandBuffer);
                hebiRenderer.endFrame();
            }
        }

        vkDeviceWaitIdle(hebiDevice.device());
    }

    void App::loadGameObjects()
    {
        std::vector<HebiModel::Vertex> vertices
        {
            {{  0.0f, -0.5f }, { 1.0f, 1.0f, 0.0f }},
            {{  0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f }},
            {{ -0.5f,  0.5f }, { 0.0f, 1.0f, 1.0f }}
        };

        auto hebiModel = std::make_shared<HebiModel>(hebiDevice, vertices);

        auto triangle = HebiGameObject::createGameObject();
        triangle.model = hebiModel;
        triangle.color = { 1.0f, 0.2f, 0.4f };
        triangle.transform2d.translation.x = 0.2f;
        triangle.transform2d.scale = { 2.0f, 0.5f };
        triangle.transform2d.rotation = 0.25f * glm::two_pi<float>();

        gameObjects.push_back(std::move(triangle));
    }
}
