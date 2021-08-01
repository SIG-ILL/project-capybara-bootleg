#pragma once

#include <glm/glm.hpp>

#include "RenderObjectData.hpp"

namespace pcb {
	class RenderObjectInstance {
	public:
		RenderObjectInstance(const pcb::RenderObjectData& objectData, glm::vec3 position);

		const pcb::RenderObjectData objectData;
		glm::vec3 position;
		float rotation;
		glm::vec3 scale;
		glm::mat4 modelMatrix;
	};
}