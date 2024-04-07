#version 450 core

struct LightProperties
{
	bool isEnabled;
	bool isLocal;
	bool isSpot;
	vec3 ambient;
	vec3 color;
	vec3 position;
	vec3 halfVector;
	vec3 coneDir;
	float spotCosineCutoff;
	float spotExponent;
	float constantAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;
};

// max number of lights to apply in shader
layout(constant_id = 0) const int maxLightCount = 10;
layout(set = 1, binding = 3)uniform LightPropertyBuffer
{
LightProperties Lights[maxLightCount];
int lightCount;
float shininess;
float strength;
}lightProperties;

layout(set = 1, binding = 4)uniform ViewPropertyBuffer
{
vec3 eyeDir;
}viewProperties;

layout(set = 1, binding = 5)uniform ObjectPropertyBuffer
{
vec4 color;
}objectProperties;

layout(location = 3) in vec3 normal;
layout(location = 4) in vec2 texCoord;
layout(location = 5) in vec4 position;

layout(location = 0)out vec4 FragColor;

void main()
{
	//// global ambient light
	//vec3 scatteredLight = vec3(0.0);
	//vec3 reflectedLight = vec3(0.0);

	//// loop through lights
	//for (int i = 0; i < lightCount; i++)
	//{
	//	LightProperties light = Lights[i];
	//	if (!light.isEnabled)
	//		continue;
	//	else
	//	{
	//		vec3 halfVector;
	//		vec3 lightDir = light.position;
	//		float attenuation = 1.0;
	//		//for local lights, compute per-fragment direction
	//		if (light.isLocal)
	//		{
	//			lightDir = lightDir - vec3(position);
	//			float lightDist = length(lightDir);
	//			lightDir = lightDir / lightDist;
	//			attenuation = 1.0 / (light.constantAttenuation
	//				+ light.linearAttenuation * lightDist
	//				+ light.quadraticAttenuation * lightDist
	//				* lightDist);

	//			// if local light is a spot light
	//			if (light.isSpot)
	//			{
	//				float spotCosine = dot(lightDir, -light.coneDir);
	//				if (spotCosine < light.spotCosineCutoff)
	//					attenuation = 0.0;
	//				else
	//					attenuation *= pow(spotCosine, light.spotExponent);
	//			}
	//			halfVector = normalize(lightDir + eyeDir);
	//		}
	//		else
	//		{
	//			halfVector = light.halfVector;
	//		}
	//		float diffuse = max(0.0, dot(normal, lightDir));
	//		float specular = max(0.0, dot(normal, halfVector));
	//		// if the material has no light hitting it
	//		if (diffuse == 0.0)
	//			// don't calculate for specular lighting
	//			specular = 0.0;
	//		else
	//			specular = pow(specular, shininess) * strength;

	//		// Combine all of the lights' effects
	//		scatteredLight += light.ambient * attenuation + light.color * diffuse* attenuation;
	//		reflectedLight += light.color * specular * attenuation;
	//		
	//	}
	//	vec3 rgb = min(objectColor.rgb * scatteredLight + reflectedLight, vec3(1.0));
	//	FragColor = vec4(rgb, objectColor.a);
	FragColor = objectProperties.color;
	
}