#pragma once
#include "dos.h"

struct Node
{
	std::vector<std::shared_ptr<Node>> children;
};

template<class NodeType>
struct Tree
{
	Tree()
	{
		root = std::make_shared<NodeType>();
	}

	std::shared_ptr<NodeType> addNode(std::shared_ptr<NodeType> parent)
	{
		auto node = std::make_shared<NodeType>();
		parent->children.push_back(node);
		return node;
	}

	std::shared_ptr<NodeType> root;
};

struct TransformNode : public Node
{
	TransformNode()
		: localTransform(HMM_Mat4_Identity())
		, worldTransform(HMM_Mat4_Identity())
	{}

	hmm_mat4 localTransform;
	hmm_mat4 worldTransform;
	std::vector<std::shared_ptr<TransformNode>> children;
};

void updateTransformsImpl(std::shared_ptr<TransformNode> node, hmm_mat4& parentTransform)
{
	node->worldTransform = node->worldTransform * parentTransform;

	for (auto& child : node->children)
	{
		updateTransformsImpl(child, node->worldTransform);
	}
}

void updateWorldTransforms(Tree<TransformNode>& tree)
{
	updateTransformsImpl(tree.root, HMM_Mat4_Identity());
}

