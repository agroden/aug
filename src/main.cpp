#define GLFW_DLL
#define GLM_FORCE_RADIANS

#include <array>
#include <iostream>
#include <vector>

#include <gl_core_3_3.hpp>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

int main(int argc, const char* argv[])
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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 6);

	// init glLoadGen
	gl::exts::LoadTest loaded = gl::sys::LoadFunctions();
	if(!loaded)
	{
		std::cerr << "could not init opengl functions" << std::endl;
		return 1;
	}

	const GLubyte* renderer = gl::GetString(gl::RENDERER);
	const GLubyte* version = gl::GetString(gl::VERSION);
	std::cout << "Renderer: " << renderer << "\n";
	std::cout << "OpenGL version supported: " << version << "\n";

	gl::Enable(gl::DEPTH_TEST);
	gl::DepthFunc(gl::LESS);

	// set skybox color
	// dark blue
	gl::ClearColor(0.0f, 0.0f, 0.06f, 0.0f);

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
	std::array<float, (3*8)> cube = {{
		-0.5, -0.5, -0.5, // front
		-0.5, 0.5, -0.5,
		0.5, 0.5, -0.5,
		0.5, -0.5, -0.5,
		-0.5, -0.5, 0.5, // back
		-0.5, 0.5, 0.5,
		0.5, 0.5, 0.5,
		0.5, -0.5, 0.5
	}};

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
	std::array<unsigned char, (2*3*6)> cube_idx = {{
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
	}};

	// vertex buffer object
	unsigned int vbo = 0;
	gl::GenBuffers(1, &vbo);
	gl::BindBuffer(gl::ARRAY_BUFFER, vbo);
	gl::BufferData(gl::ARRAY_BUFFER, cube.size() * sizeof(float),
		cube.data(), gl::STATIC_DRAW);

	// vertex attribute object
	unsigned int vao = 0;
	gl::GenVertexArrays(1, &vao);
	gl::BindVertexArray(vao);
	gl::EnableVertexAttribArray(0);
	gl::BindBuffer(gl::ARRAY_BUFFER, vbo);
	gl::VertexAttribPointer(0, 3, gl::FLOAT, false, 0, NULL);

	// index buffer
	unsigned int ibo;
	gl::GenBuffers(1, &ibo);
	gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, ibo);
	gl::BufferData(gl::ELEMENT_ARRAY_BUFFER,
		cube_idx.size() * sizeof(unsigned char), cube_idx.data(),
		gl::STATIC_DRAW);

	////////////////////////////////////////////////////////////////////////////
	// Shaders
	////////////////////////////////////////////////////////////////////////////
	//layout(location = 0) 
	const char* vertex_shader =
		"#version 330 core\n\
		layout(location = 0) in vec3 vp;\n\
		uniform mat4 mvp;\n\
		void main() {\n\
			\tgl_Position = mvp * vec4 (vp, 1);\n\
		}";

	const char* gold_fs =
		"#version 330 core\n\
		out vec4 color;\n\
		void main() {\n\
			\tcolor = vec4(1.0, 0.85, 0, 0.0);\n\
		}";
	
	GLint res = gl::FALSE_;
	int loglen = 0;
	
	unsigned int vs = gl::CreateShader(gl::VERTEX_SHADER);
	gl::ShaderSource(vs, 1, &vertex_shader, NULL);
	gl::CompileShader(vs);
	gl::GetShaderiv(vs, gl::COMPILE_STATUS, &res);
	if (res == gl::FALSE_)
	{
		gl::GetShaderiv(vs, gl::INFO_LOG_LENGTH, &loglen);
		if (loglen > 0)
		{
			std::vector<char> errmsg(loglen + 1);
			gl::GetShaderInfoLog(vs, loglen, NULL, &errmsg[0]);
			std::cerr << "VERTEX SHADER: " << &errmsg[0] << "\n";
			return 1;
		}
	}
	
	res = gl::FALSE_;
	unsigned int fs = gl::CreateShader(gl::FRAGMENT_SHADER);
	gl::ShaderSource(fs, 1, &gold_fs, NULL);
	gl::CompileShader(fs);
	gl::GetShaderiv(fs, gl::COMPILE_STATUS, &res);
	if (res == gl::FALSE_)
	{
		gl::GetShaderiv(fs, gl::INFO_LOG_LENGTH, &loglen);
		if (loglen > 0)
		{
			std::vector<char> errmsg(loglen + 1);
			gl::GetShaderInfoLog(fs, loglen, NULL, &errmsg[0]);
			std::cerr << "FRAGMENT SHADER: " << &errmsg[0] << "\n";
			return 1;
		}
	}
	
	unsigned int shader_prog = gl::CreateProgram();
	gl::AttachShader(shader_prog, fs);
	gl::AttachShader(shader_prog, vs);
	gl::LinkProgram(shader_prog);
	gl::DeleteShader(vs);
	gl::DeleteShader(fs);

	// check shader program
	res = gl::FALSE_;
	gl::GetProgramiv(shader_prog, gl::LINK_STATUS, &res);
	if (res == gl::FALSE_)
	{
		gl::GetProgramiv(shader_prog, gl::INFO_LOG_LENGTH, &loglen);
		if (loglen > 0)
		{
			std::vector<char> errmsg(loglen + 1);
			gl::GetProgramInfoLog(shader_prog, loglen, NULL, &errmsg[0]);
			std::cerr << "SHADER PROGRAM: " << &errmsg[0] << "\n";
			return 1;
		}
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
	GLuint matrix_id = gl::GetUniformLocation(shader_prog, "mvp");
	gl::UniformMatrix4fv(matrix_id, 1, false, &mvp[0][0]);

	////////////////////////////////////////////////////////////////////////////
	// Drawing Loop
	////////////////////////////////////////////////////////////////////////////
	while (!glfwWindowShouldClose(win))
	{
		gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);
		gl::PolygonMode(gl::FRONT_AND_BACK, gl::LINE);
		gl::UseProgram(shader_prog);
		gl::UniformMatrix4fv(matrix_id, 1, false, &mvp[0][0]);
		gl::BindVertexArray(vao);
		gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, ibo);
		gl::DrawElements(gl::TRIANGLES, cube_idx.size(), gl::UNSIGNED_BYTE, 0);

		// required for glfw event processing
		glfwPollEvents();
		glfwSwapBuffers(win);
	}

	////////////////////////////////////////////////////////////////////////////
	// Cleanup
	////////////////////////////////////////////////////////////////////////////
	gl::DeleteBuffers(1, &vbo);
	gl::DeleteBuffers(1, &ibo);
	gl::DeleteProgram(shader_prog);
	gl::DeleteVertexArrays(1, &vao);

	glfwTerminate();
	return 0;
}