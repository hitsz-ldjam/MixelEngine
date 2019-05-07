#pragma once
#ifndef _MX_VK_BUFFER_H_
#define _MX_VK_BUFFER_H_

#include <memory>
#include "MxVkCore.h"
#include "MxVkCommand.h"
#include "MxVkAllocator.h"

// #define BUFFER_MANAGER_ENABLE

namespace Mix {
    namespace Graphics {
        class Buffer :GraphicsComponent {
        public:
            vk::Buffer buffer;
            vk::DeviceSize size;
            vk::DeviceSize alignment;
            vk::DescriptorBufferInfo descriptor;
            MemoryBlock memory;

            vk::BufferUsageFlags usages;
            vk::MemoryPropertyFlags memoryProperty;

            void init(std::shared_ptr<Core> core, std::shared_ptr<DeviceAllocator> allocator);
            virtual ~Buffer() { destory(); }

            void setupDescriptor(const vk::DeviceSize size = VK_WHOLE_SIZE, const vk::DeviceSize offset = 0);
            void copyTo(const void * data, const vk::DeviceSize size);
            void flush(const vk::DeviceSize size = VK_WHOLE_SIZE, const vk::DeviceSize offset = 0);
            void invalidate(const vk::DeviceSize size = VK_WHOLE_SIZE, const vk::DeviceSize offset = 0);
            void destory();

            static Buffer* createBuffer(std::shared_ptr<Core> core,
                                        std::shared_ptr<DeviceAllocator> allocator,
                                        const vk::BufferUsageFlags usage,
                                        const vk::MemoryPropertyFlags memoryProperty,
                                        const vk::DeviceSize size,
                                        const void * data = nullptr,
                                        const vk::CommandBuffer& cmd = vk::CommandBuffer(),
                                        const vk::SharingMode sharingMode = vk::SharingMode::eExclusive);

            static void copyToDeviceBuffer(std::shared_ptr<Core> core,
                                           std::shared_ptr<DeviceAllocator> allocator,
                                           const vk::CommandBuffer& cmd,
                                           const Buffer* dstBuffer,
                                           const void* data);

            static void copyToDeviceBuffer(std::shared_ptr<Core> core,
                                           std::shared_ptr<DeviceAllocator> allocator,
                                           const vk::CommandBuffer& cmd,
                                           const std::unique_ptr<Buffer>& dstBuffer,
                                           const void* data);
        private:
            std::shared_ptr<DeviceAllocator> mAllocator;
        };

#ifdef BUFFER_MANAGER_ENABLE

        // buffer manager
        // BufferManager will create a large buffer for one chunk and sub-allocate from it
        namespace Test {
            struct BufferBlock {
                vk::Buffer buffer = nullptr;
                vk::DeviceSize offset = 0;
                vk::DeviceSize size = 0;
                bool free = false;
                MemoryBlock memory;
            };

            struct Buffer {
                BufferBlock bufferBlock;
                vk::DeviceSize alignment;

                vk::BufferUsageFlags usages;
                vk::MemoryPropertyFlags memoryProperty;

                Buffer(const BufferBlock& block, const vk::DeviceSize& alignment, const vk::BufferUsageFlags& usages, const vk::MemoryPropertyFlags& memProperty) :
                    bufferBlock(block), alignment(alignment), usages(usages), memoryProperty(memProperty) {
                }

                void copyTo(const void * data, const vk::DeviceSize size);
            };

            class BufferChunk {
            public:
                BufferChunk(std::shared_ptr<Core> core,
                            std::shared_ptr<DeviceAllocator> allocator,
                            const vk::BufferUsageFlags usage,
                            const vk::MemoryPropertyFlags memoryProperty,
                            const vk::DeviceSize size,
                            const vk::SharingMode sharingMode = vk::SharingMode::eExclusive);

                BufferChunk(const Chunk& chunk) = delete;
                BufferChunk(BufferChunk&& chunk) {
                    *this = std::move(chunk);
                }

                BufferChunk& operator=(const Chunk& chunk) = delete;
                BufferChunk& operator=(BufferChunk&& chunk);

                ~BufferChunk();

                bool createBuffer(vk::DeviceSize size, BufferBlock& block);
                void destroyBuffer(const BufferBlock& block);

                bool isIn(const BufferBlock& buffer) const {
                    return mBuffer == buffer.buffer;
                }

            private:
                std::shared_ptr<Core> mCore;
                std::shared_ptr<DeviceAllocator> mAllocator;
                vk::Buffer mBuffer;
                vk::DeviceSize mSize;
                vk::MemoryRequirements mMemReq;
                vk::BufferUsageFlags mBufferUsage;

                MemoryBlock mMemBlock;
                vk::MemoryPropertyFlags mMemProperty;

                std::vector<BufferBlock> mBufferBlocks;

                // todo 
                uint32_t mOperationCount = 0;
                static uint32_t MaxOperationCount;

                void sortOut() {};
            };

            class BufferChunkFactory :public GraphicsComponent {
            public:
                void setMinChunkSize(vk::DeviceSize size) {
                    assert(Utils::isPowerOf2(size));
                    mMinChunkSize = size;
                }

                std::unique_ptr<BufferChunk> getChunk(const vk::BufferUsageFlags& usage,
                                                      const vk::MemoryPropertyFlags& memoryProperty,
                                                      vk::DeviceSize size,
                                                      const vk::SharingMode sharingMode = vk::SharingMode::eExclusive);

            private:
                std::shared_ptr<DeviceAllocator> mAllocator;
                vk::DeviceSize mMinChunkSize = 8 * 1024 * 1024;
            };

            class AbstractBufferManager :public GraphicsComponent {
            public:
                AbstractBufferManager() = default;
                AbstractBufferManager(const AbstractBufferManager&) = delete;
                AbstractBufferManager(AbstractBufferManager&& manager) {
                    *this = std::move(manager);
                }
                virtual ~AbstractBufferManager() = 0 {};

                AbstractBufferManager& operator=(const AbstractBufferManager&) = delete;
                AbstractBufferManager& operator=(AbstractBufferManager&& manager) {
                    mCore = manager.mCore;
                    mChunks = std::move(manager.mChunks);
                    return *this;
                }

            private:
                BufferChunkFactory mChunkFactory;
                std::vector<BufferChunk> mChunks;
                std::shared_ptr<DeviceAllocator> mAllocator;
            };

            
            class UniformBufferManager :public AbstractBufferManager {
                
            };

        }

#endif // BUFFER_MANAGER_ENABLE
    }
}
#endif // !_MX_VK_BUFFER_H_