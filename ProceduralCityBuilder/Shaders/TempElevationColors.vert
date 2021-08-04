#version 460

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aColor;

layout(location = 0) out vec3 color;

layout(location = 0) uniform mat4 uProjectionMatrix;
layout(location = 1) uniform mat4 uViewMatrix;
layout(location = 2) uniform mat4 uModelMatrix;

void main() {
	color = vec3(0, 0.5, 1);
	if(aPosition.y > 0.1){
		color = vec3(0.9, 0.85, 0.45);
	}
	if(aPosition.y > 0.2){
		color = vec3(0, 1, 0);
	}
	if(aPosition.y > 0.375){
		color = vec3(0, 0.75, 0);
	}
	if(aPosition.y > 0.5){
		color = vec3(0.42, 0.2, 0);
	}
	if(aPosition.y > 0.8){
		color = vec3(0.8, 0.8, 0.9);
	}
	
	gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(aPosition, 1);
}