#include "dos.h"
#include "scene.h"
#include "tree.h"
#include "shader.h"

//TODO(Phil): sphere needs vec4 center, otherwise we dont have translations

int main(int argc, char** argv)
{
	if (!glfwInit())
	{
		printf("Failed to init GLFW\n");
	}
	else
	{
		printf("Inited GLFW\n");
	}

	int windowWidth = 1280;
	int windowHeight = 720;

	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "DOS", nullptr, nullptr);
	if (!window)
	{
		printf("Failed to open window\n");
	}

	glfwMakeContextCurrent(window);

	if (gl3wInit())
	{
		printf("Failed to init glew");
	}

	GLuint vert = createShader("../src/shader.vert", GL_VERTEX_SHADER);
	GLuint frag = createShader("../src/shader.frag", GL_FRAGMENT_SHADER);

	GLuint progHandle = glCreateProgram();

	glAttachShader(progHandle, vert);
	glAttachShader(progHandle, frag);

	glLinkProgram(progHandle);

	glDeleteShader(vert);
	glDeleteShader(frag);

	if (progHandle == 0)
	{
		printf("Failed to create program\n");
	}

	GLuint vbo;
	GLuint normals_vbo;
	GLuint vao;

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(hmm_vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &normals_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, normals_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(hmm_vec3) * normals.size(), normals.data(), GL_STATIC_DRAW);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(hmm_vec3), nullptr);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, normals_vbo);
	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(hmm_vec3), nullptr);

	glEnable(GL_CULL_FACE);

	auto worldMat = HMM_Mat4_Identity();
	auto viewMat = HMM_LookAt({ 0, 0, -30 }, { 0,0,0 }, { 0,1,0 }); // eye, center, up
	auto projMat = HMM_Perspective(60, (float)windowWidth / (float)windowHeight, 1, 100); // float FOV, float AspectRatio, float Near, float Far;

	using ms = std::chrono::duration<float, std::milli>;
	using time_t = std::chrono::time_point<std::chrono::steady_clock>;

	std::chrono::high_resolution_clock timer;

	time_t lastFrametime = timer.now();

	SceneTree tree = SceneTree::buildBalancedTree(2, 2);
	Scene scene;
	scene.buildFromSceneTree(tree);

	hmm_frustum frustum(projMat, viewMat);

	float totalTime = 0.f;
	srand(time(0));

	while (!glfwWindowShouldClose(window))
	{
		auto nowFrameTime = timer.now();
		auto deltaTime = std::chrono::duration_cast<ms>(nowFrameTime - lastFrametime).count() / 1000.f;
		lastFrametime = nowFrameTime;
		totalTime += deltaTime;

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_MULTISAMPLE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		GLfloat color[] = { 1.f, 1.f, 1.f, 1.f };
		glClearBufferfv(GL_COLOR, 0, color);
		glUseProgram(progHandle);

		hmm_mat4 transMat = HMM_Translate(HMM_Vec3(0, HMM_SinF(totalTime), 0));
		hmm_mat4 rotMat = HMM_Rotate(deltaTime * 100.f, { 0,1,0 });

		auto root = scene.getRoot();
		scene.local[root.index] = transMat;

		//int randomIdx = rand() % scene.nextFree;
		//scene.local[4] = scene.local[4] * rotMat;

		glBindVertexArray(vao);
		glUniform3f(2, 0.f, 1.f, -3.f);
		glUniformMatrix4fv(4, 1, GL_FALSE, (GLfloat*)&viewMat.Elements);
		glUniformMatrix4fv(5, 1, GL_FALSE, (GLfloat*)&projMat.Elements);

		scene.updateWorldTransforms();
		scene.updateWorldBounds();
		//scene.cullScene(frustum);
		scene.cullSceneHierarchical(frustum);

		int nodesDrawn = 0;
		for (size_t i = 0; i < scene.nextFree; ++i)
		{
			if (scene.bVisible[i])
			{
				glUniformMatrix4fv(3, 1, GL_FALSE, (GLfloat*)&(scene.world[i]));
				glDrawArrays(GL_TRIANGLES, 0, vertices.size());
				nodesDrawn++;
			}
		}
		printf("Nodes sent to render: %d\n", nodesDrawn);

		glfwSwapBuffers(window);

		glDisable(GL_MULTISAMPLE);
		glDisable(GL_DEPTH_TEST);

		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glDeleteProgram(progHandle);
	glfwTerminate();

	return 0;
}


//int main(int argc, char** argv)
//{
//	int levels = 5;
//	int children = 8;
//
//	int numNodes = (std::pow(children, levels + 1) - 1) / (children - 1);
//	int numTests = 100;
//
//	printf("Numnodes: %i", numNodes);
//	check(numNodes < MAX_ENTITIES);
//
//	hmm_mat4 projMat = HMM_Perspective(60, 1280.f / 720.f, 1, 100); 
//	hmm_frustum frustum(projMat);
//
//	// Split test
//	printf("\n\nSPLIT DF TEST\n");
//
//	using ms = std::chrono::duration<float, std::milli>;
//	using time = std::chrono::time_point<std::chrono::steady_clock>;
//	std::chrono::high_resolution_clock timer;
//
//	time start;
//	time end;
//
//	float deltaTime = 0.f;
//	float totalRuntime = 0.f;
//
//	Scene scene;
//	SceneTree tree;
//
//	buildBalancedTree(tree,levels, children);
//	scene.buildFromSceneTree(tree);
//
//	for (int i = 0; i < numTests; ++i)
//	{
//		start = timer.now();
//
//		scene.updateWorldTransforms();
//		scene.cullSceneHierarchical(frustum);
//		scene.render();
//
//		end = timer.now();
//		deltaTime = std::chrono::duration_cast<ms>(end - start).count();
//		totalRuntime += deltaTime;
//	}
//
//	printf("Number of nodes: %d\n", scene.nextFree);
//	printf("Over %d samples, update took: %f ms on avg\n", numTests, totalRuntime / numTests);
//	
//	// Tree Depth First test
//	SceneTree transformTree;
//
//	buildBalancedTree(transformTree, levels, children);
//
//	printf("\n\nTREE DEPTH FIRST TEST\n");
//
//	deltaTime = 0.f;
//	totalRuntime = 0.f;
//
//	for (int i = 0; i < numTests; ++i)
//	{
//		start = timer.now();
//
//		transformTree.updateWorldTransformsDFS();
//		transformTree.cullSceneTreeHierarchical(frustum);
//		transformTree.renderTree();
//
//		end = timer.now();
//		deltaTime = std::chrono::duration_cast<ms>(end - start).count();
//		totalRuntime += deltaTime;
//	}
//
//	printf("Number of nodes: %d\n", numNodes);
//	printf("Over %d samples, update took: %f ms on avg\n", numTests, totalRuntime / numTests);	
//
//	return 0;
//}
