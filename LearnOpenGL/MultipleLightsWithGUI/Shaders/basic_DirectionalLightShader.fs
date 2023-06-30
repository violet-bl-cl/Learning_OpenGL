#version 460 core
out vec4 FragColor;
//keep in mind that sampler 2D is so called opaque type which means we cant instantiate.
struct Material{
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform Material material;

struct DirectionalLight{
	//vec3 position;
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform DirectionalLight directionalLight;


void main()
{
	//DIRECTIONAL LIGHT STANDARDS
	//Ambient Light
	vec3 ambient =  directionalLight.ambient * texture(material.diffuse, TexCoords).rgb;
	//diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(-directionalLight.direction);
	float diff = max(dot(norm,lightDir), 0.0f);
	vec3 diffuse =  directionalLight.diffuse * diff * texture(material.diffuse, TexCoords).rgb;
	//specualr
	//float specularStrength = 0.5f;
	vec3 viewDir = normalize(viewPos -FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir,reflectDir), 0.0f),material.shininess);
	vec3 specular =  directionalLight.specular * spec * texture(material.specular, TexCoords).rgb;


	vec3 result = ambient + diffuse + specular;
	FragColor = vec4(result, 1.0f); //set all 4 vectors values to 1.0.

}