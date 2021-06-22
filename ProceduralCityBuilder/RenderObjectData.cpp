#include "RenderObjectData.hpp"

pcb::RenderObjectData::RenderObjectData(const pcb::VertexArrayObject& vertexArrayObject, const pcb::ShaderProgram& shaderProgram) : vertexArrayObject(vertexArrayObject), shaderProgram(shaderProgram) {}