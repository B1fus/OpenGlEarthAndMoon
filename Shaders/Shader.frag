#version 330 core
out vec4 FragColor; 
in vec3 normal; 
in vec2 TexCoord;  
uniform sampler2D ourTexture; 
void main(){
	float a = dot(normal, vec3(1,1,1));
	if(a<0.2) a = 0.2;
	FragColor = texture(ourTexture, TexCoord) * a; 
}