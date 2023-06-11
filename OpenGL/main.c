#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

const int windowWidth = 1366;
const int windowHeight = 768;

const char *computeShaderSource = "#version 430 core\n"
	"uniform int pretty;\n"
	"uniform int quality;\n"
	"uniform float zoom;\n"
	"uniform float movex;\n"
	"uniform float movey;\n"
	"layout(rgba8, binding = 0) uniform writeonly image2D outputTexture;\n"
	"layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;\n"
	"void main(){\n"
	"ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);\n"
	"float bright = 0.5;\n"
	"double pointc[2];\n"
	"pointc[0] = movex + ((double(pixelCoords.x) / double(imageSize(outputTexture).x))-0.5)*(double(imageSize(outputTexture).x)/double(imageSize(outputTexture).y))/zoom;\n"
	"pointc[1] = movey + ((double(pixelCoords.y) / double(imageSize(outputTexture).y))-0.5)/zoom;\n"
	"int i = 0;\n"
	"double z0[2] = {0, 0};\n"
	"double z1[2] = {0, 0};\n"
	"while ( (i < quality) && (4.0 > (z0[0]*z0[0] + z0[1]*z0[1])) ) {\n"
	"	z1[0] = (z0[0]*z0[0] - z0[1]*z0[1]) + pointc[0];\n"
	"	z1[1] = (z0[0]*z0[1]*2) + pointc[1];\n"
	"	z0[0] = z1[0];\n"
	"	z0[1] = z1[1];\n"
	"	++i;\n"
	"}\n"
	"bright = float(i) / float(quality) * (((pretty + 1)*15)+1);\n"
	"bright = 1-bright;\n"
	"vec4 pixel = vec4(mod(bright, 1), mod(bright*0.8, 1), mod(bright*0.6, 1), 1.0);\n"
	//"vec4 pixel = vec4(float(pixelCoords.x) / float(imageSize(outputTexture).y), float(pixelCoords.y) / float(imageSize(outputTexture).y), 1.0, 1.0);\n"
	"imageStore(outputTexture, pixelCoords, pixel);\n"
	"}\0";


const char *vertexShaderSource = "#version 330 core\n"
	"layout (location = 0) in vec2 position;\n"
	"layout (location = 1) in vec2 texCoord;\n"
	"out vec2 fragTexCoord;\n"
	"void main(){\n"
	"	gl_Position = vec4(position, 0.0, 1.0);\n"
	"	fragTexCoord = texCoord;\n"
	"}\n";

const char *fragmentShaderSource = "#version 330 core\n"
	"in vec2 fragTexCoord;\n"
	"out vec4 fragColor;\n"
	"uniform sampler2D inputTexture;\n"
	"void main(){\n"
	"	fragColor = texture(inputTexture, fragTexCoord);\n"
	"}\n";

