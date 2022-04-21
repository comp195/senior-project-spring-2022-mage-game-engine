#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace mage {

	class CameraHandling {
		private:
			glm::mat4 projection_matrix{1.f};
			glm::mat4 view_matrix{1.f};
		public:
			CameraHandling();
			~CameraHandling();
			void set_orthographic_projection(float left, float right, float top, float bottom, float near, float far);
			void set_perspective_projection(float fovy, float aspect, float near, float far);
			void set_view_direction(glm::vec3 camera_position, glm::vec3 camera_direction, glm::vec3 up = glm::vec3{0.f, -1.f, 0.f});
			void set_view_target(glm::vec3 position, glm::vec3 target, glm::vec3 up);
			void set_view_euler(glm::vec3 position, glm::vec3 rotation);
			const glm::mat4& get_projection_matrix() const {return projection_matrix;};			
			const glm::mat4& get_view_matrix() const {return view_matrix;};
	};

}