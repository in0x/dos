#include "dos.h"
#include "scene.h"
#include "tree.h"

Scene::Scene()
	: nextFree(0)
{
	local.resize(MAX_ENTITIES);
	world.resize(MAX_ENTITIES);
	parents.resize(MAX_ENTITIES);
	localBounds.resize(MAX_ENTITIES);
	worldBounds.resize(MAX_ENTITIES);
	bVisible.resize(MAX_ENTITIES);

	for (int i = 0; i < MAX_ENTITIES; ++i)
	{

		clearTransform(i);
	}

	parents[0] = 0;
	nextFree = 1;
}

TransformID Scene::getRoot()
{
	return TransformID{ 0 };
}

void Scene::resize(int size)
{
	check(size < MAX_ENTITIES);

	for (int i = 0; i < size; ++i)
	{
		clearTransform(i);
	}

	nextFree = size;
}

void Scene::clearTransform(int id)
{
	HMM_Clear(local[nextFree]);
	HMM_Clear(world[nextFree]);
}

TransformID Scene::addTransform()
{
	check(nextFree < MAX_ENTITIES);

	clearTransform(nextFree);
	parents[nextFree] = 0;

	return TransformID{ nextFree++ };
}

TransformID Scene::addTransform(const hmm_mat4& localTrafo)
{
	TransformID trafo = addTransform();
	local[trafo.index] = localTrafo;
	return trafo;
}

void Scene::updateWorldTransforms()
{
	world[0] = local[0];

	for (int i = 1; i < nextFree; ++i)
	{
		world[i] = world[parents[i]] * local[i];
		HMM_Transform(localBounds[i], worldBounds[i], world[i]);
	}
}

void Scene::updateWorldBounds()
{
	for (int i = 1; i < nextFree; ++i)
	{
		HMM_Transform(localBounds[i], worldBounds[i], world[i]);
		HMM_Expand(worldBounds[parents[i]], worldBounds[i]);
	}
}

void Scene::cullScene(const hmm_frustum& frustum)
{
	for (int i = 0; i < nextFree; ++i)
	{
		bVisible[i] = HMM_Intersects(frustum, worldBounds[i]);
	}
}

void Scene::cullSceneHierarchical(const hmm_frustum& frustum)
{
	for (int i = 0; i < nextFree; ++i)
	{
		bVisible[i] = HMM_Intersects(frustum, worldBounds[i]);

		if (!bVisible[i])
		{
			int nextSubTree = i;
			int currentParent = parents[i];
			while (nextSubTree < nextFree && parents[nextSubTree] >= currentParent)
			{
				// Since we are in depth first order, our children follow us. 
				// We can find the next non-child by looking at the next node's parent
				// since nodes in the same subtree will only have a parentIdx larger than us.
				nextSubTree++;
			}
		}
	}
}

void Scene::updateTransform(const TransformID& transform, const hmm_mat4& newLocal)
{
	local[transform.index] = newLocal;
	world[transform.index] = world[parents[transform.index]] * local[transform.index];
}

void Scene::render()
{
	for (int i = 0; i < nextFree; ++i)
	{
		if (bVisible[i])
		{
			bVisible[i] = !bVisible[i];
		}
	}
}

void Scene::buildFromSceneTree(const SceneTree& tree)
{
	resize(tree.numNodes);
	int nodeCount = 0;
	visitSceneDF(tree.root, 0, nodeCount);
}

void Scene::visitSceneDF(std::shared_ptr<TransformNode> node, int parentIdx, int& nodeCount)
{
	nodeCount += 1;

	for (auto& child : node->children)
	{
		parents[nodeCount] = parentIdx;
		visitSceneDF(child, nodeCount, nodeCount);
	}
}
