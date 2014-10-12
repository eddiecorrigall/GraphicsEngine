#ifndef __INPUT_HPP__
#define	__INPUT_HPP__

#include "Process.hpp"

#include <string>

using namespace std;

enum Key {
    KEY_UP = 0,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_MAX
};

class Input : public Process {
    
    private:
        
        bool key_down[KEY_MAX];
    
    protected:
        
        void Update(const unsigned int& elapsed_milliseconds);
        
    public:
        
        Input(const string& name);
        ~Input();
        
        bool IsKeyDown(Key key) { return key_down[key]; }
};

#endif
