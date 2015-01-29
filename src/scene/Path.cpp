// Copyright (C) 2009-2015, Panagiotis Christopoulos Charitos.
// All rights reserved.
// Code licensed under the BSD License.
// http://www.anki3d.org/LICENSE

#include "anki/scene/Path.h"

namespace anki {

#if 0

//==============================================================================
Path::Path(
	const char* name, SceneGraph* scene, 
	const char* filename)
	:	SceneNode(name, scene), 
		MoveComponent(this),
		points(getSceneAllocator())
{
	// Set scene node related flags
	sceneNodeProtected.moveC = this;
	sceneNodeProtected.pathC = this;

	// XXX Fix the dummy load 
	Vector<std::pair<Vec3, Quat>> tmp = {
		{Vec3(-12.4964, 10.3002, -8.83644),  Quat(0.0174752, -0.969103, -0.00854539, 0.245888)},
{Vec3(16.1371, 11.5968, -8.3768),  Quat(-0.0310868, 0.940534, 0.023991, 0.337438)},
{Vec3(19.3678, 11.8323, -7.89213),  Quat(-0.0268366, 0.91397, 0.0201037, 0.40441)},
{Vec3(20.7731, 12.5386, -4.00229), Quat(-0.0971082, 0.782116, 0.0873222, 0.6093)},
{Vec3(20.2446, 11.8249, 1.2901),  Quat(-0.00907603, 0.719142, 0.0141181, 0.694673)},
{Vec3(12.5349, 11.1931, 0.648789),  Quat(-0.127753, 0.711443, 0.129103, 0.678884)},
{Vec3(9.90018, 2.31293, 1.5228),  Quat(0.0185091, 0.774396, -0.00581863, 0.632404)},
{Vec3(11.3538, 1.32283, 0.125991),  Quat(0.00583224, 0.771427, 0.0107741, 0.636202)},
{Vec3(19.0313, 2.58059, -3.90416),  Quat(-0.00359187, 0.797582, 0.0205641, 0.602854)},
{Vec3(20.7252, 2.72255, -7.11897),  Quat(-0.00762224, 0.845843, 0.0233639, 0.532872)},
{Vec3(16.2244, 2.63427, -9.57957),  Quat(0.00111139, 0.923906, -0.00257634, 0.382623)},
{Vec3(-1.72731, 2.57222, -10.4981),  Quat(-0.00251057, 0.999248, 0.00509784, 0.0385309)},
{Vec3(-16.3919, 3.66866, -9.5139),  Quat(0.00272353, -0.97944, -0.0238562, 0.200345)},
{Vec3(-21.3014, 3.56377, -6.92104),  Quat(0.00880986, -0.89649, -0.0243779, 0.442329)},
{Vec3(-22.5468, 4.19827, -0.0445367),  Quat(0.0118534, -0.639768, -0.0276997, 0.767993)},
{Vec3(-20.5534, 4.01987, 6.74214),  Quat(0.00108179, -0.474133, -0.0374377, 0.879656)},
{Vec3(-14.6694, 3.63242, 4.82707),  Quat(-0.0702802, -0.423791, -0.06851, 0.900429)},
{Vec3(-13.8528, 4.5495, -1.21759),  Quat(-0.0974997, -0.731442, -0.0571003, 0.672482)},
{Vec3(-6.98517, 3.04676, -1.17589),  Quat(-0.0974314, -0.785521, -0.0440555, 0.609533)},
{Vec3(-5.46171, 2.75274, -0.785615),  Quat(-0.0947099, -0.78128, -0.0416765, 0.61555)},
{Vec3(-2.24792, 2.63747, -3.55529),  Quat(-0.0249922, -0.998763, -0.0296618, 0.031339)},
{Vec3(1.26739, 2.34591, 1.15404),  Quat(0.0520454, 0.723209, -0.0448162, 0.687212)},
{Vec3(-0.376718, 2.57919, 3.98087),  Quat(0.0168362, 0.303837, -0.0616157, 0.950586)},
{Vec3(-1.35176, 2.79261, 2.48651),  Quat(0.099356, 0.247582, -0.0892974, 0.95962)}
	};

	distance = 0.0;

	for(U i = 0; i < tmp.size(); i++)
	{
		PathPoint pp;

		pp.pos = tmp[i].first;
		pp.rot = tmp[i].second;

		// Update the distances
		if(i != 0)
		{
			pp.dist = (pp.pos - points[i - 1].pos).getLength();
			pp.distStart = distance + pp.dist;

			distance = pp.distStart;
		}
		else
		{
			pp.dist = 0.0;
			pp.distStart = 0.0;
		}

		// Push back point
		points.push_back(pp);
	}
}

#endif

} // end namespace anki
