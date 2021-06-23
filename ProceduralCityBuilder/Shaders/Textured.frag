#version 460

layout(location = 0) in vec2 textureCoordinate;

layout(location = 0) out vec4 resultColor;

layout(location = 3) uniform sampler2D uTexture;

void main() {
	resultColor = texture(uTexture, textureCoordinate);
}