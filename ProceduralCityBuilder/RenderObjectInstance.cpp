#include "RenderObjectInstance.hpp"

pcb::RenderObjectInstance::RenderObjectInstance(const pcb::RenderObjectData& objectData, glm::vec3 position) : objectData(objectData), position(position), rotation(0), scale(0, 0, 0) {}