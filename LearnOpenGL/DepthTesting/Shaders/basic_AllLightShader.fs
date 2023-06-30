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

struct DirLight{
	vec3 direction;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform DirLight dirLight;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);

struct PointLight{
	vec3 position;
	
	float constant;
	float linear;
	float quadratic;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);


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
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
void main()
{
	//properties 
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
	
	//phase 1: directional lighting
	vec3 result = CalcDirLight(dirLight, norm, viewDir);
	
	
	//phase 2: point lights
	for(int i = 0; i < NR_POINT_LIGHTS; i++){
		result += CalcPointLight(pointLights[i], norm ,FragPos, viewDir);
	}
	//phase 3: adding spot lights.
	result += CalcSpotLight(spotLight, norm, FragPos, viewDir);
	
	FragColor = vec4(result, 1.0f); //set all 4 vectors values to 1.0.

}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir){
	vec3 lightDir = normalize(-light.direction);
	//diffuse shading
	float diff = max(dot(normal, lightDir),0.0f);
	//specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir,reflectDir),0.0f),material.shininess);
	//combine results;
	vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
	vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;
	vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;
	
	return(ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir){
	vec3 lightDir = normalize(light.position - fragPos);
	//diffuse shading
	float diff = max(dot(normal,lightDir), 0.0f);
	//specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir),0.0f), material.shininess);
	//attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	
	//combine results
	vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
	vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;
	vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	
	return (ambient + diffuse + specular);
	
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir){
	//ambient
	vec3 ambient =  spotLight.ambient * texture(material.diffuse, TexCoords).rgb;
	vec3 lightDir = normalize(light.position - fragPos);
	float diff = max(dot(normal,lightDir), 0.0f);
	vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;
	//specular 
	//vec3 viewDir = normalize(viewPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir,reflectDir), 0.0f), material.shininess);
	vec3 specular =light.specular * spec * texture(material.specular, TexCoords).rgb;
	
	//spot light edges.
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0f, 1.0f);
	
	//multiplying intensity
	diffuse *= intensity;
	specular *= intensity;
	//Spot Light Standards
	float distance = length(light.position - fragPos);
	float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	// we are not going to add ambient otherwise its too bright.
	diffuse *= attenuation;
	specular *= attenuation;
	
	return (ambient + diffuse + specular);

}