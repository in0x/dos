#include "dos.h"
#include "scene.h"
#include "tree.h"
#include "shader.h"

int glMain()
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
	auto viewMat = HMM_LookAt({ 0, 0, 30 }, { 0,0,0 }, { 0,1,0 }); // eye, center, up
	auto projMat = HMM_Perspective(40, (float)windowWidth / (float)windowHeight, 1, 100); // float FOV, float AspectRatio, float Near, float Far;

	using ms = std::chrono::duration<float, std::milli>;
	using time_t = std::chrono::time_point<std::chrono::steady_clock>;

	std::chrono::high_resolution_clock timer;
	c_time start;
	c_time end;

	int levels = 14;
	int children = 2;

	SceneTree tree = SceneTree::buildBalancedTree(levels, children);
	Scene scene;
	scene.buildFromSceneTree(tree);

	int numNodes = (std::pow(children, levels + 1) - 1) / (children - 1);
	printf("Nodes(#) = %d\n", numNodes);

	hmm_frustum frustum(projMat, viewMat);

	float totalTime = 0.f;
	float frames = 100;
	float currentFrame = 0;
	srand(time(0));

	while (!glfwWindowShouldClose(window))
	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_MULTISAMPLE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		GLfloat color[] = { 1.f, 1.f, 1.f, 1.f };
		glClearBufferfv(GL_COLOR, 0, color);
		glUseProgram(progHandle);

		glBindVertexArray(vao);
		glUniform3f(2, 0.f, 1.f, -3.f);
		glUniformMatrix4fv(4, 1, GL_FALSE, (GLfloat*)&viewMat.Elements);
		glUniformMatrix4fv(5, 1, GL_FALSE, (GLfloat*)&projMat.Elements);

		start = timer.now();

		scene.updateWorldTransforms();
		scene.updateWorldBounds();
		scene.cullSceneHierarchical(frustum);

		for (size_t i = 0; i < scene.nextFree; ++i)
		{
			if (scene.bVisible[i])
			{
				glUniformMatrix4fv(3, 1, GL_FALSE, (GLfloat*)&(scene.world[i]));
				glDrawArrays(GL_TRIANGLES, 0, vertices.size());
			}
		}

		end = timer.now();
		float deltaTime = std::chrono::duration_cast<ms>(end - start).count();
		totalTime += deltaTime;

		glfwSwapBuffers(window);

		glDisable(GL_MULTISAMPLE);
		glDisable(GL_DEPTH_TEST);

		glfwPollEvents();

		currentFrame++;

		if (currentFrame >= frames)
			break;
	}

	printf("Result: %f ms on avg per frame\n", totalTime / frames);

	glfwDestroyWindow(window);
	glDeleteProgram(progHandle);
	glfwTerminate();

	return 0;
}

std::vector<float> sceneResults;
std::vector<float> treeResults;

void testScene(int levels, int children, int samples, const hmm_frustum& frustum, std::ofstream& outFile)
{
	printf("\n\nSPLIT DF TEST\n");
	int numNodes = (std::pow(children, levels + 1) - 1) / (children - 1);

	std::chrono::high_resolution_clock timer;

	c_time start;
	c_time end;

	float deltaTime = 0.f;
	float totalRuntime = 0.f;

	Scene scene;
	SceneTree tree;

	tree = SceneTree::buildBalancedTree(levels, children);
	scene.buildFromSceneTree(tree);

	for (int i = 0; i < samples; ++i)
	{
		start = timer.now();

		scene.updateWorldTransforms();
		scene.updateWorldBounds();
		scene.cullSceneHierarchical(frustum);
		scene.render();

		end = timer.now();
		deltaTime = std::chrono::duration_cast<ms>(end - start).count();
		totalRuntime += deltaTime;
	}

	float result = totalRuntime / samples;

	//printf("Number of nodes: %d\n", numNodes);
	//printf("Over %d samples, update took: %f ms on avg\n", samples, result);

	outFile << numNodes << "," << result << '\n';
	sceneResults.push_back(result);

	// Tree Depth First test
}

void testTree(int levels, int children, int samples, const hmm_frustum& frustum, std::ofstream& outFile)
{
	printf("\n\nTREE DEPTH FIRST TEST\n");
	int numNodes = (std::pow(children, levels + 1) - 1) / (children - 1);

	SceneTree transformTree = SceneTree::buildBalancedTree(levels, children);

	std::chrono::high_resolution_clock timer;

	c_time start;
	c_time end;

	float deltaTime = 0.f;
	float totalRuntime = 0.f;

	for (int i = 0; i < samples; ++i)
	{
		start = timer.now();

		transformTree.updateWorldTransformsDFS();
		transformTree.cullSceneTreeHierarchical(frustum);
		transformTree.renderTree();

		end = timer.now();
		deltaTime = std::chrono::duration_cast<ms>(end - start).count();
		totalRuntime += deltaTime;
	}

	float result = totalRuntime / samples;

	//printf("Number of nodes: %d\n", numNodes);
	//printf("Over %d samples, update took: %f ms on avg\n", samples, result);

	outFile << numNodes << "," << result << '\n';
	treeResults.push_back(result);
}

int benchMain()
{
	int levels = 18;
	int children = 2;

	int numNodes = (std::pow(children, levels + 1) - 1) / (children - 1);
	int samples = 100;

	printf("Numnodes: %i", numNodes);
	check(numNodes < MAX_ENTITIES);

	hmm_mat4 viewMat = HMM_LookAt(HMM_Vec3(0, 0, -10), HMM_Vec3(0, 0, 0), HMM_Vec3(0, 1, 0));
	hmm_mat4 projMat = HMM_Perspective(60, 1280.f / 720.f, 1, 100);
	hmm_frustum frustum(projMat, viewMat);

	std::ofstream scene_csv;
	std::ofstream tree_csv;

	scene_csv.open("../bin/scene.csv");
	tree_csv.open("../bin/tree.csv");

	scene_csv << "Scene Nodes, Scene Time\n";
	tree_csv << "Tree Nodes, Tree Time\n";

	for (int levels_test = 1; levels_test <= levels; ++levels_test)
	{
		testScene(levels_test, children, samples, frustum, scene_csv);
		testTree(levels_test, children, samples, frustum, tree_csv);
	}

	std::size_t count = sceneResults.size();
	float total = 0.0f;
	for (std::size_t i = 0; i < count; ++i)
	{
		total += /*1.0f -*/ (treeResults[i] / sceneResults[i]);
	}
	printf("Average relative performance: %f \n", total / levels);

	return 0;
}

int main(int argc, char** argv)
{
	return glMain();
	//return benchMain();
}