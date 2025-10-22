#pragma once

/*
This file is for defining system wide constants that are being used
through the codebase
*/

namespace fenrir{
    enum ResourceCodes : uint32_t{
        kRESOURCE_CLEANUP_SUCCESS = 0,
        kFAILED_RESOURCE_CLEANUP = 1
    };

    enum ServiceCodes : uint32_t{
        kSERVICE_SERIALIZE_SUCCESS = 0,
        kFAILED_SERVICE_SERIALIZE = 1
    };
}