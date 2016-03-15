// Copyright (C) 2009-2016, Panagiotis Christopoulos Charitos and contributors.
// All rights reserved.
// Code licensed under the BSD License.
// http://www.anki3d.org/LICENSE

#pragma once

#include <anki/gr/gl/GlObject.h>
#include <anki/util/DArray.h>

namespace anki
{

/// @addtogroup opengl
/// @{

/// Texture container.
class TextureImpl : public GlObject
{
public:
	GLenum m_target = GL_NONE; ///< GL_TEXTURE_2D, GL_TEXTURE_3D... etc
	GLenum m_internalFormat = GL_NONE; ///< GL_COMPRESSED_RED, GL_RGB16 etc
	GLenum m_format = GL_NONE;
	GLenum m_type = GL_NONE;
	U32 m_width = 0;
	U32 m_height = 0;
	U32 m_depth = 0;
	U32 m_surfaceCount = 0;
	U8 m_mipsCount = 0;
	U8 m_faceCount = 0; ///< 6 for cubes and 1 for the rest
	Bool8 m_compressed = false;
	DArray<GLuint> m_texViews; ///< Temp views for gen mips.

	TextureImpl(GrManager* manager)
		: GlObject(manager)
	{
	}

	~TextureImpl();

	/// Create the texture storage.
	void init(const TextureInitInfo& init);

	/// Write texture data.
	void write(const TextureSurfaceInfo& surf, void* data, PtrSize dataSize);

	/// Generate mipmaps.
	void generateMipmaps(U depth, U face);

	/// Copy a single surface from one texture to another.
	static void copy(const TextureImpl& src,
		const TextureSurfaceInfo& srcSurf,
		const TextureImpl& dest,
		const TextureSurfaceInfo& destSurf);

	void bind();

	void clear(const TextureSurfaceInfo& surf, const ClearValue& clearValue);

	U computeSurfaceIdx(const TextureSurfaceInfo& surf) const;
};
/// @}

} // end namespace anki
