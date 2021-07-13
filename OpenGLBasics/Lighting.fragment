#version 330 core

// Constant defining the maximum number of lights that will be rendered in a given scene
#define MAX_LIGHTS 50

uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;

in vec3 normal;
in vec3 fragPos;
in vec2 texCoord;

out vec4 fragColor;

uniform vec4 lightPos;
/*uniform vec3 lightDirection;*/
uniform vec3 objectColor;

uniform vec3 viewPos;


uniform sampler2D myTexture;


struct Light
{
	// int variable describing the type of light [Direction, Point, Spot, etc.]
	int type;	
	vec3 position;
	vec3 direction;
	float radius;

	vec3 _ambient;
	vec3 _diffuse;
	vec3 _specular;

	float constant;
	float linear;
	float quadratic;
	
	float lightPower;
	vec3 lightColor;
};

// Returns the color info based on the direction light info
vec3 CalcDirLight(Light light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0f);
	// specular shading
	vec3 reflectDir = -reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), /*shininess value*/ 5);
	// combine results
	vec3 _ambient = light._ambient * ambient;
	vec3 _diffuse = light._diffuse * diff;
	vec3 _specular = light._specular * spec;

	return(_ambient + _diffuse + _specular);
};

// Returns the color info based on the spot light info
vec3 CalcSpotLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0f);
	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), /*shininess value*/5.0f);
	// attenuation
	float distance = length(light.position - fragPos);	
	float attenuation = light.lightPower / (light.constant + light.linear *
											distance + light.quadratic * (distance* distance));
	// combine results
	vec3 _ambient	= light._ambient * ambient;
	vec3 _diffuse	= light._diffuse * diff;
	vec3 _specular	= light._specular * spec;
	_ambient	*= attenuation;
	_diffuse	*= attenuation;
	_specular	*= attenuation;

	return (_ambient + _specular + _diffuse);
}

uniform Light lights[MAX_LIGHTS];
// Number of lights that exist in the scene
uniform int lightSize;


void main()
{
	// Properties
	vec3 norm = normalize(normal);
	vec3 viewDir = normalize(viewPos - fragPos);

	// Lighting result
	vec3 result = vec3(0.0f, 0.0f, 0.0f);
	for(int i = 0; i < lightSize; i++)
	{
		// if the current light is a directional light
		if(lights[i].type == 1)
			result += CalcDirLight(lights[i], norm, viewDir);

		// if the current light is a point light
		if(lights[i].type == 2)
			result += CalcSpotLight(lights[i], norm, fragPos, viewDir);
		// if the current light is a spot light
		// TODO
	}

	

	//vec3 finalColor = (ambientLighting + diffuse + specular) * objectColor * lightPower;

	//fragColor = vec4(finalColor, 1.0f) * texture(myTexture, texCoord);*/
	
	result *= objectColor;

	fragColor = vec4(result, 1.0) * texture(myTexture, texCoord);
}