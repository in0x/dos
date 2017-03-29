#include "dos.h"
#include "dos_scene.h"
#include "combined_scene.h"
#include "tree.h"

using namespace dos;

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

template<class TScene>
TScene buildDegenWideScene(int children)
{
	TScene scene;

	for (int i = 0; i < children; ++i)
	{
		scene.AddTransform();
	}
}

template<class TScene>
TScene buildDegenDeepScene(int children)
{
	TScene scene;
	TransformID last = scene.getRoot();

	for (int i = 0; i < children; ++i)
	{
		last = scene.AddTransform(last);
	}
}

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

int nodeCount = 0;

template<class TScene, class TreeNodeType>
void buildDepthFirstScene(TScene& outScene, Tree<TreeNodeType>& outTree, int levels, int childNodesPerLevel)
{
	buildBalancedTree(outTree, levels, childNodesPerLevel);

	int numNodes = (std::pow(childNodesPerLevel, levels + 1) - 1) / (childNodesPerLevel - 1);
	outScene.resize(numNodes);
	nodeCount = 0;
	visitSceneDF(outScene, outTree.root, 0);
}

template<class TScene, class TreeNodeType>
void buildBreadthFirstScene(TScene& outScene, Tree<TreeNodeType>& outTree, int levels, int childNodesPerLevel)
{
	buildBalancedTree(outTree, levels, childNodesPerLevel);

	int numNodes = (std::pow(childNodesPerLevel, levels + 1) - 1) / (childNodesPerLevel - 1);
	outScene.resize(numNodes);
	nodeCount = 0;
	visitSceneBF(outScene, outTree.root, 0);
}

template<class TScene, class TreeNodeType>
void visitSceneDF(TScene& scene, std::shared_ptr<TreeNodeType> node, int parentIdx)
{
	nodeCount += 1;

	for (auto& child : node->children)
	{
		scene.parents[nodeCount] = parentIdx;
		visitSceneDF(scene, child, nodeCount);
	}
}

template<class TScene, class TreeNodeType>
void visitSceneBF(TScene& scene, std::shared_ptr<TreeNodeType> node, int parentIdx)
{
	for (auto& child : node->children)
	{
		nodeCount += 1;
		scene.parents[nodeCount] = parentIdx;
	}

	for (auto& child : node->children)
	{
		parentIdx += 1;
		visitSceneBF(scene, child, parentIdx);
	}
}

template<class TScene>
void TestScene(TScene& scene, int numTests)
{
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
}

int main(int argc, char** argv)
{
	int levels = 10;
	int children = 3;

	int numNodes = (std::pow(children, levels + 1) - 1) / (children - 1);
	int numTests = 1000;

	check(numNodes < MAX_ENTITIES);

	// Split test
	{
		printf("\n\nSPLIT TEST\n");

		Scene scene = buildBalancedScene<Scene>(levels, children);

		TestScene(scene, numTests);
	}

	// Combined test
	{
		printf("\n\nCombined TEST\n");

		auto combined = buildBalancedScene<hierarchy::combined::Scene>(levels, children);

		TestScene(combined, numTests);
	}

	// Depth first split test
	{
		printf("\n\nDEPTH FIRST SPLIT TEST\n");

		Scene scene;
		Tree<TransformNode> tree;

		buildDepthFirstScene(scene, tree, levels, children);
		TestScene(scene, numTests);
	}

	// Depth first combined test 
	{
		printf("\n\nDEPTH First COMBINED TEST\n");

		hierarchy::combined::Scene scene;
		Tree<TransformNode> tree;

		buildDepthFirstScene(scene, tree, levels, children);
		TestScene(scene, numTests);
	}

	// Breadth first split test
	{
		printf("\n\nBREADTH FIRST SPLIT TEST\n");

		Scene scene;
		Tree<TransformNode> tree;

		buildBreadthFirstScene(scene, tree, levels, children);
		TestScene(scene, numTests);
	}

	// Breadth first combined test 
	{
		printf("\n\nBREADTH FIRST COMBINED TEST\n");

		hierarchy::combined::Scene scene;
		Tree<TransformNode> tree;

		buildBreadthFirstScene(scene, tree, levels, children);
		TestScene(scene, numTests);
	}

	// Tree Depth First test
	{
		using ms = std::chrono::duration<float, std::milli>;
		using time = std::chrono::time_point<std::chrono::steady_clock>;
		std::chrono::high_resolution_clock timer;

		time start;
		time end;

		Scene scene;
		Tree<TransformNode> tree;

		buildBalancedTree(tree, levels, children);
		buildDepthFirstScene(scene, tree, levels, children);

		printf("\n\nTREE DEPTH FIRST TEST\n");

		float deltaTime = 0.f;
		float totalRuntime = 0.f;

		for (int i = 0; i < numTests; ++i)
		{
			start = timer.now();

			updateWorldTransformsDFS(tree);

			end = timer.now();
			deltaTime = std::chrono::duration_cast<ms>(end - start).count();
			totalRuntime += deltaTime;
		}

		printf("Number of nodes: %d\n", scene.nextFree);
		printf("Over %d samples, update took: %f ms on avg\n", numTests, totalRuntime / numTests);
	}

	return 0;
}