#include "ScriptingCommon.h"
#include "SceneNode.h"


WRAP(SceneNode)
{
	class_<SceneNode, noncopyable>("SceneNode", no_init)
		.def("getSceneNodeName", &SceneNode::getSceneNodeName, return_value_policy<reference_existing_object>())
	;
}