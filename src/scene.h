#pragma once

#include <vector>

class SceneTree;

struct TransformID
{
	int index;
};

class Scene 
{
public:
	Scene();

	TransformID getRoot();
	TransformID addTransform(); // Parent will be root.
	TransformID addTransform(const hmm_mat4& localTrafo);

	void render();
	void resize(int size);
	void updateWorldBounds();
	void clearTransform(int id);
	void updateWorldTransforms();
	void cullScene(const hmm_frustum& frustum);
	void buildFromSceneTree(const SceneTree& tree);
	void cullSceneHierarchical(const hmm_frustum& frustum);
	void updateTransform(const TransformID& transform, const hmm_mat4& newLocal);

	std::vector<hmm_mat4> local;
	std::vector<hmm_mat4> world;
	std::vector<uint16_t> parents;
	std::vector<hmm_sphere> localBounds;
	std::vector<hmm_sphere> worldBounds;
	std::vector<bool> bVisible;

	int nextFree;
};


