#include "helper.h"
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

static GLFWwindow * win = NULL;

// Constants

#define HEIGHT_IN_WINDOWED_MODE 600		// Height of the window in the windowed mode
#define WIDTH_IN_WINDOWED_MODE 600		// Width of the window in the windowed mode

// Shaders
GLuint idProgramShader;
GLuint idFragmentShader;
GLuint idVertexShader;
GLuint idJpegTexture;
GLuint idMVPMatrix;

int widthTexture, heightTexture;
bool fullscreen;			// Flag for current fullscreen status
int xpos,ypos;				// Coordinates of the window position
float heightFactor= 10.0f;
float cameraSpeed = 0.0f;

glm::vec3 cameraPosition;
glm::vec3 cameraFront    = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 cameraUp       = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 lightPosition;

float pitch = 0.0f; // We believe that The world is flat(!)
float yaw =  90.0f;

const static float aspect_ratio = 1.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

static void errorCallback(int error,
	const char * description) {
	fprintf(stderr, "Error: %s\n", description);
}

static void callback_key(GLFWwindow* win, int key, int scancode, int action, int mods)
{
    switch (key){
        case GLFW_KEY_ESCAPE:		// Closes the window altogether
            glfwSetWindowShouldClose(win, GLFW_TRUE);
            break;
        case GLFW_KEY_F:			// Fullscreen toggle 
            if(action == GLFW_PRESS){
            	if(fullscreen){
	                glfwSetWindowMonitor(win,NULL,xpos,ypos,WIDTH_IN_WINDOWED_MODE,HEIGHT_IN_WINDOWED_MODE,0);
	                fullscreen = false;
	            }
	            else {
	                fullscreen = true;
	                glfwGetWindowPos(win, &xpos, &ypos);
	                glfwSetWindowMonitor(win,glfwGetPrimaryMonitor(),0,0,1920,1080,0);	
	            }
            }
            break;
        case GLFW_KEY_O:			// Increases height factor
            heightFactor += 0.5;
            break;
        case GLFW_KEY_L:			// Decreases height factor
            heightFactor -= 0.5;
            break;
        case GLFW_KEY_U:			// Increases speed constant
            cameraSpeed += 0.045;
            break;
        case GLFW_KEY_J:			// Decreases speed constant 
            cameraSpeed -= 0.045;
            break;
        case GLFW_KEY_W:
            pitch += 1.0f;
            if(pitch == 90.0f)
                pitch = 91.0f;
            if(pitch >= 270.0f)
                pitch = -89.0f;
            if(pitch == -90.0f)
                pitch += 1.0f;

            if(pitch >= -90.0f and pitch <= 90.0f)
                cameraUp = glm::vec3(0, 1, 0);
            else
                cameraUp = glm::vec3(0, -1, 0);
            break;
        case GLFW_KEY_S:
            pitch -= 1.0f;
            if(pitch == -90.0f)
                pitch = -91.0f;
            if(pitch <= -270.0f)
                pitch = 91.0f;
            
            if(pitch >= -90.0f and pitch <= 90.0f)
                cameraUp = glm::vec3(0, 1, 0);
            else
                cameraUp = glm::vec3(0, -1, 0);

            break;
        case GLFW_KEY_A:
            yaw -= 0.75f;
            break;
        case GLFW_KEY_D:
            yaw += 0.75f;
            break;
        default:					// Any key press that is not predefined falls here
            //std:cout << "This key doesn't trigger anything." << std::endl;
            break;
        }
        //std::cout << "Pitch: " << pitch << " yaw: " << yaw << "Speed : "  << cameraSpeed <<std::endl;
        // If the key triggers the camera to change
        if(key == GLFW_KEY_W || key == GLFW_KEY_S || key == GLFW_KEY_A || key == GLFW_KEY_D)
        {
            
            glm::vec3 newFront;
            newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            newFront.y = sin(glm::radians(pitch));
            // This sin below may be with minus
            newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            cameraFront = glm::normalize(newFront);
        }	
}

