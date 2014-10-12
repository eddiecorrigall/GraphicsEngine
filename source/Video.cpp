#include "Process.hpp"
#include "Video.hpp"
#include "Input.hpp"

#include "Drawable.hpp"

#include "Misc.hpp"

#include <string>
#include <iostream>

#include <vector>
#include <algorithm>

#include <cmath>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Colour GetPixel(void* image, const unsigned int& x, const unsigned int& y) {
    
    SDL_Surface* surface = (SDL_Surface*)image;
    
    assert(surface != NULL);
    assert(surface->format->BytesPerPixel > 0);
    
    SDL_LockSurface(surface);
    
    unsigned int pixel;
    unsigned char* pixel_pointer = (unsigned char*)surface->pixels + (y * surface->pitch) + (x * surface->format->BytesPerPixel);

    switch (surface->format->BytesPerPixel) {

        case 1: pixel = *(unsigned char *)pixel_pointer; break;
        case 2: pixel = *(unsigned short*)pixel_pointer; break;
        case 4: pixel = *(unsigned int  *)pixel_pointer; break;

        case 3: {
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                pixel = 0;
                pixel |= (pixel_pointer[0] << 16);
                pixel |= (pixel_pointer[1] << 8);
                pixel |= (pixel_pointer[2] << 0);
            }
            else {
                pixel = 0;
                pixel |= (pixel_pointer[0] << 0);
                pixel |= (pixel_pointer[1] << 8);
                pixel |= (pixel_pointer[2] << 16);
            }
        } break;
    }
    
    Colour pixel_colour;

    SDL_GetRGBA(
        pixel,
        surface->format,
        &(pixel_colour.red),
        &(pixel_colour.green),
        &(pixel_colour.blue),
        &(pixel_colour.alpha)
    );
    
    SDL_UnlockSurface(surface);

    return pixel_colour;
}

void SetPixel(void* image, const unsigned int& x, const unsigned int& y, const Colour& pixel_colour) {
    
    SDL_Surface* surface = (SDL_Surface*)image;
    
    unsigned int pixel = SDL_MapRGBA(
        surface->format,
        pixel_colour.red,
        pixel_colour.green,
        pixel_colour.blue,
        pixel_colour.alpha
    );
    
    SDL_LockSurface(surface);

    unsigned char* pixel_pointer = (unsigned char*)surface->pixels + (y * surface->pitch) + (x * surface->format->BytesPerPixel);
    
    switch (surface->format->BytesPerPixel) {

        case 1: *(unsigned char *)pixel_pointer = pixel; break;
        case 2: *(unsigned short*)pixel_pointer = pixel; break;
        case 4: *(unsigned int  *)pixel_pointer = pixel; break;

        case 3: {
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                pixel_pointer[0] = 0xFF && (pixel >> 0);
                pixel_pointer[1] = 0xFF && (pixel >> 8);
                pixel_pointer[2] = 0xFF && (pixel >> 16);
            }
            else {
                pixel_pointer[0] = pixel >> 16;
                pixel_pointer[1] = pixel >> 8;
                pixel_pointer[2] = pixel >> 0;
            }
        } break;
    }

    SDL_UnlockSurface(surface);
}

void Subdivide3D(
    unsigned int depth,
    vector<float>& buffer,
    const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
    
    // Recursive base-case...
    
    if (depth == 0) {
        
        // a is vertex[0]
        buffer.push_back(a[X]);
        buffer.push_back(a[Y]);
        buffer.push_back(a[Z]);
        
        // b is vertex[1]
        buffer.push_back(b[X]);
        buffer.push_back(b[Y]);
        buffer.push_back(b[Z]);
        
        // c is vertex[2]
        buffer.push_back(c[X]);
        buffer.push_back(c[Y]);
        buffer.push_back(c[Z]);
        
        return;
    }
    
    /*          [a]
     *          / \
     *         /   \
     *     (x)/-----\(y)
     *       / \   / \
     *      /___\ /___\
     *    [b]   (z)   [c]
     */
    
    // Calculate vertices along the triangle edges...
    glm::vec3 u = (b - a);
    glm::vec3 v = (c - a);
    glm::vec3 w = (c - b);
    
    // Calculate points for new triangles...
    glm::vec3 x = a + 0.5f * u;
    glm::vec3 y = a + 0.5f * v;
    glm::vec3 z = b + 0.5f * w;
    
    // Push vertices along normal...
    
    /* Note:
     * Attempted at rendering more natural polygon models.
     * This has noticeable side-effects:
     * - Edges are not attached, ie. holes in out model!
     * - Increasing subdivision can help, but run-time increases exponentially!
     */
    
    /*
    glm::vec3 N = glm::cross(u, v);
    x += 0.01f*N;
    y += 0.01f*N;
    z += 0.01f*N;
    */
    
    // Recursive step...
    
    depth--;
    
    Subdivide3D(depth, buffer, a, x, y);
    Subdivide3D(depth, buffer, x, b, z);
    Subdivide3D(depth, buffer, y, z, c);
    Subdivide3D(depth, buffer, z, y, x);
}

