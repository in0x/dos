#pragma once

#include "dos.h"

namespace dos
{
	void setEntity(hmm_mat4& mat)
	{
		mat.Elements[0][0] = 1;
		mat.Elements[1][1] = 1;
		mat.Elements[2][2] = 1;
		mat.Elements[3][3] = 1;
	}

	void clear(hmm_mat4& mat)
	{
		for (int x = 0; x < 4; ++x)
		{
			for (int y = 0; y < 4; ++y)
			{
				mat.Elements[x][y] = 0.f;
			}
		}

		setEntity(mat);
	}

	// Problem with this approach is that hierarchy gets fragmented.
	// Parent is guaranteed to always be in front of child due to swap on insert.
	struct Scene 
	{
		Scene()
			: nextFree(0)
		{
			local.resize(MAX_ENTITIES);
			world.resize(MAX_ENTITIES);
			parents.resize(MAX_ENTITIES);

			for (int i = 0; i < MAX_ENTITIES; ++i)
			{

				clearTransform(i);
			}

			parents[0] = 0; 
			nextFree = 1;
		}

		TransformID getRoot()
		{
			return TransformID{ 0 };
		}

		void resize(int size)
		{
			check(size < MAX_ENTITIES);

			for (int i = 0; i < size; ++i)
			{
				clearTransform(i);
			}

			nextFree = size;
		}

		void clearTransform(int id)
		{
			clear(local[nextFree]);
			clear(world[nextFree]);
		}

		TransformID addTransform() // Parent will be root.
		{
			check(nextFree < MAX_ENTITIES);
			
			clearTransform(nextFree);
			parents[nextFree] = 0;

			return TransformID{ nextFree++ };
		}

		TransformID addTransform(const hmm_mat4& localTrafo)
		{
			TransformID trafo = addTransform();
			local[trafo.index] = localTrafo;
			return trafo;
		}

		TransformID addTransform(TransformID& parent) // Parent ref not const because order change in array might change index of parent.
		{
			check(nextFree < MAX_ENTITIES);
			
			clearTransform(nextFree);
			parents[nextFree] = parent.index;

			// Make sure our parent is at a lower index, this way we can guarantee that their
			// world transform is ready before we need it.
			if (nextFree < parent.index) 
			{
				std::swap(local[nextFree], local[parent.index]);
				std::swap(world[nextFree], world[parent.index]);
				std::swap(parents[nextFree], parents[parent.index]);

				// We swapped parent and child so that parent is updated first, so their index also swaps.
				TransformID id{parent.index};
				parent.index = nextFree++;
				return id;
			}

			return TransformID{ nextFree++ };
		}

		TransformID addTransform(TransformID& parent, const hmm_mat4& localTrafo)
		{
			TransformID trafo = addTransform(parent);
			local[trafo.index] = localTrafo;
			return trafo;
		}

		void removeTransform(const TransformID& transform)
		{
			check(transform.index > 0);

			int newParentIdx = parents[transform.index];
			
			// Relink children of removed node to transforms parent.
			std::replace(parents.begin(), parents.end(), transform.index, newParentIdx);
		
			std::swap(local[nextFree - 1], local[transform.index]);
			std::swap(world[nextFree - 1], world[transform.index]);
			std::swap(parents[nextFree - 1], parents[transform.index]);
			
			nextFree--;
		}

		void updateWorldTransforms()
		{
			world[0] = local[0];

			for (int i = 1; i < nextFree; ++i)
			{
				world[i] = world[parents[i]] * local[i];
			}
		}

		void updateTransform(const TransformID& transform, const hmm_mat4& newLocal) // Just implementing this since its a prototype for now
		{
			local[transform.index] = newLocal;
			world[transform.index] = world[parents[transform.index]] * local[transform.index];

		}

		// Split up local and world transforms.
		// Local -> Depth first
		// World -> ??

		//std::array<hmm_mat4, MAX_ENTITIES> local;
		//std::array<hmm_mat4, MAX_ENTITIES> world;
		//std::array<uint16_t, MAX_ENTITIES> parents;
	
		std::vector<hmm_mat4> local;
		std::vector<hmm_mat4> world;
		std::vector<uint16_t> parents;
		
		int nextFree;
	};
}