int main(int argc, char * argv[]) {
	fullscreen = false;
	if (argc != 2) {
		printf("Only one texture image expected!\n");
		exit(-1);
	}

	glfwSetErrorCallback(errorCallback);

	if (!glfwInit()) {
		exit(-1);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	win = glfwCreateWindow(WIDTH_IN_WINDOWED_MODE, HEIGHT_IN_WINDOWED_MODE, "CENG477 - HW3", NULL, NULL);

    glfwSetFramebufferSizeCallback(win, framebuffer_size_callback);
	if (!win) {
		glfwTerminate();
		exit(-1);
	}

	glfwSetKeyCallback(win, callback_key);

	glfwMakeContextCurrent(win);

	GLenum err = glewInit();
	if (err != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));

		glfwTerminate();
		exit(-1);
	}

	initShaders();
	initTexture(argv[1], & widthTexture, & heightTexture);
    
    cameraPosition = glm::vec3(widthTexture/2.0f, widthTexture/10.0f, -widthTexture/4.0f);
	lightPosition = glm::vec3(widthTexture/2.0f, widthTexture+heightTexture, heightTexture/2.0f);
    
    int numOfVertices = 3 * (widthTexture+1) * (heightTexture+1);
	float* vertices = new float[numOfVertices];

	int position = 0;
    for(int z=0; z <= heightTexture; z++)
    {
    	for(int x=0; x <= widthTexture; x++)
    	{
			vertices[position] = x; 
	    	vertices[position+1] = 0; 
	    	vertices[position+2] = z;
    		position += 3; 
    	}
    }
    int numOfIndices = 6 * widthTexture * heightTexture;
    unsigned int* indices = new unsigned int[numOfIndices];
    position = 0;
    for(int z=0; z < heightTexture; z++)
    {
    	for(int x=0; x < widthTexture; x++)
    	{
            // We are at point a
    		//      a_b
    		//		|/  -> acb triangle
    		//	    c d
	    	indices[position] = x + z*(widthTexture+1); 
	    	indices[position + 1] = x + 0 + (z+1)*(widthTexture+1);
	    	indices[position + 2] = x + 1 + (z)*(widthTexture+1); 
            // We are at point a
    		//       a b
    		//		  /|  -> cdb triangle 
    		//	     c d
    		indices[position + 3] = x + 0 + (z+1)*(widthTexture+1); 
	    	indices[position + 4] = x + 1 + (z+1)*(widthTexture+1);
	    	indices[position + 5] = x + 1 + (z)*(widthTexture+1);
	    	position += 6;
    	}
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
    glEnable(GL_DEPTH_TEST); 

	unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, numOfVertices * sizeof(float), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numOfIndices * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
	
	glUseProgram(idProgramShader);
    while (!glfwWindowShouldClose(win)) {
        // render
        // ------
        //glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, idJpegTexture);

        glm::mat4 view;
        glm::mat4 projection;

        unsigned int widthLoc = glGetUniformLocation(idProgramShader, "widthTexture");
        unsigned int heightLoc  = glGetUniformLocation(idProgramShader, "heightTexture");
        unsigned int heightFactorLoc  = glGetUniformLocation(idProgramShader, "heightFactor");

        glUniform1i(widthLoc, widthTexture);
        glUniform1i(heightLoc, heightTexture);
        glUniform1f(heightFactorLoc, heightFactor);

        cameraPosition += cameraSpeed * cameraFront; // It is 0 initially
        /*
        if(pitch < -89.0f)
            cameraUp = vec3(0.0f, -1.0f, 0.0f);
        else
            cameraUp = vec3(0.0f, 1.0f, 0.0f);
        */
  		view = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);      
        
        projection = glm::perspective(45.0f, aspect_ratio, 0.1f, 1000.0f);

        // retrieve the matrix uniform locations
        unsigned int viewLoc  = glGetUniformLocation(idProgramShader, "view");
        unsigned int projLoc  = glGetUniformLocation(idProgramShader, "projection");
        unsigned int lightPosLoc  = glGetUniformLocation(idProgramShader, "lightPosition");
        unsigned int cameraPosLoc  = glGetUniformLocation(idProgramShader, "cameraPosition");

        // pass them to the shaders (3 different ways)
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3fv(lightPosLoc, 1, &lightPosition[0]);
        glUniform3fv(cameraPosLoc, 1, &cameraPosition[0]);
        // render container
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, numOfIndices, GL_UNSIGNED_INT, 0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(win);
        glfwPollEvents();
	}

	glfwDestroyWindow(win);
	glfwTerminate();
	delete[] vertices;
	delete[] indices;

	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
