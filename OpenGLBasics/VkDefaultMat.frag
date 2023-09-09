#version 450 core



// max number of lights to apply in shader
const int maxLightCount = 10;
layout(std140, set = 0,binding = 0)uniform LightProperties
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
} Lights[maxLightCount];

layout(std140, set = 1, binding = 0)uniform lightBufInfo 
{
	int lightCount;
} lightInfo;

layout(std140, set = 2, binding = 0)uniform materialBufInfo
{
	float shininess;
	float strength;
	vec4 objectColor;
} materialInfo;

layout(std140, set = 3, binding = 0)uniform cameraBufInfo
{
	vec4 eyeDir;
} cameraInfo;


layout(location = 3)in vec2 texCoord;
layout(location = 2)in vec4 normal;
layout(location = 1)in vec4 position;

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
	FragColor = materialInfo.objectColor;
	
}