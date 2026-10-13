#pragma once
#include "Data/Common.h"

class DIServiceContainer 
{
public:

    template<typename TInterface, typename TImplementation>
    void registerService() 
    {
        auto key = std::type_index(typeid(TInterface));

        _singletons[key] = []() -> std::shared_ptr<void> 
        {
            return std::make_shared<TImplementation>();
        };
    }
    template<typename TInterface>
    void registerInstance(std::shared_ptr<TInterface> instance) {
        auto key = std::type_index(typeid(TInterface));
        _singletons[key] = [inst = std::move(instance)]() -> std::shared_ptr<void> {
            return inst;
            };
    }

    template<typename T>
    std::shared_ptr<T> resolve() const 
    {
        auto key = std::type_index(typeid(T));
        auto it = _singletons.find(key);
        if (it == _singletons.end())
        {
            throw std::runtime_error("Service not registered: " + std::string(typeid(T).name()));
        }

        return std::static_pointer_cast<T>(it->second());
    }

private:
    std::unordered_map<std::type_index, std::function<std::shared_ptr<void>()>> _singletons;
};

