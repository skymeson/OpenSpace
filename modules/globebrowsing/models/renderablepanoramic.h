/*****************************************************************************************
*                                                                                       *
* OpenSpace                                                                             *
*                                                                                       *
* Copyright (c) 2014-2017                                                               *
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

#ifndef __OPENSPACE_MODULE_GLOBEBROWSING___RENDERABLE_PANORAMIC___H__
#define __OPENSPACE_MODULE_GLOBEBROWSING___RENDERABLE_PANORAMIC___H__

#include <openspace/rendering/renderable.h>
#include <openspace/properties/stringproperty.h>
#include <ghoul/opengl/texture.h>

namespace openspace {

class SphereGeometry;
namespace globebrowsing {
class RenderablePanoramic : public Renderable {

public:
	RenderablePanoramic(const ghoul::Dictionary& dictionary);
	~RenderablePanoramic() = default;

	bool initialize() override;
	bool deinitialize() override;

	bool isReady() const override;

	void render(const RenderData& data) override;
	void update(const UpdateData& data) override;

	void loadTexture();
	
private:
	properties::StringProperty _texturePath;
	float _size;

	properties::IntProperty _segments;

	std::unique_ptr<ghoul::opengl::ProgramObject> _shader;
	std::unique_ptr<ghoul::opengl::Texture> _texture;

	SphereGeometry* _sphere;

	double prevYaw;

};
}
}

#endif // __OPENSPACE_MODULE_GLOBEBROWSING___RenderablePanoramic___H__