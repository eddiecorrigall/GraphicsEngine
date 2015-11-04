#include "Process.hpp"

#include <iostream>
#include <cassert>

#include <SDL2/SDL.h> // SDL_GetTicks

ProcessManager engine("engine");

bool operator< (const System& a, const System& b) {
	return (typeid(a).before(typeid(b)));
}

bool operator== (const System& a, const System& b) {
	return (typeid(a) == typeid(b));
}

ProcessManager::ProcessManager(const string& name) : System(name) {
}

ProcessManager::~ProcessManager() {
	
	// ##### Systems...
	
	for (SystemMap::iterator itr = systems.begin(); itr != systems.end(); itr++) {
		
		System* system = itr->second;
		delete system;
	}
	
	systems.clear();
	
	// ##### Resources...
	
	for (ResourceList::iterator itr = resources.begin(); itr != resources.end(); itr++) {
		
		Resource* resource = (*itr);
		delete resource;
	}
	
	resources.clear();
}

void ProcessManager::Update(const unsigned int& elapsed_milliseconds) {
	
	// ##### Systems...
	
	/* Note:
	 * A System is a Process which handles resources.
	 * At least one System must use a resource.
	 * When a System updates, the System MUST call Resource::Touch().
	 */
	
	for (SystemMap::iterator itr = systems.begin(); itr != systems.end(); itr++) {
		
		if (IsRunning() == false) break;

		System* system = itr->second;

		if (system->IsRunning() == false) continue;

		system->Update(elapsed_milliseconds);
	}
	
	// ##### Resources...
	
	/* Note:
	 * If a system failed to touch a Resource,
	 * The Resource is deallocated.
	 */
	
	for (ResourceList::iterator itr = resources.begin(); itr != resources.end(); itr++) {
		
		if (IsRunning() == false) break;
		
		Resource* resource = (*itr);
		
		if (resource->IsTouched()) continue;
		
		cerr << "ERROR: Resource " << resource->Name() << " never touched!" << endl;
		
		delete resource;
		resources.erase(itr);
	}
}

void ProcessManager::Start() {
	
	System::Start();

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
	
	if (ContainsSystem(system_type)) {
		cerr << "ERROR: already contains process!" << endl;
		return false;
	}

	systems[system_type] = system;

	return true;
}

System* ProcessManager::FindSystem(const type_info* system_type) {
	
	SystemMap::iterator results = systems.find(system_type);
	
	if (results == systems.end()) return NULL;
	System* system = results->second;
	
	return system;
}

ResourceList* ProcessManager::Resources() {
	return &resources;
}
