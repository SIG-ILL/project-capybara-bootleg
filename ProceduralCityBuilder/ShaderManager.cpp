#include "ShaderManager.hpp"

void pcb::ShaderManager::createVertexShader(const std::string& name, const std::string& sourceCode) {
	vertexShaders.insert(std::make_pair(name, std::make_unique<pcb::VertexShader>(sourceCode)));
}

void pcb::ShaderManager::createFragmentShader(const std::string& name, const std::string& sourceCode) {
	fragmentShaders.insert(std::make_pair(name, std::make_unique<pcb::FragmentShader>(sourceCode)));
}

void pcb::ShaderManager::createProgram(const std::string& name, const std::string& vertexShaderName, const std::string& fragmentShaderName) {
	programs.insert(std::make_pair(name, std::make_unique<pcb::ShaderProgram>(*(vertexShaders.at(vertexShaderName)), *(fragmentShaders.at(fragmentShaderName)))));
}

void pcb::ShaderManager::useProgram(const std::string& name) const {
	programs.at(name)->use();
}