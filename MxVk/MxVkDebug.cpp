#include "MxVkDebug.h"

namespace Mix {
    namespace Graphics {
        const vk::DebugUtilsMessengerEXT& Debug::setDebugCallback(const vk::DebugUtilsMessageSeverityFlagsEXT & messageSeverity, const vk::DebugUtilsMessageTypeFlagsEXT & messageType, PFN_vkDebugUtilsMessengerCallbackEXT callback, void * userData) {
            vk::DebugUtilsMessengerCreateInfoEXT createInfo;
            createInfo.messageSeverity = messageSeverity;
            createInfo.messageType = messageType;
            createInfo.pfnUserCallback = callback;

            vk::DebugUtilsMessengerEXT messenger = mCore->GetInstance().createDebugUtilsMessengerEXT(createInfo,
                                                                                                  nullptr,
                                                                                                  mCore->DynamicLoader());

            mMessengers.push_back(std::move(messenger));
            return mMessengers.back();
        }

        void Debug::destroyDebugCallback(const vk::DebugUtilsMessengerEXT & messenger) {
            mCore->GetInstance().destroyDebugUtilsMessengerEXT(messenger, nullptr, mCore->DynamicLoader());
            mMessengers.erase(std::find(mMessengers.begin(),
                              mMessengers.end(),
                              messenger));
        }

        void Debug::destroy() {
            if (!mCore)
                return;

            for (const auto& messenger : mMessengers)
                mCore->GetInstance().destroyDebugUtilsMessengerEXT(messenger, nullptr, mCore->DynamicLoader());

            mMessengers.clear();
            mCore = nullptr;
        }

        std::string Debug::severityToString(const vk::DebugUtilsMessageSeverityFlagBitsEXT severity) {
            switch (severity) {
            case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose:
                return "Verbose";
                break;
            case vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo:
                return "Info";
                break;
            case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
                return "Warning";
                break;
            case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
                return "Error";
                break;
            default:
                return "Unknown";
                break;
            }
        }

        std::string Debug::typeToString(const vk::DebugUtilsMessageTypeFlagBitsEXT type) {
            switch (type) {
            case vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral:
                return "General";
                break;
            case vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation:
                return "Validation";
                break;
            case vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance:
                return "Performance";
                break;
            default:
                return "Unkown";
                break;
            }
        }

        VKAPI_ATTR VkBool32 VKAPI_CALL Debug::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT * pCallbackData, void * pUserData) {
            std::string msg = "[ Validation layer ] : [ ";
            msg += severityToString(static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(messageSeverity));
            msg += " ] [ ";
            msg += typeToString(static_cast<vk::DebugUtilsMessageTypeFlagBitsEXT>(messageType));
            msg = msg + " ]\n\t" + pCallbackData->pMessage;
            std::cerr << std::endl << msg << std::endl;
            return VK_FALSE;
        }

    }
}
