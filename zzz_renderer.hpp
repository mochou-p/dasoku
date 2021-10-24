// zzz

#pragma once

#include "zzz_window.hpp"
#include "zzz_device.hpp"
#include "zzz_swap_chain.hpp"
#include "zzz_model.hpp"

#include <memory>
#include <vector>
#include <cassert>

namespace zzz
{
    class ZzzRenderer
    {
        public:
            ZzzRenderer(ZzzWindow &zzzWindow, ZzzDevice &zzzDevice);
            ~ZzzRenderer();

            ZzzRenderer(const ZzzRenderer &) = delete;
            ZzzRenderer &operator=(const ZzzRenderer &) = delete;

            VkRenderPass getSwapChainRenderPass() const { return zzzSwapChain->getRenderPass(); }
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

            ZzzWindow &zzzWindow;
            ZzzDevice &zzzDevice;
            std::unique_ptr<ZzzSwapChain> zzzSwapChain;
            std::vector<VkCommandBuffer> commandBuffers;

            uint32_t currentImageIndex;
            int currentFrameIndex {0};
            bool isFrameStarted {false};
    };
}
