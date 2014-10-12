#ifndef __PROCESS_HPP__
#define __PROCESS_HPP__

#include <typeinfo>
#include <ostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

class Process;
class Resource;
class ProcessManager;

extern ProcessManager engine;
extern bool operator< (const Process& p, const Process& q);
extern bool operator== (const Process& p, const Process& q);

typedef Process System;
typedef map<const type_info*, System*> SystemMap; // type => System
typedef vector<Resource*> ResourceList;

#include <iostream>
using namespace std;

class Process {
    
    friend class ProcessManager;

    private:
    
        const string name;
        bool running;
        
        Process() {}
        
    protected:
        
        virtual void Update(const unsigned int& elapsed_milliseconds) = 0;
        
    public:
        
        Process(const string& name) : name(name), running(true) {
            cout << "Process " << Name() << " running " << IsRunning() << endl;
        }
        
        string Name() { return name; }
        
        virtual void Start() { running = true; }
        virtual void Stop() { running = false; }
        
        virtual bool IsRunning() { return running; }
        
        friend bool operator< (const Process& p, const Process& q);
        friend bool operator== (const Process& p, const Process& q);
};

class Resource : public Process {
    
    private:
        
        bool touched; // Touched/used by a system
        
    public:
        
        Resource(const string& name) : Process(name), touched(false) {
            cout << "Resource " << Name() << " running " << IsRunning() << endl;
        }
        
        /* IMPORTANT:
         * If a system uses this resource,
         * that system MUST call Resource::Touch
         */
        
        void Touch() { touched = true; }
        bool IsTouched() { return touched; }
};

class ProcessManager : public Process {
    
    private:
        
        SystemMap systems; // permanent
        ResourceList resources; // allocated / deallocated
        
        ProcessManager() {}
        
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