void Subdivide2D(
    unsigned int depth,
    vector<float>& buffer,
    const glm::vec2& a, const glm::vec2& b, const glm::vec2& c) {
    
    /*          [a]
     *          / \
     *         /   \
     *     (x)/-----\(y)
     *       / \   / \
     *      /___\ /___\
     *    [b]   (z)   [c]
     */
    
    // Recursive base-case...
    
    if (depth == 0) {
        
        // a is vertex[0]
        buffer.push_back(a[X]);
        buffer.push_back(a[Y]);
        
        // b is vertex[1]
        buffer.push_back(b[X]);
        buffer.push_back(b[Y]);
        
        // c is vertex[2]
        buffer.push_back(c[X]);
        buffer.push_back(c[Y]);
        
        return;
    }
    
    // Calculate vertices along the triangle edges...
    glm::vec2 u = (b - a);
    glm::vec2 v = (c - a);
    glm::vec2 w = (c - b);
    
    // Calculate points for new triangles...
    glm::vec2 x = a + 0.5f * u;
    glm::vec2 y = a + 0.5f * v;
    glm::vec2 z = b + 0.5f * w;
    
    // Recursive step...
    
    depth--;
    
    Subdivide2D(depth, buffer, a, x, y);
    Subdivide2D(depth, buffer, x, b, z);
    Subdivide2D(depth, buffer, y, z, c);
    Subdivide2D(depth, buffer, z, y, x);
}

void* toon_filter(
    void* image,
    const unsigned int& width,
    const unsigned int& height,
    const unsigned char& bytes_per_pixel) {
    
    #define TOON_PALETTE_SIZE (6)
    
    // Select colour palette from texture...
    
    vector<Colour> image_palette;
    
    for (unsigned int x = 0; x < width; x++) {
        for (unsigned int y = 0; y < height; y++) {
            image_palette.push_back(GetPixel(image, x, y));
        }
    }
    
    // Remove duplicates...
    
    image_palette.resize(
        distance(
            image_palette.begin(),
            unique(
                image_palette.begin(),
                image_palette.end()
            )
        )
    );
    
    // Sort...
    
    sort(image_palette.begin(), image_palette.end());
    
    // Select a palette: a subset of colours...
    // Uniformly select a subset of pixels...
    // The subset is the toon colour palette...
    // These pixels will be "farthest" away and will express higher level spectrums the best...
    
    Colour toon_palette[TOON_PALETTE_SIZE];
    
    int palette_colour_distance = floor(image_palette.size() / TOON_PALETTE_SIZE);
    
    for (int i = 0; i < TOON_PALETTE_SIZE; i++) { // Debug only...
        toon_palette[i] = image_palette[i * palette_colour_distance];
    }
    
    // Map new colour palette to image palette...
    
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            
            Colour image_colour = GetPixel(image, x, y);
            
            // Find toon_colour with the closest distance to image_colour...
            
            unsigned int distance_min = UINT_MAX;
            Colour toon_colour = toon_palette[0];
            
            for (int i = 1; i < TOON_PALETTE_SIZE; i++) {

                unsigned int distance = image_colour.Distance(toon_palette[i]);
                
                if (distance < distance_min) {
                    distance_min = distance;
                    toon_colour = toon_palette[i];
                }
            }
            
            SetPixel(image, x, y, toon_colour);
        }
    }
    
    return image;
}

TextureFilter TextureFilter_Toon = &toon_filter;

