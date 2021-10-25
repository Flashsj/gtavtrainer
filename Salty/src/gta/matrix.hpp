#pragma once
#include "vector.hpp"

namespace rage
{
	class matrix4x4
	{
	public:
		union
		{
			struct
			{
				vector4 _1;
				vector4 _2;
				vector4 _3;
				vector4 _4;
			};

			float raw[4 * 4] = {};
		};
	};

	class CViewPort
	{
	public:
		char _0x0000[0x24C];
		float m_matrix[0x10];
	};

	struct ScreenResolution
	{
		uint32_t w, h;
	};

	struct alignas(16) Vec3V
	{
		float x;
		float y;
		float z;
		float pad;

		Vec3V()
			: x(0), y(0), z(0), pad(0)
		{
		}

		Vec3V(float x, float y, float z)
			: x(x), y(y), z(z), pad(NAN)
		{
		}
	};

	struct alignas(16) Vec4V
	{
		float x;
		float y;
		float z;
		float w;
	};

	struct spdAABB
	{
		rage::Vec3V mins;
		rage::Vec3V maxs;
	};

	struct spdSphere
	{
		// xyz = center
		// w   = radius
		Vec4V sphere;
	};

	// fake name
	struct spdRay
	{
		rage::Vec3V start;
		rage::Vec3V end;
	};

	enum class eSearchVolumeType : int
	{
		SphereContains,
		SphereIntersect,
		SphereIntersectPrecise,
		AabbContainsAabb,
		AabbContainsSphere,
		AabbIntersectsSphere,
		AabbIntersectsAabb,
		RayIntersectsAabb
	};

	struct fwSearchVolume
	{
		spdAABB aabb;
		spdSphere sphere;
		spdRay ray;
		eSearchVolumeType type;
	};

	struct grcViewport
	{
		float m_mat1[16];
		float m_mat2[16];
		float m_viewProjection[16];
		float m_inverseView[16];
		char m_pad[64];
		float m_projection[16];
	};

	struct spdViewport : grcViewport
	{
		static spdViewport* GetCurrent();
	};
}

struct CViewportGame
{
public:
	virtual ~CViewportGame() = 0;

private:
	char m_pad[8];

public:
	rage::grcViewport viewport;
};