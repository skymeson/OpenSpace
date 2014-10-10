/*****************************************************************************************
 *                                                                                       *
 * OpenSpace                                                                             *
 *                                                                                       *
 * Copyright (c) 2014                                                                    *
 *                                                                                       *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this  *
 * software and associated documentation files (the "Software"), to deal in the Software *
 * without restriction, including without limitation the rights to use, copy, modify,    *
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to    *
 * permit persons to whom the Software is furnished to do so, subject to the following   *
 * conditions:                                                                           *
 *                                                                                       *
 * The above copyright notice and this permission notice shall be included in all copies *
 * or substantial portions of the Software.                                              *
 *                                                                                       *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,   *
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A         *
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT    *
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF  *
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE  *
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                         *
 ****************************************************************************************/

#ifndef __RENDERENGINE_H__
#define __RENDERENGINE_H__

#include <openspace/scenegraph/scenegraph.h>

#include <openspace/scripting/scriptengine.h>

#include <memory>
#include <string>

#include <openspace/abuffer/abuffer.h>

namespace openspace {

class Camera;

class RenderEngine {
public:
	RenderEngine();
	~RenderEngine();
	
	bool initialize();

    void setSceneGraph(SceneGraph* sceneGraph);
    SceneGraph* sceneGraph();

    Camera* camera() const;
    ABuffer* abuffer() const;

	// sgct wrapped functions
    bool initializeGL();
    void postSynchronizationPreDraw();
    void render();
    void postDraw();

	void takeScreenshot();

	void serialize(std::vector<char>& dataStream, size_t& offset);
	void deserialize(const std::vector<char>& dataStream, size_t& offset);
	
	/**
	 * Returns the Lua library that contains all Lua functions available to affect the
	 * rendering. The functions contained are
	 * - openspace::luascriptfunctions::printScreen
	 * \return The Lua library that contains all Lua functions available to affect the
	 * rendering
	 */
	static scripting::ScriptEngine::LuaLibrary luaLibrary();


private:
	Camera* _mainCamera;
	SceneGraph* _sceneGraph;
	ABuffer* _abuffer;

	bool _takeScreenshot;

	void generateGlslConfig();
};

} // namespace openspace

#endif // __RENDERENGINE_H__
