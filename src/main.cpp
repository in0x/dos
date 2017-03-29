#include "dos.h"
#include "scene.h"
#include "tree.h"

void buildBalancedTree(SceneTree& outTree, int levels, int childNodesPerLevel)
{
	std::vector<std::shared_ptr<TransformNode>> level;
	level.push_back(outTree.root);

	for (int i = 0; i < levels; ++i)
	{
		std::vector<std::shared_ptr<TransformNode>> nextLevel;

		for (auto& node : level)
		{
			for (int child = 0; child < childNodesPerLevel; ++child)
			{
				nextLevel.push_back(outTree.addNode(node));
			}
		}

		level = nextLevel;
	}
}

Scene buildBalancedScene(int levels, int childNodesPerLevel)
{
	Scene scene;

	std::vector<TransformID> level;
	level.push_back(scene.getRoot());

	for (int i = 0; i < levels; ++i)
	{
		std::vector<TransformID> nextLevel;

		for (TransformID id : level)
		{
			for (int child = 0; child < childNodesPerLevel; ++child)
			{
				nextLevel.push_back(scene.addTransform());
			}
		}

		level = nextLevel;
	}

	return scene;
}

int nodeCount = 0;

void visitSceneDF(Scene& scene, std::shared_ptr<TransformNode> node, int parentIdx)
{
	nodeCount += 1;

	for (auto& child : node->children)
	{
		scene.parents[nodeCount] = parentIdx;
		visitSceneDF(scene, child, nodeCount);
	}
}

void buildDepthFirstScene(Scene& outScene, SceneTree& outTree, int levels, int childNodesPerLevel)
{
	buildBalancedTree(outTree, levels, childNodesPerLevel);

	int numNodes = (std::pow(childNodesPerLevel, levels + 1) - 1) / (childNodesPerLevel - 1);
	outScene.resize(numNodes);
	nodeCount = 0;
	visitSceneDF(outScene, outTree.root, 0);
}

int main(int argc, char** argv)
{
	int levels = 5;
	int children = 8;

	int numNodes = (std::pow(children, levels + 1) - 1) / (children - 1);
	int numTests = 100;

	printf("Numnodes: %i", numNodes);
	check(numNodes < MAX_ENTITIES);

	hmm_mat4 projMat = HMM_Perspective(60, 1280.f / 720.f, 1, 100); 
	hmm_frustum frustum(projMat);

	// Split test
	printf("\n\nSPLIT DF TEST\n");

	using ms = std::chrono::duration<float, std::milli>;
	using time = std::chrono::time_point<std::chrono::steady_clock>;
	std::chrono::high_resolution_clock timer;

	time start;
	time end;

	float deltaTime = 0.f;
	float totalRuntime = 0.f;

	Scene scene;
	SceneTree tree;

	buildDepthFirstScene(scene, tree, levels, children);

	for (int i = 0; i < numTests; ++i)
	{
		start = timer.now();

		scene.updateWorldTransforms();
		//scene.cullScene(frustum);
		scene.cullSceneHierarchical(frustum);
		scene.render();

		end = timer.now();
		deltaTime = std::chrono::duration_cast<ms>(end - start).count();
		totalRuntime += deltaTime;
	}

	printf("Number of nodes: %d\n", scene.nextFree);
	printf("Over %d samples, update took: %f ms on avg\n", numTests, totalRuntime / numTests);
	
	// Tree Depth First test
	SceneTree transformTree;

	buildBalancedTree(transformTree, levels, children);

	printf("\n\nTREE DEPTH FIRST TEST\n");

	deltaTime = 0.f;
	totalRuntime = 0.f;

	for (int i = 0; i < numTests; ++i)
	{
		start = timer.now();

		transformTree.updateWorldTransformsDFS();
		//transformTree.cullSceneTree(frustum);
		transformTree.cullSceneTreeHierarchical(frustum);
		transformTree.renderTree();

		end = timer.now();
		deltaTime = std::chrono::duration_cast<ms>(end - start).count();
		totalRuntime += deltaTime;
	}

	printf("Number of nodes: %d\n", numNodes);
	printf("Over %d samples, update took: %f ms on avg\n", numTests, totalRuntime / numTests);	

	return 0;
}

/*
If we use Depth First Order, im pretty sure i can easily do hierarchical culling aswell, the algorithm would be as follows

If not in bounding volume
	- Remember current parent value
	- Keep iterating until parent value smaller than current value is encountered
	- This has to be a new subtree since down a subtree the parent index always has to increase when DFS

This becomes even easier with a fixed number of children since you just have to jump to the next complete offset

*/