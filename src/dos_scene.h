#pragma once

#include "dos.h"

#define DEPTH_FIRST 0
#define UNSORTED 1

namespace dos
{
#if DEPTH_FIRST	
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

	TransformID addTransform(Scene& scene, TransformID& parent, const std::string& name)
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

	void removeTransform(Scene& scene, TransformID& transform)
	{
		auto level = scene.hierarchy[transform.level];

		level.erase(std::remove(level.begin(), level.end(), level[transform.idx]), level.end());


	}

#endif // DEPTH_FIRST

#if UNSORTED

	const int MAX_ENTITIES = /*UINT16_MAX*/ 1000;

	struct Transform	
	{
		hmm_mat4 localTransform;
		hmm_mat4 worldTransform;
		bool isLeaf = true; // We can hide this as part of the index more efficiently later.

		void clear()
		{
			for (int x = 0; x < 4; ++x)
			{
				for (int y = 0; y < 4; ++y)
				{
					localTransform.Elements[x][y] = 0.f;
					worldTransform.Elements[x][y] = 0.f;
				}
			}
		}

	};

	struct TransformID 
	{
		int index;
	};

	// Problem with this approach is that hierarchy gets fragmented.
	// Parent is guaranteed to always be in front of child due to swap on insert.
	struct Scene 
	{
		Scene()
			: nextFree(0)
		{
			transforms[0].isLeaf = false;
			parents[0] = MAX_ENTITIES; // This is stupid, but I want bad things to happen if we try to look up the root's parent for now.
			nextFree = 1;
		}

		TransformID addTransform() // Parent will be root.
		{
			check(nextFree < MAX_ENTITIES);
			
			transforms[nextFree].clear();
			parents[nextFree] = 0;

			return TransformID{ nextFree++ };
		}

		TransformID addTransform(TransformID& parent) // Parent ref not const because order change in array might change index of parent.
		{
			check(nextFree < MAX_ENTITIES);
			
			transforms[nextFree].clear();
			parents[nextFree] = parent.index;
			transforms[parent.index].isLeaf = false;

			// Make sure our parent is at a lower index, this way we can guarantee that their
			// world transform is ready before we need it.
			if (nextFree < parent.index) 
			{
				std::swap(transforms[nextFree], transforms[parent.index]);
				std::swap(parents[nextFree], parents[parent.index]);

				// We swapped parent and child so that parent is updated first, so their index also swaps.
				TransformID id{parent.index};
				parent.index = nextFree++;
				return id;
			}

			return TransformID{ nextFree++ };
		}

		void removeTransform(const TransformID& transform)
		{
			check(transform.index > 0);

			int newParentIdx = parents[transform.index];
			bool isLeaf = transforms[transform.index].isLeaf;
			
			if (isLeaf)
			{
				transforms[newParentIdx].isLeaf = true;
			}
			else
			{
				// Relink children of removed node to transforms parent.
				std::replace(parents.begin(), parents.end(), transform.index, newParentIdx);
			}

			std::swap(transforms[nextFree - 1], transforms[transform.index]);
			std::swap(parents[nextFree - 1], parents[transform.index]);
			
			nextFree--;
		}

		void updateWorldTransforms()
		{
			for (int i = 1; i < nextFree; ++i)
			{
				transforms[i].worldTransform = transforms[i].localTransform * transforms[parents[i]].worldTransform;
			}
		}

		void updateTransform(const TransformID& transform, const hmm_mat4& newLocal) // Just implementing this since its a prototype for now
		{
			transforms[transform.index].localTransform = newLocal;
			transforms[transform.index].worldTransform = transforms[transform.index].localTransform * transforms[parents[transform.index]].worldTransform;

			auto parentIdx = transform.index;

			updateChildTransforms(parentIdx);
		}

		void updateChildTransforms(int parentIdx)
		{
			// Since children are always after parent, one iteration of the array should be enough to update all children
			for (int i = parentIdx; i < nextFree; ++i)
			{
				if (parents[i] == parentIdx)
				{
					transforms[i].worldTransform = transforms[i].localTransform * transforms[parents[i]].worldTransform;
					
					if (!transforms[i].isLeaf)
					{
						updateChildTransforms(i);
					}
				}
			}
		}

		std::array<Transform, MAX_ENTITIES> transforms;
		std::array<uint16_t, MAX_ENTITIES> parents;
		int nextFree;
	};

#endif // UNSORTED
}

