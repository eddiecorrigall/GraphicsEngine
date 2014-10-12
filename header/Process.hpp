#ifndef __PROCESS_HPP__
#define __PROCESS_HPP__

#include <typeinfo>
#include <ostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

class Process;
class ProcessManager;

class System;
class Resource;

extern ProcessManager engine;
extern bool operator< (const System& a, const System& b);
extern bool operator== (const System& a, const System& b);

typedef map<const type_info*, System*> SystemMap;
typedef vector<Resource*> ResourceList;

#include <iostream>
using namespace std;

class Process {
    
    friend class ProcessManager;
    
    private:
        
        const string name;
        
        //Process() {}
        
    protected:
        
        virtual void Update(const unsigned int& elapsed_milliseconds) = 0;
        
    public:
        
        Process(const string& name) : name(name) {}
        
        string Name() { return name; }
};

class System : public Process {
    
    private:
    
        bool running;
        
        //System() {}
        
    public:
        
        System(const string& name) : Process(name), running(true) {}
        
        virtual void Start() { running = true; }
        virtual void Stop() { running = false; }
        
        virtual bool IsRunning() { return running; }
        
        friend bool operator< (const System& a, const System& b);
        friend bool operator== (const System& a, const System& b);
};

class Resource : public Process {
    
    friend class System;
    
    private:
        
        bool touched; // Touched/used by a system
        
        //Resource() {}
        
    protected:
        
        void Touch() { touched = true; }
        
    public:
        
        Resource(const string& name) : Process(name), touched(false) {}
        
        /* IMPORTANT:
         * If a System uses this Resource,
         * that System MUST call Resource::Touch!
         * Otherwise the resource will be freed.
         */
        
        bool IsTouched() { return touched; }
};

class ProcessManager : public System {
    
    private:
        
        SystemMap systems; // permanent
        ResourceList resources; // allocated / deallocated
        
        //ProcessManager() {}
        
    protected:
        
        void Update(const unsigned int& elapsed_milliseconds);

    public:
        
        ProcessManager(const string& name);
        ~ProcessManager();
        
        void Start();
        
        bool ContainsSystem(const type_info* system_type);
        bool InsertSystem(const type_info*, System* system);
        System* FindSystem(const type_info* system_type);
        
        ResourceList* Resources();
};

template<typename S>
S* SystemInstance() {
    
    const type_info* system_type = &typeid(S);
    System* system = engine.FindSystem(system_type);
    
    if (system == NULL) {
        system = new S(system_type->name());
        engine.InsertSystem(system_type, system);
    }
    
    return static_cast<S*>(system);
}

#endif
