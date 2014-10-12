#ifndef __MD2_HPP__
#define __MD2_HPP__

#include "Animation.hpp"

#include <string>
#include <fstream>

using namespace std;

#define MD2_IDENTITY                ("IDP2")
#define MD2_VERSION                 (8)

#define MD2_MAX_TRIANGLES           (4096)
#define MD2_MAX_VERTICES            (2048)
#define MD2_MAX_TEXTURE_COORDINATES (2048)
#define MD2_MAX_FRAMES              (512)
#define MD2_MAX_SKINS               (32)
#define MD2_MAX_NORMALS             (162)

// Quake 2 Model

namespace md2 {

    // Quake2 (anorms.h): Lookup table for md2::Vertex normals...
    extern const float NORMALS[MD2_MAX_NORMALS][3];

    struct Header {

        // Model type and version...

        char    identity[4];    // Model identity (must be equal to "IDP2")
        int     version;        // MD2 version (must be equal to 8)

        // Dimensions of the models texture map(s)...

        int skinWidth;  // Texture width
        int skinHeight; // Texture height

        // ...

        int frameSize; // UNUSED?: Size of each frame in bytes...

        // ...

        int numberOfSkins;              // Number of textures
        int numberOfVertices;           // Number of vertices per frame
        int numberOfTextureCoordinates; // Number of texture coordinates
        int numberOfTriangles;          // Number of triangles
        int numberOfGraphicCommands;    // Number of opengl commands
        int numberOfFrames;             // Number of frames

        int skinOffset;                 // Offset to skin names (64 bytes each)
        int textureCoordinateOffset;    // Offset to s-t texture coordinates
        int triangleOffset;             // Offset to triangles
        int frameOffset;                // Offset to frame data
        int graphicCommandOffset;       // Offset to opengl commands
        int endOfFileOffset;            // Offset to end of file
    };

    struct Skin {
        char path[64];
    };

    struct TextureCoordinate {
        short s;
        short t;
    };

    struct Triangle {
        short vertexIndices[3];
        short textureCoordinateIndices[3];
    };

    struct Vertex {
        unsigned char components[3];
        unsigned char normalIndex;
    };

    struct Frame {  // Variable sized struct...
        float   scale[3];
        float   translate[3];
        char    name[16];
        Vertex* vertices;
    };
}

class MD2Model : public AnimationModel {

    private:
        
        int skin_texture;

        md2::Header             header;
        md2::TextureCoordinate* texture_coordinates;
        md2::Triangle*          triangles;
        md2::Frame*             frames;

        // Lookup table: maps frame name to frame index
        map<string, int> frame_lookup;
        
        void LoadModel(const string& md2_path);
        void UnloadModel();

        void LoadTexture(const string& skin_path);
        void UnloadTexture();

    protected:

        // Used to select animation (first and last) frames
        // Returns the index of the frame if the frame name is found in the model
        // Otherwise, returns -1
        int FindFrameIndex(string const& frame_name);

    public:

        MD2Model(const AnimationInfo& animation_info);
        MD2Model(const string& act_path);

        ~MD2Model();

        void Render(Animation* object);
    };

#endif
