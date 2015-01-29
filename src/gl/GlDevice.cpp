// Copyright (C) 2009-2015, Panagiotis Christopoulos Charitos.
// All rights reserved.
// Code licensed under the BSD License.
// http://www.anki3d.org/LICENSE

#include "anki/gl/GlDevice.h"
#include "anki/core/Timestamp.h"
#include <cstring>

namespace anki {

//==============================================================================
Error GlDevice::create(
	AllocAlignedCallback alloc, void* allocUserData,
	const CString& cacheDir)
{
	m_alloc = HeapAllocator<U8>(alloc, allocUserData);

	// Allocate cache dir
	auto len = cacheDir.getLength();
	m_cacheDir = reinterpret_cast<char*>(m_alloc.allocate(len + 1));
	std::memcpy(m_cacheDir, &cacheDir[0], len + 1);

	// Create queue
	m_queue = m_alloc.newInstance<GlQueue>(
		this, alloc, allocUserData);

#if ANKI_QUEUE_DISABLE_ASYNC
	ANKI_LOGW("GL queue works in synchronous mode");
#endif

	return ErrorCode::NONE;
}

//==============================================================================
void GlDevice::destroy()
{
	if(m_queue)
	{
		if(m_queueStarted)
		{
			m_queue->stop();
		}

		m_alloc.deleteInstance(m_queue);
	}

	if(m_cacheDir)
	{
		m_alloc.deallocate(m_cacheDir, std::strlen(m_cacheDir) + 1);
	}
}

//==============================================================================
Error GlDevice::startServer(
	GlMakeCurrentCallback makeCurrentCb, void* makeCurrentCbData, void* ctx,
	GlCallback swapBuffersCallback, void* swapBuffersCbData,
	Bool registerDebugMessages)
{
	Error err = m_queue->start(makeCurrentCb, makeCurrentCbData, ctx, 
		swapBuffersCallback, swapBuffersCbData, 
		registerDebugMessages);

	if(!err)
	{
		syncClientServer();

		m_queueStarted = true;
	}

	return err;
}

//==============================================================================
void GlDevice::syncClientServer()
{
	m_queue->syncClientServer();
}

//==============================================================================
void GlDevice::swapBuffers()
{
	m_queue->swapBuffers();
}

//==============================================================================
PtrSize GlDevice::getBufferOffsetAlignment(GLenum target) const
{
	const GlState& state = m_queue->getState();

	if(target == GL_UNIFORM_BUFFER)
	{
		return state.m_uniBuffOffsetAlignment;
	}
	else
	{
		ANKI_ASSERT(target == GL_SHADER_STORAGE_BUFFER);
		return state.m_ssBuffOffsetAlignment;
	}
}

} // end namespace anki
