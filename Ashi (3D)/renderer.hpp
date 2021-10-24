// ashi

#pragma once

#include "window.hpp"
#include "device.hpp"
#include "swap_chain.hpp"
#include "model.hpp"

#include <memory>
#include <vector>
#include <cassert>

namespace ashi
{
    class AshiRenderer
    {
        public:
            AshiRenderer(AshiWindow &ashiWindow, AshiDevice &ashiDevice);
            ~AshiRenderer();

            AshiRenderer(const AshiRenderer &) = delete;
            AshiRenderer &operator=(const AshiRenderer &) = delete;

            VkRenderPass getSwapChainRenderPass() const { return ashiSwapChain->getRenderPass(); }
            bool isFrameInProgres() const { return isFrameStarted; }

            VkCommandBuffer getCurrentCommandBuffer()
            const {
                assert(isFrameStarted && "cannot get command buffer when frame not in progress");

                return commandBuffers[currentFrameIndex];
            }

            int getFrameIndex() const
            {
                assert(isFrameStarted && "cannot get frame index when frame not in progress");
            }

            VkCommandBuffer beginFrame();
            void endFrame();
            void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
            void endSwapChainRenderPass(VkCommandBuffer commandBuffer);


        private:
            void createCommandBuffers();
            void freeCommandBuffers();
            void recreateSwapChain();

            AshiWindow &ashiWindow;
            AshiDevice &ashiDevice;
            std::unique_ptr<AshiSwapChain> ashiSwapChain;
            std::vector<VkCommandBuffer> commandBuffers;

            uint32_t currentImageIndex;
            int currentFrameIndex {0};
            bool isFrameStarted {false};
    };
}
