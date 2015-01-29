// Copyright (C) 2009-2015, Panagiotis Christopoulos Charitos.
// All rights reserved.
// Code licensed under the BSD License.
// http://www.anki3d.org/LICENSE

#ifndef ANKI_RENDERER_SSLR_H
#define ANKI_RENDERER_SSLR_H

#include "anki/renderer/RenderingPass.h"
#include "anki/resource/Resource.h"
#include "anki/Gl.h"

namespace anki {

/// @addtogroup renderer
/// @{

/// Screen space local reflections pass
class Sslr: public BlurringRenderingPass
{
	friend class Pps;

public:
	/// @privatesection
	/// @{
	GlTextureHandle& _getRt()
	{
		return m_dirs[(U)DirectionEnum::VERTICAL].m_rt;
	}
	/// @}

private:
	U32 m_width;
	U32 m_height;

	// 1st pass
	ProgramResourcePointer m_reflectionFrag;
	GlPipelineHandle m_reflectionPpline;
	GlSamplerHandle m_depthMapSampler;

	// 2nd pass: blit
	ProgramResourcePointer m_blitFrag;
	GlPipelineHandle m_blitPpline;

	Sslr(Renderer* r)
	:	BlurringRenderingPass(r)
	{}

	ANKI_USE_RESULT Error init(const ConfigSet& config);
	ANKI_USE_RESULT Error run(GlCommandBufferHandle& cmdBuff);
};

/// @}

} // end namespace anki

#endif
