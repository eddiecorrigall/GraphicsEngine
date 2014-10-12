#ifndef __ANIMATIONMODEL_HPP__
#define	__ANIMATIONMODEL_HPP__

#include <string>
#include <fstream>

#include <cassert>

using namespace std;

class Animation;

enum ActionType {
    INVALID, IDLE, RUN, JUMP, ATTACK, WAVE, DEAD
};

struct ActionInfo {
    
    ActionType type;
    
    bool loop;
    int frameIndexOffset;
    int numberOfFrames;
};

typedef map<ActionType, ActionInfo> ActionMap;

class AnimationInfo {
    
    friend class AnimationModel;
    
    private:
        
        string model_path;
        string texture_path;
        
        ActionMap actions;
        
        AnimationInfo() {}
        
    public:
        
        AnimationInfo(const string& path) {
            
            cout << "Load ACT file..." << endl;

            cout << "\tPath " << path << endl;

            ifstream file(path.c_str(), fstream::in);

            if (file.good() == false) {
                cerr << "ERROR: File failed to load!" << endl;
                assert(false);
            }
            
            getline(file, model_path);
            getline(file, texture_path);
            
            ActionInfo action_info;
            
            while(true) {
                
                string action_type_string;
                
                if ((file >> action_type_string) == NULL) break;
                if ((file >> action_info.loop) == NULL) break;
                if ((file >> action_info.frameIndexOffset) == NULL) break;
                if ((file >> action_info.numberOfFrames) == NULL) break;
                
                action_info.type = StringToActionType(action_type_string);
                
                assert(action_info.type != INVALID);
                assert(action_info.frameIndexOffset >= 0);
                assert(action_info.numberOfFrames > 0);
                
                actions[action_info.type] = action_info;
            }

            file.close();
        }
        
        AnimationInfo(
            const ActionMap& actions,
            const string& model_path,
            const string& texture_path)
                : actions(actions)
                , model_path(model_path)
                , texture_path(texture_path) {}
        
        ActionType StringToActionType(const string& action_type_string) {
            
            if (action_type_string == "IDLE")   return IDLE;
            if (action_type_string == "RUN")    return RUN;
            if (action_type_string == "JUMP")   return JUMP;
            if (action_type_string == "ATTACK") return ATTACK;
            if (action_type_string == "WAVE")   return WAVE;
            if (action_type_string == "DEAD")   return DEAD;
            
            return INVALID;
        }
        
        string ActionTypeToString(const ActionType& action_type) {
            
            if (action_type == IDLE)   return "IDLE";
            if (action_type == RUN)    return "RUN";
            if (action_type == JUMP)   return "JUMP";
            if (action_type == ATTACK) return "ATTACK";
            if (action_type == WAVE)   return "WAVE";
            if (action_type == DEAD)   return "DEAD";
            
            return "INVALID";
        }
};

class AnimationModel {

    private:
        
        AnimationInfo animation_info;

        AnimationModel() {}

    public:

        AnimationModel(const AnimationInfo& animation_info) : animation_info(animation_info) {}
        
        AnimationModel(const string& path) {
            animation_info = AnimationInfo(path);
        }
        
        virtual void Render(Animation* object) = 0;
        
        string GetModelPath() { return animation_info.model_path; }
        string GetTexturePath() { return animation_info.texture_path; }

        ActionInfo GetActionInfo(const ActionType& action_type) {
            return animation_info.actions[action_type];
        }
        
        void SetActionInfo(const ActionType& action_type, const ActionInfo& action_info) {
            animation_info.actions[action_type] = action_info;
        }
        
        AnimationInfo GetAnimationInfo() { return animation_info; }
};

#endif
