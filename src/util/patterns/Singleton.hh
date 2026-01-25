template <typename SingletonType>
class Singleton {
public: 
    static T& Get(){
        static SingletonType instance; 
        return instance; 
    }


    Singleton(const Singleton&) = delete; 
    Singleton(Singleton&&) = delete; 

protected: 
    Singleton() = default; 
    ~Singleton() = default; 
}; 

// todo ; this should probably pull in an interface for the settings and type
// and should probably make the RuntimeSingleton for the args as well 
template <typename RuntimeSingletonType, typename RuntimeSingletonParameters>
class RuntimeSingleton {
public: 
    static T& Get(RuntimeSingletonParameters&& args){
       std::call_once(initFlag_, [&] {
            instance_.reset(new RuntimeSingletonType(std::move(args))); 
       }); 
    }
protected: 
    static std::unique_ptr<RuntimeSingleton> instance_; 
    static std::once_flag initFlag_; 
}; 
