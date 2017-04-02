#pragma once

#include <vector>

struct TransformNode;
class SceneTree;

struct TransformID
{
	int index;
};

struct Scene 
{
	Scene();

	TransformID getRoot();
	TransformID addTransform(); // Parent will be root.
	TransformID addTransform(const hmm_mat4& localTrafo);

	void render();
	void resize(int size);
	void resetNode(int idx);
	void updateWorldBounds();
	void updateWorldTransforms();
	void cullScene(const hmm_frustum& frustum);
	void buildFromSceneTree(const SceneTree& tree);
	void cullSceneHierarchical(const hmm_frustum& frustum);
	void updateTransform(const TransformID& transform, const hmm_mat4& newLocal);
	void visitSceneDF(std::shared_ptr<TransformNode> node, int parentIdx, int& nodeCount);

	std::vector<hmm_mat4> local;
	std::vector<hmm_mat4> world;
	std::vector<uint16_t> parents;
	std::vector<hmm_sphere> localBounds;
	std::vector<hmm_sphere> worldBounds;
	std::vector<bool> bVisible;

	int nextFree;
};


