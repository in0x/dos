#pragma once

#include "dos.h"

namespace dos 
{
	const int MAX_ENTITIES = 1000;

	struct Entity
	{
		int entityID;
		int parentID;
	};

	class SceneManager
	{
	public:
		std::array<hmm_mat4, MAX_ENTITIES> transforms;
		std::array<int, MAX_ENTITIES> parents;
	
		std::shared_ptr<Entity> addEntity(Entity* parent = nullptr)
		{
			if (lastEntityIdx == transforms.size())
			{
				Logger::Error("Exceeded max entity count");
				return nullptr;
			}

			auto entity = std::make_shared<Entity>();
			entity->entityID = lastEntityIdx;
			entity->parentID = parent ? parent->entityID : 0;

			lastEntityIdx++;
		}

		void removeEntity(const Entity& entity)
		{
			std::swap(transforms[lastEntityIdx], transforms[entity.entityID]);
		}

	private:
		int lastEntityIdx;
	};
	
}