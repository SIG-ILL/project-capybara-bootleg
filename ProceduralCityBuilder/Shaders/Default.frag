#version 460

layout(location = 0) in vec3 aColor;

layout(location = 0) out vec4 resultColor;

void main() {
	resultColor = vec4(aColor, 1);
}