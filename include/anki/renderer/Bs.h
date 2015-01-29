// Copyright (C) 2009-2015, Panagiotis Christopoulos Charitos.
// All rights reserved.
// Code licensed under the BSD License.
// http://www.anki3d.org/LICENSE

#ifndef ANKI_RENDERER_BS_H
#define ANKI_RENDERER_BS_H

#include "anki/renderer/RenderingPass.h"

namespace anki {

/// @addtogroup renderer
/// @{

/// Blending stage. The objects that blend must be handled differently
class Bs: public RenderingPass
{
	friend class Renderer;

private:
	Bs(Renderer* r)
	:	RenderingPass(r) 
	{}

	~Bs();

	ANKI_USE_RESULT Error init(const ConfigSet& initializer);
	ANKI_USE_RESULT Error run(GlCommandBufferHandle& cmdb);
};

/// @}

} // end namespace anki

#endif
