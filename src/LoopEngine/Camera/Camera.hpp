#pragma once

#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "glm/gtx/euler_angles.hpp"

#include "LoopEngine/Core/DisableCopyAndMove.hpp"

namespace LoopEngine::Camera {
    enum class ClearFlags {
        None = 0,
        Color = 1,
        Depth = 2,
        Stencil = 4,
    };

    struct Camera : LoopEngine::Core::DisableCopyAndMove {
        void set_clear_color(glm::vec4 color) {
            clear_color = color;
        }

        [[nodiscard]] auto get_clear_color() const -> const glm::vec4 & {
            return clear_color;
        }

//    void set_clear_flags(ClearFlags flags) {
//        clear_flags = flags;
//    }

//    auto get_clear_flags() const -> ClearFlags {
//        return clear_flags;
//    }

        void set_perspective(float fov, float aspect, float near, float far) {
            projection = glm::perspectiveLH_ZO(glm::radians(fov), aspect, near, far);
            projection[1][1] = -projection[1][1];
        }

        void set_ortho(float left, float right, float bottom, float top, float near, float far) {
            projection = glm::orthoLH_ZO(left, right, bottom, top, near, far);
            projection[1][1] = -projection[1][1];
        }

        void set_look_at(glm::vec3 eye, glm::vec3 center, glm::vec3 up) {
            view = glm::lookAtRH(eye, center, up);
        }

        [[nodiscard]] auto get_projection_matrix() const -> const glm::mat4 & {
            return projection;
        }

        void set_transform(glm::vec3 _position, glm::vec3 _rotation) {
            position = _position;
            rotation = _rotation;

            orientation = glm::yawPitchRoll(
                    glm::radians(rotation.y),
                    glm::radians(rotation.x),
                    glm::radians(rotation.z)
            );
            view = glm::inverse(glm::translate(glm::mat4(1.0f), position) * orientation);
        }

        [[nodiscard]] auto get_view_matrix() const -> const glm::mat4 & {
            return view;
        }

        [[nodiscard]] auto get_orientation() const -> const glm::mat4 & {
            return orientation;
        }

        [[nodiscard]] auto get_position() const -> const glm::vec3 & {
            return position;
        }

        [[nodiscard]] auto get_rotation() const -> const glm::vec3 & {
            return rotation;
        }

    private:
        glm::vec3 position = glm::vec3(0.0f);
        glm::vec3 rotation = glm::vec3(0.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        glm::mat4 orientation = glm::mat4(1.0f);
        glm::vec4 clear_color = {0.0f, 0.0f, 0.0f, 1.0f};
        ClearFlags clear_flags = ClearFlags::None;
    };
}