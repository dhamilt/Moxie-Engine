#version 330 core

layout(location = 0) in vec3 vertexPosition_MS;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 normal_MS;
layout(location = 3) in vec3 vertexColor;

out vec3 fragmentColor;
out vec2 texCoord;
out vec3 normal_CS;
out vec3 lightDir_CS;
out vec3 position_WS;
out vec3 eyeDir_CS;

// Values that stay constant for the whole mesh
uniform mat4 mvp;
uniform mat4 m_Model;
uniform mat4 v_View;
uniform mat4 p_Projection;
uniform vec3 lightPos_WS;
uniform vec3 lightColor;
uniform float lightPower;
// Material properties
uniform vec3 mat_Diffuse; // Color of the object
uniform vec3 mat_Specular; // Reflection color of the object
uniform mat4 transform; // transformations of the object

void main()
{
	// Determine the position of the vertex in clip space
	gl_Position = mvp * vec4(vertexPosition_MS.xyz, 1.0);

	// Position of the vertex in world space
	position_WS = (m_Model * vec4(vertexPosition_MS, 1)).xyz;

	// Vector that goes from the vertex to the camera, in camera space
	// In camera space, the camera is at the origin (0,0,0)
	vec3 vertexPosition_CS = (v_View * m_Model * vec4(vertexPosition_MS, 1)).xyz;
	eyeDir_CS = vec3(0, 0, 0) - vertexPosition_CS;

	// Vector that goes from the vertex to the light, in camera space,
	// Model matrix is not included because it is an identity Matrix
	vec3 lightPos_CS = (v_View * vec4(lightPos_WS, 1)).xyz;
	lightDir_CS = lightPos_CS + eyeDir_CS;
	// Direction of the light (from the fragment to the light)
	vec3 l = normalize(lightDir_CS);


	// Normal of the vertex in camera space
	// NOTE: This only works if the Model matrix has not been scaled!
	// Inveres transpose if so!
	normal_CS = (v_View * m_Model * vec4(normal_MS, 0)).xyz;
	// Normal of the computed fragment in camera space
	vec3 n = normalize(normal_CS);


	// Cosine of the angle between normal and the light direction
	// clamped above 0
	// - light is at the vertical of the triangle -> 1
	// - light is perpendicular to the triangle -> 0
	// - light is behind the triangle -> 0
	float cosTheta = clamp(dot(n, l), 0, 1);

	// Eye vector (towards the camera)
	vec3 e = normalize(eyeDir_CS);
	// Direction in which the triangle reflects the light
	vec3 r = reflect(-l, n);

	// Cosine of the angle between the eye vector and the reflect vector,
	// clamped to 0
	//  - Looking into the reflection -> 1
	//  - Looking elsewhere -> < 1
	float cosAlpha = clamp(dot(e, r), 0, 1);

	// Get the distance from the light to the vertex position
	float distance = distance(lightPos_WS, position_WS);
	// Ambient: simulates indirect lighting
	vec3 mat_Ambient = vec3(0.15, 0.15, 0.15) * mat_Diffuse;

	fragmentColor = mat_Ambient +
		// Diffuse: color of the object
		mat_Diffuse * lightColor * lightPower * cosTheta / (distance * distance) +
		// Specular: reflective highlight, like a mirror
		(mat_Specular + vertexColor) * lightColor * lightPower * pow(cosAlpha, 5) / (distance * distance);

	//fragmentColor = /*vec3(0.0f, 0.45f, 0.65f) +*/ vertexColor;
	
	texCoord = vertexUV;
}