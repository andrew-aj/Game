#ifndef VULKAN_VULKAN_H
#define VULKAN_VULKAN_H

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <iostream>
#include <boxer/boxer.h>
#include <glm/glm.hpp>
#include <vector>
#include <array>
#include <cstring>
#include <fstream>
#include <any>

#include "Components.h"

namespace SGE::Vulkan {

    using ProgramID = int64_t;

    using UBOID = int64_t;

    using ShaderID = int64_t;

    std::map<ShaderID, std::pair<VkPipeline, VkPipelineLayout>> shaderMap;
    std::unordered_map<std::string, ShaderID> storedShaders;
    std::unordered_map<ShaderID, VkDescriptorSetLayout> descriptorSetLayouts;
    std::map<UBOID, std::pair<VkBuffer, VkDeviceMemory>> uniformBuffers;
    std::unordered_map<ShaderID, std::vector<UBOID>> uniformBufferMap;
    std::unordered_map<ShaderID, VkDescriptorPool> descriptorPools;
    std::unordered_map<ShaderID, std::vector<VkDescriptorSet>> descriptorSets;

    struct QueueFamilyIndices {
        int graphicsFamily = -1;
        int presentFamily = -1;

        bool isComplete() {
            return graphicsFamily >= 0 && presentFamily >= 0;
        }
    };

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    const std::vector<const char *> validationLayers = {
            "VK_LAYER_LUNARG_standard_validation"
    };

    const std::vector<const char *> deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    std::vector<const char *> getRequiredExtensions();

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                          const VkAllocationCallbacks *pAllocator,
                                          VkDebugUtilsMessengerEXT *pCallback);

    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT callback,
                                       const VkAllocationCallbacks *pAllocator);

    void initVulkan();

    void createInstance();

    void createVertexBuffer(std::vector<Vertex> &vertices, VkBuffer &vertexBuffer, VkDeviceMemory &vertexBufferMemory);

    bool checkValidationLayerSupport();

    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer,
                      VkDeviceMemory &bufferMemory);

    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    VkCommandBuffer beginSingleTimeCommands();

    void endSingleTimeCommands(VkCommandBuffer commandBuffer);

    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    void setupDebugCallback();

    VKAPI_ATTR VkBool32 VKAPI_CALL
    debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT flags, VkDebugUtilsMessageTypeFlagsEXT messageType,
                  const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData);

    void createSurface();

    void pickPhysicalDevice();

    bool isDeviceSuitable(VkPhysicalDevice device);

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

    bool checkDeviceExtensionSupport(VkPhysicalDevice device);

    void createLogicalDevice();

    void createSwapChain();

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);

    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

    void createImageViews();

    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

    void createRenderPass();

    VkFormat findDepthFormat();

    VkFormat
    findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

    VkDescriptorSetLayoutBinding
    createDescriptorBinding(uint32_t binding, VkDescriptorType type, uint32_t count, VkShaderStageFlags flags,
                            VkSampler *sampler = nullptr);

    void createDescriptorSetLayout(std::initializer_list<VkDescriptorSetLayoutBinding> list, ShaderID id);

    ShaderID createShader(const std::string &shader, std::initializer_list<VkDescriptorSetLayoutBinding> list);

    template<size_t n>
    ShaderID createShader(const std::string &shader, VkVertexInputBindingDescription bindingDescription,
                          std::array<VkVertexInputAttributeDescription, n> attributeDescription,
                          std::initializer_list<VkDescriptorSetLayoutBinding> list);

    std::vector<char> readFile(const std::string &filename);

    VkShaderModule createShaderModule(const std::vector<char> &code);

    void createCommandPool();

    void createDepthResources();

    void
    createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format,
                VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage &image,
                VkDeviceMemory &imageMemory);

    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout,
                               uint32_t mipLevels);

    bool hasStencilComponent(VkFormat format);

    void createFramebuffers();

    //Must pass them in the order you have the layouts numbered
    template<typename B>
    void createUniformBuffers(ShaderID id, int layout) {
        VkDeviceSize bufferSize = sizeof(B);
        UBOID ID;
        if (uniformBuffers.empty())
            ID = 0;
        else
            ID = (uniformBuffers.rbegin()->first) + 1;

        createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                     uniformBuffers[ID].first,
                     uniformBuffers[ID].second);

        auto &list = uniformBufferMap[id];
        if (layout == list.size())
            list.push_back(ID);
        else
            uniformBufferMap[id].insert(uniformBufferMap[id].begin() + layout, ID);
    }

    void createDescriptorPool(ShaderID id, std::initializer_list<VkDescriptorType> types);

//    void createTextureSampler();

    GLFWwindow *window;

    VkInstance instance;
    VkDebugUtilsMessengerEXT callback;
    VkSurfaceKHR surface;

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
    VkDevice device;

    VkQueue graphicsQueue;
    VkQueue presentQueue;

    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;

    VkRenderPass renderPass;
//    VkDescriptorSetLayout descriptorSetLayout;

    VkCommandPool commandPool;

    //VkImage colorImage;
    //VkDeviceMemory colorImageMemory;
    //VkImageView colorImageView;

    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;

    VkDescriptorPool descriptorPool;
//    std::vector<VkDescriptorSet> descriptorSets;

    std::vector<VkCommandBuffer> commandBuffers;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;


}


#endif //VULKAN_VULKAN_H
