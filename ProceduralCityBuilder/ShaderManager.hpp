#pragma once

#include <memory>
#include <map>
#include <string>

#include "ShaderProgram.hpp"

namespace pcb {
	class ShaderManager {
	public:
		void createVertexShader(const std::string& name, const std::string& sourceCode);
		void createFragmentShader(const std::string& name, const std::string& sourceCode);
		void createProgram(const std::string& name, const std::string& vertexShaderName, const std::string& fragmentShaderName);

		void useProgram(const std::string& name) const;

	private:
		std::map<std::string, std::unique_ptr<pcb::VertexShader>> vertexShaders;
		std::map<std::string, std::unique_ptr<pcb::FragmentShader>> fragmentShaders;
		std::map<std::string, std::unique_ptr<pcb::ShaderProgram>> programs;
	};
}