#pragma once

#include "HandmadeMath.h"

// NOTE(): These aren't fully featured extensions, they have just what I need to implement my tests

hmm_mat4 HMM_Mat4_Identity()
{
	return HMM_Mat4d(1.f);
}

struct hmm_plane
{
	hmm_vec3 n;
	float d;

	hmm_plane(const hmm_vec3& _n, float _d)
		: n(_n)
		, d(_d)
	{}

	hmm_plane(hmm_vec3& p0, hmm_vec3& p1, hmm_vec3&p2)
	{
		hmm_vec3 edge1 = p1 - p0;
		hmm_vec3  edge2 = p2 - p0;

		n = HMM_Cross(edge1, edge2);
		HMM_NormalizeVec3(n);
		d = HMM_DotVec3(n, p0);
	}
};

struct hmm_sphere
{
	hmm_vec3 center;
	float radius;
};

struct hmm_frustum
{
	enum FrustumSide
	{
		FS_NEAR = 0,
		FS_FAR = 1,
		FS_LEFT = 2,
		FS_RIGHT = 3,
		FS_TOP = 4,
		FS_BOTTOM = 5
	};

	std::vector<hmm_plane> sides;

	hmm_frustum(const hmm_mat4& projMat)
	{
		sides.resize(6);

		sides[FS_LEFT].n.X = projMat.Elements[3][0] + projMat.Elements[0][0];
		sides[FS_LEFT].n.Y = projMat.Elements[3][1] + projMat.Elements[0][1];
		sides[FS_LEFT].n.Z = projMat.Elements[3][2] + projMat.Elements[0][2];
		sides[FS_LEFT].d = projMat.Elements[3][3] + projMat.Elements[0][3];

		sides[FS_RIGHT].n.X = projMat.Elements[3][0] - projMat.Elements[0][0];
		sides[FS_RIGHT].n.Y = projMat.Elements[3][1] - projMat.Elements[0][1];
		sides[FS_RIGHT].n.Z = projMat.Elements[3][2] - projMat.Elements[0][2];
		sides[FS_RIGHT].d = projMat.Elements[3][3] - projMat.Elements[0][3];

		sides[FS_TOP].n.X = projMat.Elements[3][0] - projMat.Elements[1][0];
		sides[FS_TOP].n.Y = projMat.Elements[3][1] - projMat.Elements[1][1];
		sides[FS_TOP].n.Z = projMat.Elements[3][2] - projMat.Elements[1][2];
		sides[FS_TOP].d = projMat.Elements[3][3] - projMat.Elements[1][3];

		sides[FS_BOTTOM].n.X = projMat.Elements[3][0] + projMat.Elements[1][0];
		sides[FS_BOTTOM].n.Y = projMat.Elements[3][1] + projMat.Elements[1][1];
		sides[FS_BOTTOM].n.Z = projMat.Elements[3][2] + projMat.Elements[1][2];
		sides[FS_BOTTOM].d = projMat.Elements[3][3] + projMat.Elements[1][3];

		sides[FS_NEAR].n.X = projMat.Elements[3][0] + projMat.Elements[2][0];
		sides[FS_NEAR].n.Y = projMat.Elements[3][1] + projMat.Elements[2][1];
		sides[FS_NEAR].n.Z = projMat.Elements[3][2] + projMat.Elements[2][2];
		sides[FS_NEAR].d = projMat.Elements[3][3] + projMat.Elements[2][3];

		sides[FS_FAR].n.X = projMat.Elements[3][0] - projMat.Elements[2][0];
		sides[FS_FAR].n.Y = projMat.Elements[3][1] - projMat.Elements[2][1];
		sides[FS_FAR].n.Z = projMat.Elements[3][2] - projMat.Elements[2][2];
		sides[FS_FAR].d = projMat.Elements[3][3] - projMat.Elements[2][3];

		for (size_t i = 0; i < 6; ++i)
		{
			float length = HMM_LengthVec3(sides[i].n);
			sides[i].n = HMM_NormalizeVec3(sides[i].n);
			sides[i].d /= -length;
		}
	}
};

bool HMM_Intersects(const hmm_frustum& frustum, const hmm_sphere& sphere)
{
	hmm_vec3 center = sphere.center;
	float radius = sphere.radius;

	for (auto& plane : frustum.sides)
	{
		float dist = HMM_DotVec3(center, plane.n) - plane.d;

		if (dist < -radius)
			return false;
	}

	return true;
}
