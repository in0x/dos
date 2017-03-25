#pragma once

#include "dos.h"

namespace dos
{
	const int MAX_ENTITIES = 1000;

	void setEntity(hmm_mat4& mat)
	{
		mat.Elements[0][0] = 1;
		mat.Elements[1][1] = 1;
		mat.Elements[2][2] = 1;
		mat.Elements[3][3] = 1;
	}

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

			setEntity(localTransform);
			setEntity(worldTransform);
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
			parents[0] = 0; // This is stupid, but I want bad things to happen if we try to look up the root's parent for now.
			transforms[0].clear();
			nextFree = 1;
		}

		TransformID getRoot()
		{
			return TransformID{ 0 };
		}

		TransformID addTransform() // Parent will be root.
		{
			check(nextFree < MAX_ENTITIES);
			
			transforms[nextFree].clear();
			parents[nextFree] = 0;

			return TransformID{ nextFree++ };
		}

		TransformID addTransform(const hmm_mat4& local)
		{
			TransformID trafo = addTransform();
			transforms[trafo.index].localTransform = local;
			return trafo;
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

		TransformID addTransform(TransformID& parent, const hmm_mat4& local)
		{
			TransformID trafo = addTransform(parent);
			transforms[trafo.index].localTransform = local;
			return trafo;
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
			transforms[0].worldTransform = transforms[0].localTransform;

			for (int i = 1; i < nextFree; ++i)
			{
				transforms[i].worldTransform = transforms[parents[i]].worldTransform * transforms[i].localTransform;
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

		// Split up local and world transforms.
		// Local -> Depth first
		// World -> ??

		std::array<Transform, MAX_ENTITIES> transforms;
		std::array<uint16_t, MAX_ENTITIES> parents;
		int nextFree;
	};
}

