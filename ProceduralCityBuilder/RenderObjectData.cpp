#include "RenderObjectData.hpp"

pcb::RenderObjectData::RenderObjectData(const pcb::GLBufferObject& vertices, const pcb::ShaderProgram& shaderProgram) : vertices(vertices), shaderProgram(shaderProgram) {}