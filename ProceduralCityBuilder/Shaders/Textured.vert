#version 460

layout(location = 0) in vec3 aPosition;
layout(location = 2) in vec2 aTextureCoordinate;

layout(location = 0) out vec2 textureCoordinate;

layout(location = 0) uniform mat4 uProjectionMatrix;
layout(location = 1) uniform mat4 uViewMatrix;
layout(location = 2) uniform mat4 uModelMatrix;

void main() {
	textureCoordinate = aTextureCoordinate;
	gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(aPosition, 1);
}