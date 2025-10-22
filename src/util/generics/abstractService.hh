#pragma once

#include "constants.hh"

namespace fenrir {

class AbstractService {
public:
    virtual ~AbstractService() = default;

    virtual ServiceCodes serializeService() = 0;
    virtual ServiceCodes deserializeService() = 0;
};

}  // namespace fenrir