int main(){

	if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Shaders are difficult", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
	
	glfwMakeContextCurrent(window);
	
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		fprintf(stderr, "Failed to initialize GLAD\n");
		glfwTerminate();
		return -1;
	}

	glViewport(0, 0, windowWidth, windowHeight);

	GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(computeShader, 1, &computeShaderSource, NULL);
	glCompileShader(computeShader);

    GLuint computeProgram = glCreateProgram();
    glAttachShader(computeProgram, computeShader);
    glLinkProgram(computeProgram);

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    GLuint postProcessingShaderProgram = glCreateProgram();
    glAttachShader(postProcessingShaderProgram, vertexShader);
    glAttachShader(postProcessingShaderProgram, fragmentShader);
    glLinkProgram(postProcessingShaderProgram);


	float vertices[] = {
		-1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f
	};

	GLuint VBO, VAO;
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4* sizeof(float), (void*)(2 * sizeof(float)));

	glBindVertexArray(0);

	GLuint framebuffer, outputTexture;
	glGenFramebuffers(1, &framebuffer);
	glGenTextures(1, &outputTexture);

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glBindTexture(GL_TEXTURE_2D, outputTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, windowWidth, windowHeight);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, outputTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        fprintf(stderr, "Framebuffer is not complete\n");
        glfwTerminate();
		return -1;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	
	int prettyValue = -1; // -1 is off. 1 is on
	GLint prettyLoc = glGetUniformLocation(computeProgram, "pretty");
	int qualityValue = 50;
	GLint qualityLoc = glGetUniformLocation(computeProgram, "quality");
	int update = 1;
	double zoomValue = 1.0;
	GLint zoomLoc = glGetUniformLocation(computeProgram, "zoom");
	double movexValue = 0.0;
	GLint movexLoc = glGetUniformLocation(computeProgram, "movex");
	double moveyValue = 0.0;
	GLint moveyLoc = glGetUniformLocation(computeProgram, "movey");
	int prettyFlag = 1; // 1 is default
	int qualityFlag = 0;
	double zoomFlag = 1;
	double movexFlag = 0;
	double moveyFlag = 0;
	
	int qStateOld = GLFW_RELEASE;
	int wStateOld = GLFW_RELEASE;
	int pStateOld = GLFW_RELEASE;
	int upStateOld = GLFW_RELEASE;
	int downStateOld = GLFW_RELEASE;
	int leftStateOld = GLFW_RELEASE;
	int rightStateOld = GLFW_RELEASE;
	int zStateOld = GLFW_RELEASE;
	int oStateOld = GLFW_RELEASE;
	int escapeStateOld = GLFW_RELEASE;

	while (!glfwWindowShouldClose(window)){
		glfwPollEvents();

		int qState = glfwGetKey(window, GLFW_KEY_Q);
		int wState = glfwGetKey(window, GLFW_KEY_W);
		int pState = glfwGetKey(window, GLFW_KEY_P);
		int upState = glfwGetKey(window, GLFW_KEY_UP);
		int downState = glfwGetKey(window, GLFW_KEY_DOWN);
		int leftState = glfwGetKey(window, GLFW_KEY_LEFT);
		int rightState = glfwGetKey(window, GLFW_KEY_RIGHT);
		int zState = glfwGetKey(window, GLFW_KEY_Z);
		int oState = glfwGetKey(window, GLFW_KEY_O);
		int escapeState = glfwGetKey(window, GLFW_KEY_ESCAPE);

		if (update) {
			update = 0;
			prettyValue *= prettyFlag;
			prettyFlag = 1;
			qualityValue += qualityFlag*5;
			qualityFlag = 0;
			zoomValue *= zoomFlag;
			zoomFlag = 1;
			movexValue += 0.1*movexFlag*(1/zoomValue);
			movexFlag = 0;
			moveyValue += 0.1*moveyFlag*(1/zoomValue);
			moveyFlag = 0;
			

			glUseProgram(computeProgram);
			glUniform1i(prettyLoc, prettyValue);
			glUniform1i(qualityLoc, qualityValue);
			glUniform1f(zoomLoc, zoomValue);
			glUniform1f(movexLoc, movexValue);
			glUniform1f(moveyLoc, moveyValue);
			glBindImageTexture(0, outputTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
			glDispatchCompute(windowWidth, windowHeight, 1);
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
			
			glUseProgram(postProcessingShaderProgram);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, outputTexture);

			glUniform1i(glGetUniformLocation(postProcessingShaderProgram, "inputTexture"), 0);
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			glBindVertexArray(0);


		}
		if(pState == GLFW_PRESS && pStateOld == GLFW_RELEASE){
			prettyFlag *= -1;
			update = 1;
		}
		if(escapeState == GLFW_PRESS && escapeStateOld == GLFW_RELEASE){
			glfwSetWindowShouldClose(window, 1);
		}
		if(upState == GLFW_PRESS && upStateOld == GLFW_RELEASE){
			moveyFlag = 1.0;
			update = 1;
		}
		if(downState == GLFW_PRESS && downStateOld == GLFW_RELEASE){
			moveyFlag = -1.0;
			update = 1;
		}
		if(zState == GLFW_PRESS && zStateOld == GLFW_RELEASE){
			zoomFlag = 1.41;
			update = 1;
		}
		if(oState == GLFW_PRESS && oStateOld == GLFW_RELEASE){
			zoomFlag = 1/1.41;
			update = 1;
		}
		if(leftState == GLFW_PRESS && leftStateOld == GLFW_RELEASE){
			movexFlag = -1.0;
			update = 1;
		}
		if(rightState == GLFW_PRESS && rightStateOld == GLFW_RELEASE){
			movexFlag = 1.0;
			update = 1;
		}
		if(qState == GLFW_PRESS && qStateOld == GLFW_RELEASE){
			qualityFlag = 1.0;
			update = 1;
		}
		if(wState == GLFW_PRESS && wStateOld == GLFW_RELEASE){
			qualityFlag = -1.0;
			update = 1;
		}

		qStateOld = qState;
		wStateOld = wState;
		pStateOld = pState;
		upStateOld = upState;
		downStateOld = downState;
		leftStateOld = leftState;
		rightStateOld = rightState;
		zStateOld = zState;
		oStateOld = oState;
		escapeStateOld = escapeState;
		

		glfwSwapBuffers(window);
		
		
	}

    glDeleteProgram(computeProgram);
    glDeleteShader(computeShader);
    glDeleteProgram(postProcessingShaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteTextures(1, &outputTexture);
    glDeleteFramebuffers(1, &framebuffer);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
	glfwTerminate();
	
	return 0;
}
