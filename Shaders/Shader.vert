#version 330 core
layout(location = 0) in vec3 aPos; 
layout(location = 1) in vec3 aNormal; 
layout(location = 2) in vec2 aTexCoord; 
out vec3 normal; 
out vec2 TexCoord; 
uniform mat4 transform;
uniform mat4 projection;
void main(){
	gl_Position = projection * transform * vec4(aPos, 1.0); 
	normal = normalize((projection * transform * vec4(aNormal, 1.0)).xyz);
	//normal = vec3(1,1,1);
	TexCoord = aTexCoord; 
}