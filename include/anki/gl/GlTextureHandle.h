// Copyright (C) 2009-2015, Panagiotis Christopoulos Charitos.
// All rights reserved.
// Code licensed under the BSD License.
// http://www.anki3d.org/LICENSE

#ifndef ANKI_GL_GL_TEXTURE_HANDLE_H
#define ANKI_GL_GL_TEXTURE_HANDLE_H

#include "anki/gl/GlContainerHandle.h"
#include "anki/gl/GlClientBufferHandle.h"

namespace anki {

// Forward
class GlTexture;
class GlSampler;

/// @addtogroup opengl_containers
/// @{

/// Texture handle
class GlTextureHandle: public GlContainerHandle<GlTexture>
{
public:
	using Base = GlContainerHandle<GlTexture>;

	using Filter = GlTextureFilter;

	/// Texture handle initializer
	class Initializer: public GlTextureInitializerBase
	{
	public:
		Array2d<GlClientBufferHandle, 
			ANKI_GL_MAX_MIPMAPS, ANKI_GL_MAX_TEXTURE_LAYERS> m_data;
	};

	/// Create husk
	GlTextureHandle();

	~GlTextureHandle();

	/// Create the texture
	Error create(GlCommandBufferHandle& commands, const Initializer& init);

	/// Bind to a unit
	void bind(GlCommandBufferHandle& commands, U32 unit);

	/// Change filtering type
	void setFilter(GlCommandBufferHandle& commands, Filter filter);

	/// Generate mips
	void generateMipmaps(GlCommandBufferHandle& commands);

	/// Set a texture parameter
	void setParameter(
		GlCommandBufferHandle& commands, GLenum param, GLint value);
};

/// Sampler handle
class GlSamplerHandle: public GlContainerHandle<GlSampler>
{
public:
	using Base = GlContainerHandle<GlSampler>;

	using Filter = GlTextureFilter;

	/// Create husk
	GlSamplerHandle();

	~GlSamplerHandle();

	/// Create the sampler
	ANKI_USE_RESULT Error create(GlCommandBufferHandle& commands);

	/// Bind to a unit
	void bind(GlCommandBufferHandle& commands, U32 unit);

	/// Change filtering type
	void setFilter(GlCommandBufferHandle& commands, Filter filter);

	/// Set a texture parameter
	void setParameter(GlCommandBufferHandle& commands, GLenum param, GLint value);

	/// Bind default sampler
	static void bindDefault(GlCommandBufferHandle& commands, U32 unit);
};

/// @}

} // end namespace anki

#endif
