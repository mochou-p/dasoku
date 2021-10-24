// hebi

#pragma once

#include "window.hpp"
#include "device.hpp"
#include "swap_chain.hpp"
#include "model.hpp"

#include <memory>
#include <vector>
#include <cassert>

namespace hebi
{
    class HebiRenderer
    {
        public:
            HebiRenderer(HebiWindow &hebiWindow, HebiDevice &hebiDevice);
            ~HebiRenderer();

            HebiRenderer(const HebiRenderer &) = delete;
            HebiRenderer &operator=(const HebiRenderer &) = delete;

            VkRenderPass getSwapChainRenderPass() const { return hebiSwapChain->getRenderPass(); }
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

            HebiWindow &hebiWindow;
            HebiDevice &hebiDevice;
            std::unique_ptr<HebiSwapChain> hebiSwapChain;
            std::vector<VkCommandBuffer> commandBuffers;

            uint32_t currentImageIndex;
            int currentFrameIndex {0};
            bool isFrameStarted {false};
    };
}