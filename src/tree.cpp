#include "dos.h"
#include "tree.h"

SceneTree::SceneTree()
	: numNodes(1)
{
	root = std::make_shared<TransformNode>();
}

std::shared_ptr<TransformNode> SceneTree::addNode(std::shared_ptr<TransformNode> parent)
{
	auto node = std::make_shared<TransformNode>();
	parent->children.push_back(node);
	numNodes++;
	return node;
}

void SceneTree::updateWorldTransformsDFS()
{
	updateWorldTransformNode(root, root->localTransform);
}

void SceneTree::cullSceneTree(const hmm_frustum& frustum)
{
	cullNode(root, root->localTransform, frustum);
}
void SceneTree::cullSceneTreeHierarchical(const hmm_frustum& frustum)
{
	cullNode(root, root->localTransform, frustum);
}

void SceneTree::renderTree()
{
	renderNode(root);
}

void SceneTree::updateWorldTransformNode(std::shared_ptr<TransformNode> node, const hmm_mat4& parentTransform)
{
	node->worldTransform = node->worldTransform * parentTransform;
	HMM_Transform(node->localBounds, node->worldBounds, node->worldTransform);

	for (auto& child : node->children)
	{
		updateWorldTransformNode(child, node->worldTransform);
		HMM_Expand(node->worldBounds, child->worldBounds);
	}
}

void SceneTree::cullNode(std::shared_ptr<TransformNode> node, const hmm_mat4& parentTransform, const hmm_frustum& frustum)
{
	node->bVisible = HMM_Intersects(frustum, node->worldBounds);

	for (auto& child : node->children)
	{
		cullNode(child, node->worldTransform, frustum);
	}
}

void SceneTree::cullNodeHierarchical(std::shared_ptr<TransformNode> node, const hmm_mat4& parentTransform, const hmm_frustum& frustum)
{
	node->bVisible = HMM_Intersects(frustum, node->worldBounds);

	if (!node->bVisible)
	{
		return;
	}

	for (auto& child : node->children)
	{
		cullNode(child, node->worldTransform, frustum);
	}
}

void SceneTree::renderNode(std::shared_ptr<TransformNode> node)
{
	if (node->bVisible)
	{
		node->bVisible = !node->bVisible;
	}

	for (auto& child : node->children)
	{
		renderNode(child);
	}
}

SceneTree SceneTree::buildBalancedTree(int levels, int childNodesPerLevel)
{
	SceneTree tree;

	std::vector<std::shared_ptr<TransformNode>> level;
	level.push_back(tree.root);

	for (int i = 0; i < levels; ++i)
	{
		std::vector<std::shared_ptr<TransformNode>> nextLevel;

		for (auto& node : level)
		{
			for (int child = 0; child < childNodesPerLevel; ++child)
			{
				nextLevel.push_back(tree.addNode(node));
			}
		}

		level = nextLevel;
	}

	return tree;
}

