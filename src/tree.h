#pragma once

#include <memory>

struct TransformNode
{
	TransformNode::TransformNode()
		: localTransform(HMM_Mat4_Identity())
		, worldTransform(HMM_Mat4_Identity())
		, localBounds()
		, worldBounds()
		, children()
		, bVisible(true)
	{}

	hmm_mat4 localTransform;
	hmm_mat4 worldTransform;
	hmm_sphere localBounds;
	hmm_sphere worldBounds;
	std::vector<std::shared_ptr<TransformNode>> children;
	bool bVisible;
};

class SceneTree
{
public:

	SceneTree();

	std::shared_ptr<TransformNode> addNode(std::shared_ptr<TransformNode> parent);
	void updateWorldTransformsDFS();
	void cullSceneTree(const hmm_frustum& frustum);
	void cullSceneTreeHierarchical(const hmm_frustum& frustum);
	void renderTree();
	std::shared_ptr<TransformNode> root;
	int numNodes;

private:
	void renderNode(std::shared_ptr<TransformNode> node);
	void updateWorldTransformNode(std::shared_ptr<TransformNode> node, const hmm_mat4& parentTransform);
	void cullNode(std::shared_ptr<TransformNode> node, const hmm_mat4& parentTransform, const hmm_frustum& frustum);
	void cullNodeHierarchical(std::shared_ptr<TransformNode> node, const hmm_mat4& parentTransform, const hmm_frustum& frustum);
};


