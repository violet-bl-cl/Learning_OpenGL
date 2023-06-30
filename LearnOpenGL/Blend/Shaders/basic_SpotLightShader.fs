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

struct SpotLight{
	vec3 position;
	vec3 direction;
	float cutOff;
	float outerCutOff;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	
	float constant;
	float linear;
	float quadratic;
};

uniform SpotLight spotLight;

void main()
{

	//Ambient Light
	vec3 ambient =  spotLight.ambient * texture(material.diffuse, TexCoords).rgb;
	//diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(spotLight.position - FragPos);	
	float diff = max(dot(norm,lightDir), 0.0f);
	vec3 diffuse =  spotLight.diffuse * diff * texture(material.diffuse, TexCoords).rgb;
	//specualr
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir,reflectDir), 0.0f),material.shininess);
	vec3 specular =  spotLight.specular * spec * texture(material.specular, TexCoords).rgb;
	//spot light edges.
	float theta = dot(lightDir, normalize(-spotLight.direction));
	float epsilon = spotLight.cutOff - spotLight.outerCutOff;
	float intensity = clamp((theta - spotLight.outerCutOff) / epsilon, 0.0f, 1.0f);

	diffuse *= intensity;
	specular *= intensity;	

	//SPOT LIGHT STANDARDS
	float distance = length(spotLight.position - FragPos);
	float attenuation = 1.0f / (spotLight.constant + spotLight.linear * distance + spotLight.quadratic * (distance * distance)); 

	//ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;


	vec3 result = ambient + diffuse + specular;
	FragColor = vec4(result, 1.0f); //set all 4 vectors values to 1.0.

}