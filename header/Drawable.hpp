#ifndef __DRAWABLE_HPP__
#define	__DRAWABLE_HPP__

#include "Video.hpp"
#include "Misc.hpp"

#include <glm/common.hpp>
#include <glm/gtx/transform.hpp>

class Drawable : public Resource {
    
    friend class Video;
    
    // Abstract class:
    // Empty virtual method; void Update(unsigned int)
    
    private:
        
        glm::mat4 rotation;
        glm::mat4 translation;
        
        /* Note:
         * Since matrix multiplication and matrix inverse calculations
         * are expensive, cache the operations. Only computer new matrices
         * when the matrices are modified via rotate and translate.
         */
        
        bool is_matrix_valid;
        bool is_inverse_valid;
        
        glm::mat4 cached_matrix;
        glm::mat4 cached_inverse;
    
    public:
        
        Drawable(const string& name) : Resource(name),
                is_matrix_valid(false),
                is_inverse_valid(false) {
            
            cout << "Drawable " << Name() << " running " << IsRunning() << endl;
        }
        
        void Rotate(const float& degrees, const float& axis_x, const float& axis_y, const float& axis_z) {
            is_matrix_valid = false;
            is_inverse_valid = false;
            rotation *= glm::rotate(degrees, glm::vec3(axis_x, axis_y, axis_z));
        }
        
        void Translate(const float& delta_x, const float& delta_y, const float& delta_z) {
            is_matrix_valid = false;
            is_inverse_valid = false;
            translation *= glm::translate(glm::vec3(delta_x, delta_y, delta_z));
        }
        
        glm::mat4 GetOrientation() {
            
            if (is_matrix_valid == false) {
                is_matrix_valid = true;
                cached_matrix = translation * rotation;
            }
            
            return cached_matrix;
        }
        
        glm::mat4 GetInverseOrientation() {
            
            if (is_inverse_valid == false) {
                is_inverse_valid = true;
                cached_inverse = glm::inverse(GetOrientation());
            }
            
            return cached_inverse;
        }
        
        glm::vec4 GetPosition() {
            
            glm::vec4 position;
            glm::mat4 M = GetOrientation();
            
            position[X] = M[3][X];
            position[Y] = M[3][Y];
            position[Z] = M[3][Z];
            position[W] = 1.0f; // position information
            
            return position;
        }

        glm::vec4 GetDirection() {
            
            glm::vec4 direction;
            glm::mat4 M = GetOrientation();

            direction[X] = M[0][X];
            direction[Y] = M[0][Y];
            direction[Z] = M[0][Z];
            direction[W] = 0.0f; // no position information
            
            return glm::normalize(direction);
        }
        
        glm::vec4 GetLightPosition() {
            const glm::vec4 L = SystemInstance<Video>()->GetLightPosition();
            return GetInverseOrientation() * L;
        }
        
        glm::vec4 GetViewPosition() {
            const glm::vec4 V = SystemInstance<Video>()->GetViewPosition();
            return GetInverseOrientation() * V;
        }
};

#endif