Video::Video(const string& name) : System(name) {
    
    enable_interpolation = true;
    enable_subdivision = true;
    enable_celshading = true;
    
    debug_lighting = false;
    debug_normals = false;
    debug_view = false;
    
    window = NULL;
    context = NULL;
    
    view_up = glm::vec3(0.0f, 1.0f, 0.0f);
    view_z_axis = glm::vec3(0.0f, 0.0f, -1.0f);
    view_position = glm::vec3(0.0f, 0.0f, 0.0f);
    
    light_position = glm::vec3(0.0f, 100.0f, 0.0f);
    
    // ##### SDL2...
    
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        cerr << "ERROR: Failed to initialize SDL VIDEO!" << endl;
        cerr << SDL_GetError() << endl;
        engine.Stop();
        return;
    }
    
    atexit(SDL_Quit);
    
    // Setup color depth...
    
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    
    // Setup double buffer...
    
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    
    // ...
    
    window = (void*)SDL_CreateWindow(
        engine.Name().c_str(),
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        VIDEO_WIDTH, VIDEO_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    
    if (window == NULL) {
        cerr << "ERROR: Could not create window!" << endl;
        cerr << SDL_GetError() << endl;
        engine.Stop();
        return;
    }
    
    // ##### OpenGL...
    
    context = (void*)SDL_GL_CreateContext((SDL_Window*)window);
    
    if (context == NULL) {
        cerr << "ERROR: Could not create context!" << endl;
        cerr << SDL_GetError() << endl;
        engine.Stop();
        return;
    }
    
    // ...
    
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    
    // ...
    
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    
    // Setup lighting...
    
    GLfloat material_diffuse[]  = { 0.6f, 0.6f, 0.4f, 1.0f };
    GLfloat material_specular[] = { 0.3f, 0.0f, 0.3f, 1.0f };
    GLfloat material_ambient[]  = { 0.7f, 0.7f, 0.7f, 1.0f };
    
    GLfloat material_shininess  = 5.0f;
    
    GLfloat light_diffuse[]     = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat light_specular[]    = { 0.5f, 0.5f, 0.5f, 1.0f };
    GLfloat light_ambient[]     = { 0.8f, 0.8f, 0.2f, 1.0f };
    
    glLightfv(GL_LIGHT0, GL_DIFFUSE,    light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR,   light_specular);
    glLightfv(GL_LIGHT0, GL_AMBIENT,    light_ambient);
    
    glEnable(GL_COLOR_MATERIAL);
    
    glMaterialfv(GL_FRONT, GL_DIFFUSE,  material_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
    glMaterialfv(GL_FRONT, GL_AMBIENT,  material_ambient);
    
    glMaterialf(GL_FRONT, GL_SHININESS, material_shininess);
    
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    // Setup fog...
    
    GLfloat fog[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogfv(GL_FOG_COLOR, fog);
    glFogf(GL_FOG_DENSITY, 0.01f);
    glFogf(GL_FOG_START, 1.0f);
    glFogf(GL_FOG_END, 1000.0f);
    
    glHint(GL_FOG_HINT, GL_FASTEST);
    
    glEnable(GL_FOG);
    
    // Setup viewport and perspective matrix...
    
    Resize(VIDEO_WIDTH, VIDEO_HEIGHT);
}

Video::~Video() {
    
    cout << "Video::Destroy" << endl;
    
    if (context != NULL) {
        SDL_GL_DeleteContext((SDL_GLContext)context);
    }
    
    if (window != NULL) {
        SDL_DestroyWindow((SDL_Window*)window);
    }
}

void Video::Update(const unsigned int& elapsed_milliseconds) {
    
    // Set the window context so that rendering is done in this window...
    
    if (SDL_GL_MakeCurrent((SDL_Window*)window, context) < 0) {
        cerr << "ERROR: Failed to set context to render in window!" << endl;
        cerr << SDL_GetError() << endl;
        engine.Stop();
        return;
    }
    
    // Clear screen and depth information...
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Load view-model matrix...
    
    glPushMatrix();
    
    // Reset model-view matrix...
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    
    // Setup camera...
    
    gluLookAt(
        // View position...
        view_position[X],
        view_position[Y],
        view_position[Z],
        // View look position...
        view_position[X] + view_z_axis[X],
        view_position[Y] + view_z_axis[Y],
        view_position[Z] + view_z_axis[Z],
        // View up unit vector...
        view_up[X], view_up[Y], view_up[Z]
    );
    
    ResourceList* resources = engine.Resources();
    
    for (unsigned int i = 0; i < resources->size(); i++) {
        
        Drawable* resource = (Drawable*)resources->at(i);
        
        if (resource == NULL) {
            cout << "Not drawable " << resources->at(i)->Name() << endl;
            continue;
        }
        
        glPushMatrix();
        glMultMatrixf(glm::value_ptr(resource->GetOrientation()));
        
            // Apply light to model space
            glLightfv(GL_LIGHT0, GL_POSITION, glm::value_ptr(resource->GetLightPosition()));

            // Update the resource

            /* IMPORTANT:
            * At least one system must invoke Resource::Update to keep the resource allocated.
            * Therefore, it is imperative to call Resource::Touch
            */
            
            resource->Touch();
            resource->Update(elapsed_milliseconds);
        
        glPopMatrix();
    }
    
    // Draw light position...
    
    GLUquadricObj *sphere = gluNewQuadric();
    
    glTranslatef(light_position[X], light_position[Y], light_position[Z]);
    
    glColor3f(1.0f, 1.0f, 1.0f);
    
    glBegin(GL_LINE_LOOP);
    gluQuadricDrawStyle(sphere, GLU_FILL);
    gluSphere(sphere, 10.0f, 36, 18);
    gluDeleteQuadric(sphere);
    glEndList();
    
    glPopMatrix();
    
    // Update the window...
    
    SDL_GL_SwapWindow((SDL_Window*)window);
}

void Video::Resize(const int& width, const int& height) {
    
    assert(width > 0);
    assert(height > 0);
    
    glViewport(0, 0, width, height);
    
    // Reset projection matrix...

    #define VIDEO_ASPECT ((float)width / (float)height)
    
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(VIDEO_FOV, VIDEO_ASPECT, VIDEO_NEAR, VIDEO_FAR);
}

glm::vec4 Video::GetLightPosition() {
    return glm::vec4(light_position, 1.0f);
}

glm::vec4 Video::GetViewPosition() {
    return glm::vec4(view_position, 1.0f);
}

glm::vec3 Video::GetViewXAxis() {
    return view_x_axis;
}

glm::vec3 Video::GetViewYAxis() {
    return view_y_axis;
}

glm::vec3 Video::GetViewZAxis() {
    return view_z_axis;
}

void Video::UpdateViewPitch(const float& delta_pitch) {
    
    glm::mat4 R_z = glm::rotate(glm::mat4(1.0f), delta_pitch, view_up);
    
    glm::vec4 z = R_z * glm::vec4(view_z_axis, 0.0f);
    view_z_axis[X] = z[X];
    view_z_axis[Y] = z[Y];
    view_z_axis[Z] = z[Z];
    
    view_x_axis = glm::cross(view_z_axis, view_up);
    view_y_axis = glm::cross(view_z_axis, view_x_axis);
}

void Video::UpdateViewMove(const float& delta_x, const float& delta_y, const float& delta_z) {
    
    view_position += delta_x * view_x_axis;
    view_position += delta_y * view_y_axis;
    view_position += delta_z * view_z_axis;
}

bool Video::IsTexture(const unsigned int& texture_id) {

    if (texture_id == 0) return false;
    if (glIsTexture(texture_id) == false) return false;

    return true;
}
        
unsigned int Video::LoadTexture(
    string const& path,
    unsigned int& width,
    unsigned int& height,
    unsigned char& bytes_per_pixel,
    TextureFilter filter) {

    cout << "Loading Texture..." << endl;

    cout << "Path: " << path << endl;

    SDL_Surface* image = IMG_Load(path.c_str());

    if (image == NULL) {
        cerr << "ERROR: Failed to load texture!" << endl;
        cerr << SDL_GetError() << endl;
        return 0;
    }

    // Update width, height and bytes per pixel...

    width = image->w;
    height = image->h;

    bytes_per_pixel = image->format->BytesPerPixel;

    // Validate image format...

    GLenum format;

    switch (bytes_per_pixel) {

        case 3: format = GL_RGB; break;
        case 4: format = GL_RGBA; break;

        default: { // Unknown format...

            cerr << "ERROR: Failed to load texture, unknown format!" << endl;
            cerr << gluErrorString(glGetError()) << endl;

            SDL_FreeSurface(image);
            return 0;

        } break;
    }

    // Generate one texture...

    GLuint texture_id = 0;
    glGenTextures(1, &texture_id);

    if (IsTexture(texture_id)) {

        cerr << "ERROR: Failed to generate texture!" << endl;
        cerr << gluErrorString(glGetError()) << endl;

        SDL_FreeSurface(image);
        return 0;
    }

    // Filter the texture...

    if (filter != NULL) {

        SDL_Surface* filtered_image = (SDL_Surface*)
                filter((void*)image, width, height, bytes_per_pixel);

        if (image == NULL) {
            cerr << "ERROR: Failed to filter image!" << endl;

            SDL_FreeSurface(image);
            return 0;
        }

        image = filtered_image;
    }

    // Load/render image into memory...

    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, image->pixels);

    // Assign texture parameters for rendering...

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Unbind texture...

    glBindTexture(GL_TEXTURE_2D, 0);

    // Clean up...

    SDL_FreeSurface(image);

    // Check for error...

    int error = glGetError();

    if (error != 0) {

        cerr << "ERROR: Failed to bind texture!" << endl;
        cerr << gluErrorString(error) << endl;
        cerr << error << endl;

        return 0;
    }

    // Success!

    cout << "Width:"    << width << "px" << endl;
    cout << "Height "   << height << "px" << endl;
    cout << "BPP "      << (int)bytes_per_pixel << endl;
    cout << "ID "       << texture_id << endl;

    return texture_id;
}
unsigned int Video::LoadTexture(
    string const& path,
    unsigned int& width,
    unsigned int& height,
    unsigned char& bytes_per_pixel) {

    return LoadTexture(path, width, height, bytes_per_pixel, NULL);
}

void Video::UnloadTexture(const unsigned int& texture_id) {
    
    if (IsTexture(texture_id) == false) return;
    
    glDeleteTextures(1, &texture_id);
}
