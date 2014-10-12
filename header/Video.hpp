#ifndef __VIDEO_HPP__
#define	__VIDEO_HPP__

#include "Process.hpp"
#include "Colour.hpp"

#include <glm/glm.hpp>

#include <string>

using namespace std;

#define VIDEO_FPS       (48.0f)

#define VIDEO_FOV       (30.0f)
#define VIDEO_NEAR      (1.0f)
#define VIDEO_FAR       (1000.0f)
#define VIDEO_WIDTH     (800)
#define VIDEO_HEIGHT    (600)

#define X (0)
#define Y (1)
#define Z (2)
#define W (3)

typedef void* (*TextureFilter)(
    void* image,
    const unsigned int& width,
    const unsigned int& height,
    const unsigned char& bytes_per_pixel);

extern TextureFilter TextureFilter_Toon;

extern Colour GetPixel(void* image, const unsigned int& x, const unsigned int& y);
extern void SetPixel(void* image, const unsigned int& x, const unsigned int& y, const Colour& pixel_colour);

extern void Subdivide3D( // Warning: Recursive function
    unsigned int depth,
    vector<float>& buffer,
    const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);

extern void Subdivide2D( // Warning: Recursive function
    unsigned int depth,
    vector<float>& buffer,
    const glm::vec2& a, const glm::vec2& b, const glm::vec2& c);

class Video : public System {
    
    private:
        
        //SDL_Window* window;
        //SDL_GLContext context;
        
        void* window;
        void* context;
        
        glm::vec3 light_position;
        
        glm::vec3 view_up;
        glm::vec3 view_z_axis;
        glm::vec3 view_y_axis;
        glm::vec3 view_x_axis;
        glm::vec3 view_position;
        
        bool enable_interpolation;
        bool enable_subdivision;
        bool enable_celshading;
        
        bool debug_lighting;
        bool debug_normals;
        bool debug_view;
        
    protected:
        
        void Update(const unsigned int& elapsed_milliseconds);
        
    public:
        
        Video(const string& name);
        ~Video();
        
        void Resize(const int& width, const int& height);
        
        bool IsInterpolationEnabled() { return enable_interpolation; }
        bool IsSubdivisionEnabled() { return enable_subdivision; }
        bool IsCelshadingEnabled() { return enable_celshading; }
        
        void ToggleInterpolation() { enable_interpolation = !enable_interpolation; }
        void ToggleSubdivision() { enable_subdivision = !enable_subdivision; }
        void ToggleCelshading() { enable_celshading = !enable_celshading; }
        
        bool IsDebuggingLighting() { return debug_lighting; }
        bool IsDebuggingNormals() { return debug_normals; }
        bool IsDebuggingView() { return debug_view; }
        
        bool IsDebugging() {
            if (IsDebuggingLighting()) return true;
            if (IsDebuggingNormals()) return true;
            if (IsDebuggingView()) return true;
            return false;
        }
        
        glm::vec4 GetLightPosition();
        glm::vec4 GetViewPosition();
        
        glm::vec3 GetViewXAxis();
        glm::vec3 GetViewYAxis();
        glm::vec3 GetViewZAxis();
        
        void UpdateViewPitch(const float& delta_pitch);
        void UpdateViewMove(const float& delta_x, const float& delta_y, const float& delta_z);
        
        bool IsTexture(const unsigned int& texture_id);
        
        // Return 0 on failure,
        // Otherwise return a texture_id
        unsigned int LoadTexture(
            string const& path,
            unsigned int& width,
            unsigned int& height,
            unsigned char& bytes_per_pixel,
            TextureFilter filter);
        
        unsigned int LoadTexture(
            string const& path,
            unsigned int& width,
            unsigned int& height,
            unsigned char& bytes_per_pixel);
        
        void UnloadTexture(const unsigned int& texture_id);
};

#endif
