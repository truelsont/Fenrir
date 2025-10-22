#pragma once

#include< string>

#include "abstractConfiguration.hh"

namespace fenrir {
    
enum FenrirApplicationServices {
    kStateManager = 0, // do I need to do this? 
    kWorldManager = 1,
}



class StateManagerConfig : public AbstractConfiguration {
public: 
    std::string user_name; 
}


class StateManager : public AbstractService{
public:
    StateManager(StateManagerConfig config)
    {
    }
    
    fenrir::ServiceCodes serializeService() override;
    fenrir::ServiceCodes deserializeService() override;

    std::map<FenrirApplicationServices, std::shared_ptr<AbstractService>> application_services;

private:
    
};
}