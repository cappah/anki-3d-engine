// Copyright (C) 2009-2016, Panagiotis Christopoulos Charitos and contributors.
// All rights reserved.
// Code licensed under the BSD License.
// http://www.anki3d.org/LICENSE

#pragma once

#include <anki/gr/GrObject.h>
#include <anki/gr/Texture.h>

namespace anki
{

/// @addtogroup graphics
/// @{

/// GPU sampler.
class Sampler : public GrObject
{
public:
	static const GrObjectType CLASS_TYPE = GrObjectType::SAMPLER;

	/// Construct.
	Sampler(GrManager* manager, U64 hash, GrObjectCache* cache);

	/// Destroy.
	~Sampler();

	/// Access the implementation.
	SamplerImpl& getImplementation()
	{
		return *m_impl;
	}

	/// Initialize it.
	void init(const SamplerInitInfo& init);

private:
	UniquePtr<SamplerImpl> m_impl;
};
/// @}

} // end namespace anki
