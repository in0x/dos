#include "dos.h"
#include "tree.h"

SceneTree::SceneTree()
{
	root = std::make_shared<TransformNode>();
}

std::shared_ptr<TransformNode> SceneTree::addNode(std::shared_ptr<TransformNode> parent)
{
	auto node = std::make_shared<TransformNode>();
	parent->children.push_back(node);
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



