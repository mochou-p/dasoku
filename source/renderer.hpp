// dsk

#pragma once

#include "window.hpp"
#include "device.hpp"
#include "swap_chain.hpp"
#include "model.hpp"

#include <memory>
#include <vector>
#include <cassert>
#include <array>

namespace dsk
{
    class DskRenderer
    {
        public:
            DskRenderer(DskWindow &dskWindow, DskDevice &dskDevice);
            ~DskRenderer();

            DskRenderer(const DskRenderer &) = delete;
            DskRenderer &operator=(const DskRenderer &) = delete;

            VkRenderPass getSwapChainRenderPass() const { return dskSwapChain->getRenderPass(); }
            bool isFrameInProgres() const { return isFrameStarted; }

            float getAspectRatio() const { return dskSwapChain->extentAspectRatio(); }

            VkCommandBuffer getCurrentCommandBuffer()
            const {
                assert(isFrameStarted && "cannot get command buffer when frame not in progress");

                return commandBuffers[currentFrameIndex];
            }

            int getFrameIndex() const
            {
                assert(isFrameStarted && "cannot get frame index when frame not in progress");
                //Fix
                return currentFrameIndex;
            }

            VkCommandBuffer beginFrame();
            void endFrame();
            void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
            void endSwapChainRenderPass(VkCommandBuffer commandBuffer);


        private:
            void createCommandBuffers();
            void freeCommandBuffers();
            void recreateSwapChain();

            DskWindow &dskWindow;
            DskDevice &dskDevice;
            std::unique_ptr<DskSwapChain> dskSwapChain;
            std::vector<VkCommandBuffer> commandBuffers;

            uint32_t currentImageIndex;
            int currentFrameIndex {0};
            bool isFrameStarted {false};
    };
}
