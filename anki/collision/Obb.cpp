#include "anki/collision/Obb.h"
#include "anki/collision/Plane.h"


namespace anki {


//==============================================================================
Obb::Obb(const Obb& b)
:	CollisionShape(CST_OBB),
	center(b.center),
	rotation(b.rotation),
	extends(b.extends)
{}


//==============================================================================
Obb::Obb(const Vec3& center_, const Mat3& rotation_,
	const Vec3& extends_)
:	CollisionShape(CST_OBB),
	center(center_),
	rotation(rotation_),
	extends(extends_)
{}


//==============================================================================
Obb Obb::getTransformed(const Transform& transform) const
{
	Obb out;
	out.extends = extends * transform.getScale();
	out.center = center.getTransformed(transform);
	out.rotation = transform.getRotation() * rotation;
	return out;
}


//==============================================================================
Obb Obb::getCompoundShape(const Obb& b) const
{
	Obb out;

	boost::array<Vec3, 8> points0;
	boost::array<Vec3, 8> points1;

	getExtremePoints(points0);
	b.getExtremePoints(points1);

	boost::array<Vec3, 16> points;
	for(uint i = 0; i < 8; i++)
	{
		points[i] = points0[i];
		points[i + 8] = points1[i];
	}

	out.set(points);
	return out;
}


//==============================================================================
void Obb::getExtremePoints(boost::array<Vec3, 8>& points) const
{
	// L: left, R: right, T: top, B: bottom, F: front, B: back
	enum
	{
		RTF,
		LTF,
		LBF,
		RBF,
		RTB,
		LTB,
		LBB,
		RBB
	};

	Vec3 er = rotation * extends; // extend rotated

	points[RTF] = er;
	points[LBB] = -er;

	Vec3 xAxis = rotation.getColumn(0);
	Vec3 yAxis = rotation.getColumn(1);
	Vec3 zAxis = rotation.getColumn(2);

	// Reflection: x1' = 2n|x1.n| - x1

	points[RBB] = 2.0 * er.dot(xAxis) * xAxis - er;
	points[LTB] = 2.0 * er.dot(yAxis) * yAxis - er;
	points[LBF] = 2.0 * er.dot(zAxis) * zAxis - er;

	points[LTF] = 2.0 * points[LBB].dot(-xAxis) * -xAxis - points[LBB];
	points[RTB] = 2.0 * points[LTF].dot(yAxis) * yAxis - points[LTF];
	points[RBF] = 2.0 * points[LTF].dot(zAxis) * zAxis - points[LTF];

	BOOST_FOREACH(Vec3& point, points)
	{
		point += center;
	}
}


} // end namespace