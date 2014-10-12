#include "Process.hpp"

#include "Video.hpp"
#include "Audio.hpp"
#include "Input.hpp"

#include "MD2.hpp"

int main(int argc, char** argv) {
    
    srand(time(NULL));
    
    // ...
    
    SystemInstance<Video>(); // ie. core.InsertProcess(&typeid(Video), new Video("video"));
    SystemInstance<Audio>();
    SystemInstance<Input>();
    
    // ...
    
    AnimationInfo knight_info = AnimationInfo("data/knight.act");
    AnimationInfo orgo_info = AnimationInfo("data/orgo.act");
    
    AnimationModel* knight_model    = new MD2Model(knight_info);
    AnimationModel* orgo_model      = new MD2Model(orgo_info);
    
    Animation* knight0 = new Animation("knight", knight_model);
    knight0->Translate(0.0f, 0.0f, -250.0f);
    knight0->Rotate(-120.0f, 0.0f, 1.0f, 0.0f);
    knight0->SetAction(DEAD);
    
    Animation* orgo0 = new Animation("orgo", orgo_model);
    orgo0->Translate(-100.0f, 0.0f, -250.0f);
    orgo0->SetAction(WAVE);
    
    // ...
    
    ResourceList* entities = engine.Resources();
    
    entities->push_back(knight0);
    entities->push_back(orgo0);
    
    engine.Start();
    
    for (unsigned int i = 0 ; i < entities->size(); i++) {
        delete entities->at(i);
    }
    
    entities->clear();
    
    delete knight_model;
    delete orgo_model;
    
    return 0;
}
