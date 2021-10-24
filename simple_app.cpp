// zzz

#include "simple_app.hpp"
#include "simple_render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>

namespace zzz 
{
    SimpleApp::SimpleApp() { loadGameObjects(); }

    SimpleApp::~SimpleApp() {}

    void SimpleApp::run()
    {
        SimpleRenderSystem simpleRenderSystem
        { zzzDevice, zzzRenderer.getSwapChainRenderPass() };

        while (!zzzWindow.shouldClose())
        {
            glfwPollEvents();
            
            if (auto commandBuffer = zzzRenderer.beginFrame())
            {
                zzzRenderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects);
                zzzRenderer.endSwapChainRenderPass(commandBuffer);
                zzzRenderer.endFrame();
            }
        }

        vkDeviceWaitIdle(zzzDevice.device());
    }

    void SimpleApp::loadGameObjects()
    {
        std::vector<ZzzModel::Vertex> vertices
        {
            {{  0.0f, -0.5f }, { 1.0f, 1.0f, 0.0f }},
            {{  0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f }},
            {{ -0.5f,  0.5f }, { 0.0f, 1.0f, 1.0f }}
        };

        auto zzzModel = std::make_shared<ZzzModel>(zzzDevice, vertices);

        auto triangle = ZzzGameObject::createGameObject();
        triangle.model = zzzModel;
        triangle.color = { 1.0f, 0.2f, 0.4f };
        triangle.transform2d.translation.x = 0.2f;
        triangle.transform2d.scale = { 2.0f, 0.5f };
        triangle.transform2d.rotation = 0.25f * glm::two_pi<float>();

        gameObjects.push_back(std::move(triangle));
    }
}
