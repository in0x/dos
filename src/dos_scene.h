#pragma once

#include "dos.h"

namespace dos 
{
	const int MAX_ENTITIES = 1000;

	struct Transform
	{
		hmm_mat4 localTransform;
		hmm_mat4 worldTransform;
	};

	struct EntityTransform
	{
		hmm_mat4* transform;
		hmm_mat4* parentTransform;
	};

	struct Scene
	{
		Scene()
			: transforms(MAX_ENTITIES)
			//, hierarchy(MAX_ENTITIES)
			, lastEntityIdx(0)
		{
		}

		std::vector<Transform> transforms;
		//std::vector<int> hierarchy;
		int lastEntityIdx;
	};

	EntityTransform addEntity(Scene& scene)
	{
	}

	Entity addEntity(Scene& scene, Entity& parent)
	{
	}

	void removeEntity(Scene& scene, const Entity& entity) 
	{
	}

	void generateWorldTransform(Scene& scene)
	{
		for (int idx = 1; idx < scene.transforms.size(); idx++)
		{
			int parentIdx = scene.hierarchy[idx];
			scene.transforms[idx].worldTransform = scene.transforms[idx].localTransform * scene.transforms[parentIdx].worldTransform;
		}
	}
}

