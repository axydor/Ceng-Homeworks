#version 410

layout(location = 0) in vec3 position;

// Data from CPU 
uniform vec3 cameraPosition;
uniform float heightFactor;

uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPosition;

// Texture-related data
uniform sampler2D rgbTexture;
uniform int widthTexture;
uniform int heightTexture;

// Output to Fragment Shader
out vec2 textureCoordinate; // For texture-color
out vec3 vertexNormal; // For Lighting computation
out vec3 ToLightVector; // Vector from Vertex to Light;
out vec3 ToCameraVector; // Vector from Vertex to Camera;

float find_y(vec4 rgb)
{
	return (((rgb.x *0.2126) + (0.7152 * rgb.y) + (0.0722 * rgb.z)) * heightFactor);
}

vec4 find_rgb(vec2 textureCoordinate)
{
	return texture(rgbTexture, textureCoordinate);     
}

vec3 find_normal(vec3 a, vec3 b, vec3 c)
{
// a, b, c in counter clockwise order    
	vec3 side1 = b - a; 
	vec3 side2 = c - a; 
	vec3 normal = normalize(cross(side1,side2));
	return normal;
}

// Where to calculate Normals
vec3 find_vertice(float x, float z)
{
    vec3 new_pos = vec3(x, 0, z); 
    vec2 new_textureCoordinate = vec2(1-new_pos.x/widthTexture, 1-new_pos.z/heightTexture);
    vec4 new_rgb = find_rgb(new_textureCoordinate);
    float new_y = find_y(new_rgb);
    vec3 new_result = vec3(new_pos.x, new_y, new_pos.z);
    return new_result;
}

void main()
{
    // get texture value, compute height
    // compute normal vector using also the heights of neighbor vertices
    // compute toLight vector vertex coordinate in VCS
    // set gl_Position variable correctly to give the transformed vertex position
    vec3 pos;

    pos.x = position.x;
    pos.z = position.z;
   
	textureCoordinate = vec2(1-position.x/widthTexture, 1-position.z/heightTexture);
	vec4 rgb = find_rgb(textureCoordinate);  
	pos.y = find_y(rgb);

	ToCameraVector = normalize(cameraPosition - pos);
	ToLightVector = normalize(lightPosition - pos);

   	gl_Position = projection * view * vec4(pos, 1.0);
   
	if(position.x > 0 && position.x < widthTexture && position.z > 0 && position.z < heightTexture) 
	{
        // VERTICES
        // LEFT
        vec3 left = find_vertice(position.x-1, position.z);
        // UP
        vec3 up = find_vertice(position.x, position.z-1);
        // UP_RIGHT
        vec3 up_right = find_vertice(position.x+1, position.z-1);
        // RIGHT
        vec3 right = find_vertice(position.x+1, position.z);
        //DOWN
        vec3 down = find_vertice(position.x, position.z+1);
        // DOWN_LEFT
        vec3 down_left = find_vertice(position.x-1, position.z+1);
        // VERTICES ENDS
		//
        // TRIANGLE NORMALS
        vec3 normal_left = find_normal(left, pos, up);
        vec3 normal_up_right = find_normal(pos, up_right, up);
        vec3 normal_right = find_normal(pos, right, up_right);
        vec3 normal_down_right = find_normal(down, right, pos);
        vec3 normal_down = find_normal(down_left, down, pos);
        vec3 normal_down_left = find_normal(down_left, pos, left);

        vec3 aNormal = (normal_up_right + normal_right + normal_down_right + normal_left + normal_down + normal_down_left)/6;
    	vertexNormal = normalize(aNormal);
    }
    else
    {
        vertexNormal = vec3(0,0,0);    
    }
}

// s/^\/\///g -> in Vim to comment out lines
