#version 460 core
out vec4 FragColor;

in VS_OUT{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
} fs_in;

uniform sampler2D floorTexture;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform bool blinn;
uniform float kShininess;
void main()
{
	vec3 color = texture(floorTexture, fs_in.TexCoords).rgb;
	//ambient
	vec3 ambient = 0.05f * color;
	//diffuse
	vec3 lightDir = normalize(lightPos - fs_in.FragPos);
	vec3 normal = normalize(fs_in.Normal);
	float diff = max(dot(lightDir, normal),0.0f);
	vec3 diffuse = diff * color;
	//specular 
	vec3 viewDir = normalize(viewPos - fs_in.FragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = 0.0f;

	// relative brihtness issue fixed.
	const float kPi = 3.1459265;
//	const float kShininess = 16.0;

	if(blinn){
		const float kEnergyConservation = (8.0f + kShininess) / (8.0f * kPi);
		vec3 halfwayDir = normalize(lightDir + viewDir);
		spec = kEnergyConservation * pow(max(dot(normal,halfwayDir),0.0f),kShininess);
	}
	else{
	    const float kEnergyConservation = (2.0f + kShininess) / (2.0f * kPi);
		vec3 reflectDir = reflect(-lightDir, normal);
		spec = kEnergyConservation * pow(max(dot(viewDir, reflectDir),0.0f),kShininess);
	}

//normlalized vector ?
	vec3 specular = vec3(0.3f) * spec; //assuming bright white light color.
	FragColor = vec4(ambient + diffuse + specular, 1.0f);

}