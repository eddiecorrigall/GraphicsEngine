#ifndef __ANIMATION_HPP__
#define	__ANIMATION_HPP__

#include "Video.hpp"
#include "Drawable.hpp"
#include "AnimationModel.hpp"

#include <string>
#include <fstream>

using namespace std;

class Animation : public Drawable {

    // Abstract class:
    // Empty virtual method; void Update(unsigned int)

    private:

        AnimationModel* model;
        ActionType action;
        
        unsigned int frame;
        float frame_interp;

    protected:

        virtual void Update(const unsigned int& elapsed_milliseconds) {

            model->Render(this);

            float elapsed_seconds = (float)elapsed_milliseconds / 1000.0f;
            
            ActionInfo action_info = model->GetActionInfo(GetAction());
            
            float next_frame_interp = frame_interp + ((float)VIDEO_FPS * elapsed_seconds) / (float)action_info.numberOfFrames;
            
            if (next_frame_interp < 1.0f) {
                frame_interp = next_frame_interp;
                return;
            }
            
            unsigned int next_frame = frame + 1;
            
            if (next_frame < action_info.numberOfFrames) {
                frame = next_frame;
                frame_interp = 0.0f;
                return;
            }
            
            if (action_info.loop) {
                frame = 0;
                frame_interp = 0.0f;
            }
        }

    public:

        Animation(const string& name, AnimationModel* model)
            : Drawable(name)
            , model(model)
            , action(INVALID)
            , frame(0)
            , frame_interp(0.0f) {
            
            SetAction(IDLE); // Default action!
        }

        virtual ~Animation() {}

        ActionType GetAction() {
            return action;
        }
        
        void SetAction(const ActionType& new_action) {

            if (GetAction() == new_action) return;
            action = new_action;
            
            // ...
            
            frame = 1;
            frame_interp = 0.0f;

            ActionInfo action_info = model->GetActionInfo(GetAction());
            
            if (GetAction() == IDLE) {
                frame = random<int>(0, action_info.numberOfFrames);
            }
        }

        unsigned int GetFrame() {
            return frame;
        }
        
        // Note: frame_offset can be both negative and positive
        unsigned int GetFrameIndex(const int& frame_offset) {

            const ActionInfo action_info = model->GetActionInfo(GetAction());
            
            int frame_index = frame + frame_offset;
            
            while (frame_index < 0) {
                frame_index += action_info.numberOfFrames;
            }
            
            frame_index %= action_info.numberOfFrames;
            frame_index += action_info.frameIndexOffset;
            
            return frame_index;
        }

        unsigned int GetCurrentFrameIndex() {
            return GetFrameIndex(-1);
        }
        
        unsigned int GetNextFrameIndex() {
            return GetFrameIndex(0);
        }

        float GetFrameInterp() {
            return frame_interp;
        }
};

#endif
