#include "Process.hpp"

#include <iostream>
#include <cassert>

#include <SDL2/SDL.h> // SDL_GetTicks

ProcessManager engine("engine");

bool operator< (const Process& p, const Process& q) {
    return typeid(p).before(typeid(q));
}

bool operator== (const Process& p, const Process& q) {
    return (typeid(p) == typeid(q));
}

ProcessManager::ProcessManager(const string& name) : Process(name) {
}

ProcessManager::~ProcessManager() {
    
    // ##### Systems...
    
    for (SystemMap::iterator itr = systems.begin(); itr != systems.end(); itr++) {
        
        System* system = itr->second;
        
        system->Stop();
        delete system;
    }
    
    systems.clear();
    
    // ##### Resources...
    
    for (ResourceList::iterator itr = resources.begin(); itr != resources.end(); itr++) {
        
        Resource* resource = (*itr);
        
        resource->Stop();
        delete resource;
    }
    
    resources.clear();
}

void ProcessManager::Update(const unsigned int& elapsed_milliseconds) {

    // ##### Systems...
    
    /* Note:
     * A system is a process which uses handles resources.
     * At least one system must use a resource.
     * When a system updates, the system MUST call Resource::Touch().
     */
    
    if (IsRunning() == false) return;
    
    for (SystemMap::iterator itr = systems.begin(); itr != systems.end(); itr++) {
        
        if (IsRunning() == false) continue;

        System* system = itr->second;

        if (system->IsRunning() == false) continue;

        system->Update(elapsed_milliseconds);
    }
    
    // ##### Resources...
    
    /* Note:
     * If a system failed to touch a resource,
     * Or the resource is no longer running,
     * Then deallocate the resource.
     */
    
    if (IsRunning() == false) return;
    
    for (ResourceList::iterator itr = resources.begin(); itr != resources.end(); itr++) {
        
        Resource* resource = (*itr);
        
        /*if (resource->IsTouched() == false || resource->IsRunning() == false) {
            delete resource;
            resources.erase(itr);
        }
        
        if (resource->IsTouched() == false) {
            cerr << "ERROR: Resource " << resource->Name() << " never touched!" << endl;
        }*/
    }
}

void ProcessManager::Start() {
    
    cout << "ProcessManager::ProcessManager" << endl;
    
    Process::Start();

    unsigned int last_milliseconds = SDL_GetTicks();

    while (IsRunning()) {

        unsigned int milliseconds = SDL_GetTicks();
        unsigned int elapsed_milliseconds = milliseconds - last_milliseconds;
        last_milliseconds = milliseconds;
        
        Update(elapsed_milliseconds);
    }
}

bool ProcessManager::ContainsSystem(const type_info* system_type) {
    return (systems.find(system_type) != systems.end());
}

bool ProcessManager::InsertSystem(const type_info* system_type, System* system) {
    
    cout
        << "ProcessManager::InsertProcess"
        << ' '
        << system->Name()
        << endl;
    
    if (ContainsSystem(system_type)) {
        cerr << "ERROR: already contains process!" << endl;
        return false;
    }

    systems[system_type] = system;

    return true;
}

Process* ProcessManager::FindSystem(const type_info* system_type) {
    
    SystemMap::iterator results = systems.find(system_type);
    
    if (results == systems.end()) return NULL;
    System* system = results->second;
    
    return system;
}

ResourceList* ProcessManager::Resources() {
    return &resources;
}
