// Copyright (C) 2009-2015, Panagiotis Christopoulos Charitos.
// All rights reserved.
// Code licensed under the BSD License.
// http://www.anki3d.org/LICENSE

#include "anki/gl/GlQueue.h"
#include "anki/gl/GlCommandBuffer.h"
#include "anki/gl/GlSyncHandles.h"
#include "anki/gl/GlDevice.h"
#include "anki/util/Logger.h"
#include "anki/core/Counters.h"

namespace anki {

//==============================================================================
GlQueue::GlQueue(GlDevice* device, 
	AllocAlignedCallback allocCb, void* allocCbUserData)
:	m_device(device), 
	m_allocCb(allocCb),
	m_allocCbUserData(allocCbUserData),
	m_tail(0), 
	m_head(0),
	m_renderingThreadSignal(0),
	m_thread("anki_gl")
{
	ANKI_ASSERT(m_device);
}

//==============================================================================
GlQueue::~GlQueue()
{}

//==============================================================================
void GlQueue::flushCommandBuffer(GlCommandBufferHandle& commands)
{
	commands._get().makeImmutable();

#if !ANKI_QUEUE_DISABLE_ASYNC
	{
		LockGuard<Mutex> lock(m_mtx);

		// Set commands
		U64 diff = m_tail - m_head;

		if(diff < m_queue.size())
		{
			U64 idx = m_tail % m_queue.size();

			m_queue[idx] = commands;
			++m_tail;
		}
		else
		{
			ANKI_LOGW("Rendering queue to small");
		}
	}

	m_condVar.notifyOne(); // Wake the thread
#else
	Error err = commands._executeAllCommands();
	if(err)
	{
		ANKI_LOGE("Error in command buffer");
	}
#endif
}

//==============================================================================
void GlQueue::finishCommandBuffer(GlCommandBufferHandle& commands)
{
#if !ANKI_QUEUE_DISABLE_ASYNC
	flushCommandBuffer(commands);

	flushCommandBuffer(m_syncCommands);
	m_sync.wait();
#else
	flushCommandBuffer(commands);
#endif
}

//==============================================================================
Error GlQueue::start(
	GlMakeCurrentCallback makeCurrentCb, void* makeCurrentCbData, void* ctx,
	GlCallback swapBuffersCallback, void* swapBuffersCbData,
	Bool registerMessages)
{
	Error err = ErrorCode::NONE;

	ANKI_ASSERT(m_tail == 0 && m_head == 0);
	m_state.m_registerMessages = registerMessages;

	// Context
	ANKI_ASSERT(ctx != nullptr && makeCurrentCb != nullptr);
	m_ctx = ctx;
	m_makeCurrentCbData = makeCurrentCbData;
	m_makeCurrentCb = makeCurrentCb;

	// Swap buffers stuff
	ANKI_ASSERT(swapBuffersCallback != nullptr);
	m_swapBuffersCallback = swapBuffersCallback;
	m_swapBuffersCbData = swapBuffersCbData;
	err = m_swapBuffersCommands.create(m_device);
	if(!err)
	{
		m_swapBuffersCommands.pushBackUserCommand(swapBuffersInternal, this);
	}

#if !ANKI_QUEUE_DISABLE_ASYNC
	Bool threadStarted = false;
	if(!err)
	{
		// Start thread
		m_thread.start(this, threadCallback);
		threadStarted = true;

		// Create sync command buffer
		err = m_syncCommands.create(m_device);
	}

	if(!err)
	{
		err = m_sync.create(m_syncCommands);
	}

	if(!err)
	{
		m_sync.sync(m_syncCommands);
	}

	if(err && threadStarted)
	{
		err = m_thread.join();
	}
#else
	prepare();
#endif

	return err;
}

//==============================================================================
void GlQueue::stop()
{
#if !ANKI_QUEUE_DISABLE_ASYNC
	{
		LockGuard<Mutex> lock(m_mtx);
		m_renderingThreadSignal = 1;

		// Set some dummy values in order to unlock the cond var
		m_tail = m_queue.size() + 1;
		m_head = m_tail + 1;
	}
	Error err = m_thread.join();
	(void)err;
#else
	finish();
#endif
}

//==============================================================================
void GlQueue::prepare()
{
	ANKI_ASSERT(m_makeCurrentCb && m_ctx);
	(*m_makeCurrentCb)(m_makeCurrentCbData, m_ctx);

	// Ignore the first error
	glGetError();

	ANKI_LOGI("OpenGL async thread started: OpenGL version %s, GLSL version %s",
		reinterpret_cast<const char*>(glGetString(GL_VERSION)),
		reinterpret_cast<const char*>(
		glGetString(GL_SHADING_LANGUAGE_VERSION)));

	// Get thread id
	m_serverThreadId = Thread::getCurrentThreadId();

	// Init state
	m_state.init();

	// Create default VAO
	glGenVertexArrays(1, &m_defaultVao);
	glBindVertexArray(m_defaultVao);
}

//==============================================================================
void GlQueue::finish()
{
	// Iterate the queue and release the refcounts
	for(U i = 0; i < m_queue.size(); i++)
	{
		if(m_queue[i].isCreated())
		{
			// Fake that it's executed to avoid warnings
			m_queue[i]._get().makeExecuted();

			// Release
			m_queue[i] = GlCommandBufferHandle();
		}
	}

	// Delete default VAO
	glDeleteVertexArrays(1, &m_defaultVao);

	// Cleanup
	glFinish();
	(*m_makeCurrentCb)(m_makeCurrentCbData, nullptr);
}

//==============================================================================
Error GlQueue::threadCallback(Thread::Info& info)
{
	GlQueue* queue = reinterpret_cast<GlQueue*>(info.m_userData);
	queue->threadLoop();
	return ErrorCode::NONE;
}

//==============================================================================
void GlQueue::threadLoop()
{
	prepare();

	while(1)
	{
		GlCommandBufferHandle cmd;

		// Wait for something
		{
			LockGuard<Mutex> lock(m_mtx);
			while(m_tail == m_head)
			{
				m_condVar.wait(m_mtx);
			}

			// Check signals
			if(m_renderingThreadSignal == 1)
			{
				// Requested to stop
				break;
			}

			U64 idx = m_head % m_queue.size();
			// Pop a command
			cmd = m_queue[idx];
			m_queue[idx] = GlCommandBufferHandle(); // Insert empty cmd buffer

			++m_head;
		}

		Error err = cmd._executeAllCommands();

		if(err)
		{
			ANKI_LOGE("Error in rendering thread. Aborting\n");
			abort();
		}
	}

	finish();
}

//==============================================================================
void GlQueue::syncClientServer()
{
#if !ANKI_QUEUE_DISABLE_ASYNC
	flushCommandBuffer(m_syncCommands);
	m_sync.wait();
#endif
}

//==============================================================================
Error GlQueue::swapBuffersInternal(void* ptr)
{
	ANKI_ASSERT(ptr);
	GlQueue& self = *reinterpret_cast<GlQueue*>(ptr);

	// Do the swap buffers
	self.m_swapBuffersCallback(self.m_swapBuffersCbData);

	// Notify the main thread that we are done
	{
		LockGuard<Mutex> lock(self.m_frameMtx);
		self.m_frameWait = false;
	}

	self.m_frameCondVar.notifyOne();

	return ErrorCode::NONE;
}

//==============================================================================
void GlQueue::swapBuffers()
{
	// Wait for the rendering thread to finish swap buffers...
	{
		LockGuard<Mutex> lock(m_frameMtx);
		while(m_frameWait)
		{
			ANKI_COUNTER_START_TIMER(GL_SERVER_WAIT_TIME);
			m_frameCondVar.wait(m_frameMtx);
			ANKI_COUNTER_STOP_TIMER_INC(GL_SERVER_WAIT_TIME);
		}

		m_frameWait = true;
	}

	// ...and then flush a new swap buffers
	flushCommandBuffer(m_swapBuffersCommands);
}

} // end namespace anki

