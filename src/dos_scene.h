#pragma once

#include "dos.h"

namespace dos
{
	struct TransformID
	{
		int level;
		int idx;
		std::string name;
	};

	bool operator==(const TransformID& lhv, const TransformID& rhv)
	{
		return lhv.level == rhv.level && lhv.idx == rhv.idx;
	}

	bool operator!=(const TransformID& lhv, const TransformID& rhv)
	{
		return !(lhv == rhv);
	}

	struct Transform
	{
		hmm_mat4 localTransform;
		hmm_mat4 worldTransform;
		
		TransformID self;
		TransformID parent;
	};

	struct Scene
	{
		Scene()
			: hierarchy(2)
		{
			Transform root;
			root.self = { 0,0, "Root" };
			hierarchy[0].push_back(root);
		}

		std::vector<std::vector<Transform>> hierarchy;

		TransformID getRoot()
		{
			return hierarchy[0][0].self;
		}
	};

	TransformID addTransform(Scene& scene, const std::string& name)
	{
		Transform newTrafo;
		newTrafo.parent = scene.getRoot();
		
		scene.hierarchy[1].push_back(newTrafo);

		scene.hierarchy[1][scene.hierarchy[1].size() - 1].self = TransformID{ 1, static_cast<int>(scene.hierarchy[1].size()) - 1, name };
		return scene.hierarchy[1][scene.hierarchy[1].size() - 1].self;
	}

	TransformID addTransform(Scene& scene, TransformID parent, const std::string& name)
	{
		Transform newTrafo;
		newTrafo.parent = parent;

		if (parent.level + 1 >= scene.hierarchy.size()) // Add another level if we exceed the current last
		{
			scene.hierarchy.push_back({});
		}
		
		auto& level = scene.hierarchy[parent.level + 1];
		level.push_back(newTrafo);
		
		int i;
		for (i = level.size() - 1; i > 1; i--) // Swap until we are contigous with elements of same parent
		{
			if (level[i - 1].parent != level[i].parent) 
			{
				std::swap(level[i - 1], level[i]);
			}
		}

		level[i].self = TransformID{ parent.level + 1, i, name };
		return level[i].self;
	}

}

