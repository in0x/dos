//#pragma once
//
//#include "dos.h"
//#include "dos_scene.h"
//#include "shader.h"
//
//std::vector<hmm_vec3> vertices = {
//	// bottom
//	{ 0.5f, -0.5f, 0.5f },
//	{ -0.5f, -0.5f, 0.5f },
//	{ 0.5f, -0.5f, -0.5f },
//
//	{ -0.5f, -0.5f, -0.5f },
//	{ -0.5f, -0.5f, 0.5f },
//	{ 0.5f, -0.5f, -0.5f },
//
//	// top 
//	{ 0.5f, 0.5f, 0.5f },
//	{ 0.5f, 0.5f, -0.5f },
//	{ -0.5f, 0.5f, 0.5f },
//
//	{ -0.5f, 0.5f, -0.5f },
//	{ -0.5f, 0.5f, 0.5f },
//	{ 0.5f, 0.5f, -0.5f },
//
//	// front
//	{ -0.5f, -0.5f, 0.5f },
//	{ 0.5f, -0.5f, 0.5f },
//	{ 0.5f, 0.5f, 0.5f },
//
//	{ -0.5f, -0.5f, 0.5f },
//	{ 0.5f, 0.5f, 0.5f },
//	{ -0.5f, 0.5f, 0.5f },
//
//	// left
//	{ -0.5f, -0.5f,  0.5f },
//	{ -0.5f, 0.5f,  0.5f },
//	{ -0.5f, -0.5f, -0.5f },
//
//	{ -0.5f, -0.5f, -0.5f },
//	{ -0.5f, 0.5f,  0.5f },
//	{ -0.5f, 0.5f, -0.5f },
//
//	// right
//	{ 0.5f, -0.5f, 0.5f },
//	{ 0.5f, -0.5f, -0.5f },
//	{ 0.5f, 0.5f, -0.5f },
//
//	{ 0.5f, -0.5f,  0.5f },
//	{ 0.5f, 0.5f, -0.5f },
//	{ 0.5f, 0.5f,  0.5f },
//	// back
//	{ 0.5f, -0.5f, -0.5f },
//	{ -0.5f, -0.5f, -0.5f },
//	{ -0.5f, 0.5f, -0.5f },
//
//	{ 0.5f, -0.5f, -0.5f },
//	{ -0.5f, 0.5f, -0.5f },
//	{ 0.5f, 0.5f, -0.5f }
//};
//
//std::vector<hmm_vec3> normals = {
//	{ 0,-1, 0 },{ 0,-1, 0 },{ 0,-1, 0 },{ 0,-1, 0 },{ 0,-1, 0 },{ 0,-1, 0 },
//	{ 0, 1, 0 },{ 0, 1, 0 },{ 0, 1, 0 },{ 0, 1, 0 },{ 0, 1, 0 },{ 0, 1, 0 },
//	{ 0, 0, -1 },{ 0, 0, -1 },{ 0, 0, -1 },{ 0, 0, -1 },{ 0, 0, -1 },{ 0, 0, -1 },
//	{ -1, 0, 0 },{ -1, 0, 0 },{ -1, 0, 0 },{ -1, 0, 0 },{ -1, 0, 0 },{ -1, 0, 0 },
//	{ 1, 0, 0 },{ 1, 0, 0 },{ 1, 0, 0 },{ 1, 0, 0 },{ 1, 0, 0 },{ 1, 0, 0 },
//	{ 0, 0, -1 },{ 0, 0, -1 },{ 0, 0, -1 },{ 0, 0, -1 },{ 0, 0, -1 },{ 0, 0, -1 }
//};
//
//#if 0 int main(int argc, char** argv)
//{
//	if (!glfwInit())
//	{
//		printf("Failed to init GLFW\n");
//	}
//	else
//	{
//		printf("Inited GLFW\n");
//	}
//
//	int windowWidth = 1280;
//	int windowHeight = 720;
//
//	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "DOS", nullptr, nullptr);
//	if (!window)
//	{
//		printf("Failed to open window\n");
//	}
//
//	glfwMakeContextCurrent(window);
//
//	if (gl3wInit())
//	{
//		printf("Failed to init glew");
//	}
//
//	GLuint vert = createShader("../src/shader.vert", GL_VERTEX_SHADER);
//	GLuint frag = createShader("../src/shader.frag", GL_FRAGMENT_SHADER);
//
//	GLuint progHandle = glCreateProgram();
//
//	glAttachShader(progHandle, vert);
//	glAttachShader(progHandle, frag);
//
//	glLinkProgram(progHandle);
//
//	glDeleteShader(vert);
//	glDeleteShader(frag);
//
//	if (progHandle == 0)
//	{
//		printf("Failed to create program\n");
//	}
//
//	GLuint vbo;
//	GLuint normals_vbo;
//	GLuint vao;
//
//	glGenBuffers(1, &vbo);
//	glBindBuffer(GL_ARRAY_BUFFER, vbo);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(hmm_vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
//
//	glGenBuffers(1, &normals_vbo);
//	glBindBuffer(GL_ARRAY_BUFFER, normals_vbo);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(hmm_vec3) * normals.size(), normals.data(), GL_STATIC_DRAW);
//
//	glGenVertexArrays(1, &vao);
//	glBindVertexArray(vao);
//
//	glEnableVertexAttribArray(0);
//	glBindBuffer(GL_ARRAY_BUFFER, vbo);
//	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(hmm_vec3), nullptr);
//
//	glEnableVertexAttribArray(1);
//	glBindBuffer(GL_ARRAY_BUFFER, normals_vbo);
//	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(hmm_vec3), nullptr);
//
//	glEnable(GL_CULL_FACE);
//
//	auto worldMat = HMM_Mat4_Identity();
//	auto viewMat = HMM_LookAt({ 0, 2, -15 }, { 0,0,0 }, { 0,0,1 }); // eye, center, up
//	auto projMat = HMM_Perspective(60, (float)windowWidth / (float)windowHeight, 1, 1000); // float FOV, float AspectRatio, float Near, float Far;
//
//	using ms = std::chrono::duration<float, std::milli>;
//	using time = std::chrono::time_point<std::chrono::steady_clock>;
//
//	std::chrono::high_resolution_clock timer;
//
//	time lastFrametime = timer.now();
//
//	Scene scene;
//
//	hmm_mat4 offset = HMM_Translate(hmm_vec3{ -2.f, 0, 0 });
//	auto N1 = scene.addTransform(HMM_Translate(hmm_vec3{ -2.f, 2, 0 }));
//	auto N2 = scene.addTransform(offset);
//	auto N3 = scene.addTransform(N1, offset);
//	auto N4 = scene.addTransform(N1, offset);
//	auto N5 = scene.addTransform(HMM_Translate(hmm_vec3{ -2.f, -2, 0 }));
//	auto N6 = scene.addTransform(N3, offset);
//	auto N7 = scene.addTransform(N5, offset);
//	auto N8 = scene.addTransform(N2, offset);
//
//	while (!glfwWindowShouldClose(window))
//	{
//		auto nowFrameTime = timer.now();
//		auto deltaTime = std::chrono::duration_cast<ms>(nowFrameTime - lastFrametime).count();
//		lastFrametime = nowFrameTime;
//
//		glEnable(GL_DEPTH_TEST);
//		glEnable(GL_MULTISAMPLE);
//
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//		GLfloat color[] = { 1.f, 1.f, 1.f, 1.f };
//		glClearBufferfv(GL_COLOR, 0, color);
//
//		glUseProgram(progHandle);
//
//		hmm_mat4 rotMat = HMM_Rotate(deltaTime * 0.05f, { 0,1,0 });
//		/*worldMat = worldMat * rotMat;*/
//		auto root = scene.getRoot();
//		scene.local[root.index] = scene.local[root.index] * rotMat;
//
//		glBindVertexArray(vao);
//
//		glUniform3f(2, 0.f, 1.f, -3.f);
//		//glUniformMatrix4fv(3, 1, GL_FALSE, (GLfloat*)&worldMat.Elements);
//		glUniformMatrix4fv(4, 1, GL_FALSE, (GLfloat*)&viewMat.Elements);
//		glUniformMatrix4fv(5, 1, GL_FALSE, (GLfloat*)&projMat.Elements);
//
//		scene.updateWorldTransforms();
//
//		for (size_t i = 0; i < scene.nextFree; ++i)
//		{
//			glUniformMatrix4fv(3, 1, GL_FALSE, (GLfloat*)&(scene.world[i]));
//			glDrawArrays(GL_TRIANGLES, 0, vertices.size());
//		}
//
//		glfwSwapBuffers(window);
//
//		glDisable(GL_MULTISAMPLE);
//		glDisable(GL_DEPTH_TEST);
//
//		glfwPollEvents();
//	}
//
//	glfwDestroyWindow(window);
//	glDeleteProgram(progHandle);
//	glfwTerminate();
//
//	return 0;
//}
//
//#endif