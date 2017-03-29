#include "dos.h"
#include "dos_scene.h"
#include "tree.h"

using namespace dos;

template<class TreeNodeType>
void buildBalancedTree(Tree<TreeNodeType>& outTree, int levels, int childNodesPerLevel)
{
	std::vector<std::shared_ptr<TreeNodeType>> level;
	level.push_back(outTree.root);

	for (int i = 0; i < levels; ++i)
	{
		std::vector<std::shared_ptr<TreeNodeType>> nextLevel;

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

template<class TScene>
TScene buildBalancedScene(int levels, int childNodesPerLevel)
{
	TScene scene;

	std::vector<TransformID> level;
	level.push_back(scene.getRoot());

	for (int i = 0; i < levels; ++i)
	{
		std::vector<TransformID> nextLevel;

		for (TransformID id : level)
		{
			for (int child = 0; child < childNodesPerLevel; ++child)
			{
				nextLevel.push_back(scene.addTransform(id));
			}
		}

		level = nextLevel;
	}

	return scene;
}

int nodeCount = 0;

int main(int argc, char** argv)
{
	int levels = 10;
	int children = 3;

	int numNodes = (std::pow(children, levels + 1) - 1) / (children - 1);
	int numTests = 1000;

	check(numNodes < MAX_ENTITIES);

	// Split test
	printf("\n\nSPLIT TEST\n");

	Scene scene = buildBalancedScene<Scene>(levels, children);

	using ms = std::chrono::duration<float, std::milli>;
	using time = std::chrono::time_point<std::chrono::steady_clock>;
	std::chrono::high_resolution_clock timer;

	time start;
	time end;

	float deltaTime = 0.f;
	float totalRuntime = 0.f;

	for (int i = 0; i < numTests; ++i)
	{
		start = timer.now();

		scene.updateWorldTransforms();

		end = timer.now();
		deltaTime = std::chrono::duration_cast<ms>(end - start).count();
		totalRuntime += deltaTime;
	}

	printf("Number of nodes: %d\n", scene.nextFree);
	printf("Over %d samples, update took: %f ms on avg\n", numTests, totalRuntime / numTests);
	
	// Tree Depth First test
	Tree<TransformNode> tree;

	buildBalancedTree(tree, levels, children);

	printf("\n\nTREE DEPTH FIRST TEST\n");

	deltaTime = 0.f;
	totalRuntime = 0.f;

	for (int i = 0; i < numTests; ++i)
	{
		start = timer.now();

		updateWorldTransformsDFS(tree);

		end = timer.now();
		deltaTime = std::chrono::duration_cast<ms>(end - start).count();
		totalRuntime += deltaTime;
	}

	printf("Number of nodes: %d\n", numNodes);
	printf("Over %d samples, update took: %f ms on avg\n", numTests, totalRuntime / numTests);	

	return 0;
}