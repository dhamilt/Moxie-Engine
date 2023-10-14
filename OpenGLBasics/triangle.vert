//we will be using glsl version 4.5 syntax
#version 450

//array of positions for the triangle
	vec2 positions[3] = vec2[](
		vec2(0.0, -0.5),
		vec2(0.5, 0.5),
		vec2(-0.5, 0.5)
	);

void main()
{
	//output the position of each vertex
	gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
}