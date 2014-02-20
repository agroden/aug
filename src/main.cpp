#define GLFW_DLL
#define GLM_FORCE_RADIANS

#include <array>
#include <iostream>
#include <vector>

#include <gl\glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

int main(int argc, const char* argv)
{
	////////////////////////////////////////////////////////////////////////////
	// Window Setup
	////////////////////////////////////////////////////////////////////////////
	if (!glfwInit())
	{
		std::cerr << "could not start GLFW" << std::endl;
		return 1;
	}

	GLFWwindow* win = glfwCreateWindow(1024, 768, "AUG", NULL, NULL);
	if (NULL == win)
	{
		std::cerr << "could not open window with GLFW" << std::endl;
		return 1;
	}
	glfwMakeContextCurrent(win);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 6);

	// init glew
	glewExperimental = GL_TRUE;
	glewInit();

	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	std::cout << "Renderer: " << renderer << "\n";
	std::cout << "OpenGL version supported: " << version << "\n";

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// set skybox color
	// dark blue
	glClearColor(0.0f, 0.0f, 0.06f, 0.0f);

	////////////////////////////////////////////////////////////////////////////
	// Vertext Buffers
	////////////////////////////////////////////////////////////////////////////

	// cube verticies:
	//     (-.5,.5,.5)  (.5,.5,.5)
	//       +----------+
	//      /|         /|
	//     / |        / |
	// (-.5,.5,-.5) (.5,.5,-.5)
	//   +---+------+---+
	//   |(-.5,-.5,.5) / (.5,-.5,.5)
	//   | /        | /
	//   |/         |/
	//   +----------+
	// (-.5,-.5,-.5) (.5,-.5,-.5)
	std::array<float, 3*8> cube = {
		-0.5, -0.5, -0.5, // front
		-0.5, 0.5, -0.5,
		0.5, 0.5, -0.5,
		0.5, -0.5, -0.5,
		-0.5, -0.5, 0.5, // back
		-0.5, 0.5, 0.5,
		0.5, 0.5, 0.5,
		0.5, -0.5, 0.5
	};

	// cube vertex indicies:
	//        5         6
	//       +---------+
	//      /|        /|
	//     / |       / |
	//  1 /  |    2 /  |
	//   +---+-----+---+
	//   |  / 4    |  / 7
	//   | /       | /
	//   |/        |/
	//   +---------+
	//  0         3
	std::array<unsigned char, 2 * 3 * 6> cube_idx = {
		0, 1, 2, // front
		0, 2, 3,
		4, 5, 1, // left
		4, 1, 0,
		3, 2, 6, // right
		3, 6, 7,
		7, 6, 5, // back
		7, 5, 4,
		1, 5, 2, // top
		1, 6, 2,
		4, 0, 3, // bottom
		4, 3, 7
	};

	// vertex buffer object
	unsigned int vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, cube.size() * sizeof(float),
		cube.data(), GL_STATIC_DRAW);

	// vertex attribute object
	unsigned int vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	// index buffer
	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		cube_idx.size() * sizeof(unsigned char), cube_idx.data(),
		GL_STATIC_DRAW);

	////////////////////////////////////////////////////////////////////////////
	// Shaders
	////////////////////////////////////////////////////////////////////////////
	const char* vertex_shader =
		"#version 400\n\
		layout(location = 0) in vec3 vp;\n\
		uniform mat4 mvp;\n\
		void main() {\n\
			\tgl_Position = mvp * vec4(vp, 1);\n\
		}";

	const char* gold_fs =
		"#version 400\n\
		out vec4 color;\n\
		void main() {\n\
			\tcolor = vec4(1.0, 0.85, 0, 0.0);\n\
		}";
	unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertex_shader, NULL);
	glCompileShader(vs);
	unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &gold_fs, NULL);
	glCompileShader(fs);
	unsigned int shader_prog = glCreateProgram();
	glAttachShader(shader_prog, fs);
	glAttachShader(shader_prog, vs);
	glLinkProgram(shader_prog);
	glDeleteShader(vs);
	glDeleteShader(fs);

	// check shader program
	GLint res = GL_FALSE;
	int loglen = 0;
	glGetProgramiv(shader_prog, GL_LINK_STATUS, &res);
	glGetProgramiv(shader_prog, GL_INFO_LOG_LENGTH, &loglen);
	if (loglen > 0)
	{
		std::vector<char> errmsg(loglen + 1);
		glGetProgramInfoLog(shader_prog, loglen, NULL, &errmsg[0]);
		printf("%s\n", &errmsg[0]);
	}
	
	////////////////////////////////////////////////////////////////////////////
	// Transformation matricies
	////////////////////////////////////////////////////////////////////////////
	// 45 degree field of view, 4:3 ratio, display range, 0.1 unit <-> 100 units
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), 
		4.0f / 3.0f, 0.1f, 100.0f);
	glm::mat4 view = glm::lookAt(
		glm::vec3(1.6, 1, 1.5), // camera position
		glm::vec3(0, 0, 0), // where to look
		glm::vec3(0, 1, 0)); // rightside up
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 mvp = projection * view * model;
	// pass mvp to shader program
	GLuint matrix_id = glGetUniformLocation(shader_prog, "mvp");
	glUniformMatrix4fv(matrix_id, 1, GL_FALSE, &mvp[0][0]);

	////////////////////////////////////////////////////////////////////////////
	// Drawing Loop
	////////////////////////////////////////////////////////////////////////////
	while (!glfwWindowShouldClose(win))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glUseProgram(shader_prog);
		glUniformMatrix4fv(matrix_id, 1, GL_FALSE, &mvp[0][0]);
		glBindVertexArray(vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glDrawElements(GL_TRIANGLES, cube_idx.size(), GL_UNSIGNED_BYTE, 0);

		// required for glfw event processing
		glfwPollEvents();
		glfwSwapBuffers(win);
	}

	////////////////////////////////////////////////////////////////////////////
	// Cleanup
	////////////////////////////////////////////////////////////////////////////
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
	glDeleteProgram(shader_prog);
	glDeleteVertexArrays(1, &vao);

	glfwTerminate();
	return 0;
}