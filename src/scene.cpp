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
		resetNode(i);
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
		resetNode(i);
	}

	nextFree = size;
}

void Scene::resetNode(int idx)
{
	HMM_Clear(local[idx]);
	HMM_Clear(world[idx]);
	parents[idx] = 0;

	localBounds[idx].center = HMM_Vec4(0.f, 0.f, 0.f, 1.f);
	localBounds[idx].radius = 1.f;

	worldBounds[idx].center = HMM_Vec4(0.f, 0.f, 0.f, 1.f);
	worldBounds[idx].radius = 0.f;

	bVisible[idx] = false;
}

TransformID Scene::addTransform()
{
	check(nextFree < MAX_ENTITIES);
	resetNode(nextFree);
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
	}
}

void Scene::updateWorldBounds()
{
	for (int i = 0; i < nextFree; ++i)
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
	for (int i = 0; i < nextFree;)
	{
		bVisible[i] = HMM_Intersects(frustum, worldBounds[i]);

		if (!bVisible[i])
		{
			int current = i;
			int nextSubTree = i + 1;
			while (nextSubTree < nextFree && parents[nextSubTree] > parents[current])
			{
				bVisible[nextSubTree] = false;
				nextSubTree++;
			}
			i = nextSubTree;
		}
		else
		{
			i++;
		}
	}
}

void Scene::updateTransform(const TransformID& transform, const hmm_mat4& newLocal)
{
	local[transform.index] = newLocal;
	world[transform.index] = world[parents[transform.index]] * local[transform.index];
}

void Scene::buildFromSceneTree(const SceneTree& tree)
{
	resize(tree.numNodes);
	int nodeCount = 0;
	visitSceneDF(tree.root, 0, nodeCount);
}

const float NODE_OFFSET_X = 1.0f;
const float NODE_OFFSET_Y = 1.5f;

// This is all garbled up because I built node placement right since I don't care about doing it manually
void Scene::visitSceneDF(std::shared_ptr<TransformNode> node, int parentIdx, int& nodeCount) 
{
	nodeCount += 1;

	float childCount = static_cast<float>(node->children.size());
	float columnHalfWidth = (childCount * NODE_OFFSET_X) / 2.0f;
	childCount--;
	float localIdx = 0;
	
	for (auto& child : node->children)
	{
		resetNode(nodeCount);		
		parents[nodeCount] = parentIdx;
		
		float xOffset = HMM_Lerp(-columnHalfWidth, localIdx / childCount, columnHalfWidth);
		local[nodeCount] = HMM_Translate(HMM_Vec3(local[parentIdx].Elements[3][0] + xOffset, -NODE_OFFSET_Y, 0.f));
		localIdx++;

		visitSceneDF(child, nodeCount, nodeCount);
	}
}
