// Copyright (C) 2009-2016, Panagiotis Christopoulos Charitos and contributors.
// All rights reserved.
// Code licensed under the BSD License.
// http://www.anki3d.org/LICENSE

#pragma once

#include <anki/resource/RenderingKey.h>
#include <anki/scene/Forward.h>
#include <anki/Gr.h>

namespace anki
{

// Forward
class Renderer;
class DrawContext;
class CompleteRenderingBuildInfo;

/// @addtogroup renderer
/// @{

/// It includes all the functions to render a Renderable
class RenderableDrawer
{
	friend class SetupRenderableVariableVisitor;
	friend class RenderTask;

public:
	RenderableDrawer(Renderer* r)
		: m_r(r)
	{
	}

	~RenderableDrawer();

	ANKI_USE_RESULT Error drawRange(Pass pass,
		const FrustumComponent& frc,
		CommandBufferPtr cmdb,
		GrObjectCache& pplineCache,
		const PipelineInitInfo& state,
		VisibleNode* begin,
		VisibleNode* end);

private:
	Renderer* m_r;

	void flushDrawcall(DrawContext& ctx, CompleteRenderingBuildInfo& build);
	void setupUniforms(DrawContext& ctx, CompleteRenderingBuildInfo& build);

	ANKI_USE_RESULT Error drawSingle(DrawContext& ctx);
};
/// @}

} // end namespace anki
