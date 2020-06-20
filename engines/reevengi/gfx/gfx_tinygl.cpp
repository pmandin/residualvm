/* ResidualVM - A 3D game interpreter
 *
 * ResidualVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "common/config-manager.h"
#include "common/debug.h"
#include "common/scummsys.h"
#include "common/system.h"
#include "math/glmath.h"

#include "engines/reevengi/gfx/gfx_base.h"
#include "engines/reevengi/gfx/gfx_tinygl.h"

namespace Reevengi {

GfxBase *CreateGfxTinyGL() {
	return new GfxTinyGL();
}

GfxTinyGL::GfxTinyGL() {
}

GfxTinyGL::~GfxTinyGL() {
}

byte *GfxTinyGL::setupScreen(int screenW, int screenH, bool fullscreen) {
	Graphics::PixelBuffer buf = g_system->getScreenPixelBuffer();
	byte *buffer = buf.getRawBuffer();

	_screenWidth = screenW;
	_screenHeight = screenH;
	_scaleW = _screenWidth / (float)_gameWidth;
	_scaleH = _screenHeight / (float)_gameHeight;

	debug(3, "%dx%d -> %dx%d", _gameWidth, _gameHeight, _screenWidth, _screenHeight);

	g_system->showMouse(!fullscreen);

	g_system->setWindowCaption("ResidualVM: Software 3D Renderer");

	_pixelFormat = buf.getFormat();
	debug("INFO: TinyGL front buffer pixel format: %s", _pixelFormat.toString().c_str());
	_zb = new TinyGL::FrameBuffer(screenW, screenH, buf);
	TinyGL::glInit(_zb, 256);
	tglEnableDirtyRects(ConfMan.getBool("dirtyrects"));

	_storedDisplay.create(_pixelFormat, _gameWidth * _gameHeight, DisposeAfterUse::YES);
	_storedDisplay.clear(_gameWidth * _gameHeight);

	//_currentShadowArray = nullptr;

	TGLfloat ambientSource[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	tglLightModelfv(TGL_LIGHT_MODEL_AMBIENT, ambientSource);
	TGLfloat diffuseReflectance[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	tglMaterialfv(TGL_FRONT, TGL_DIFFUSE, diffuseReflectance);

	return buffer;
}

const char *GfxTinyGL::getVideoDeviceName() {
	return "TinyGL Software Renderer";
}

void GfxTinyGL::clearScreen() {
	tglClear(TGL_COLOR_BUFFER_BIT | TGL_DEPTH_BUFFER_BIT);
}

void GfxTinyGL::clearDepthBuffer() {
	tglClear(TGL_DEPTH_BUFFER_BIT);
}

void GfxTinyGL::flipBuffer() {
	TinyGL::tglPresentBuffer();
	g_system->updateScreen();
}

bool GfxTinyGL::isHardwareAccelerated() {
	return false;
}

bool GfxTinyGL::supportsShaders() {
	return false;
}

void GfxTinyGL::prepareMovieFrame(Graphics::Surface *frame) {
	if (_smushImage == nullptr)
		_smushImage = Graphics::tglGenBlitImage();
	Graphics::tglUploadBlitImage(_smushImage, *frame, 0, false);

	_smushWidth = frame->w;
	_smushHeight = frame->h;
}

void GfxTinyGL::drawMovieFrame(int offsetX, int offsetY) {
	int sysW = g_system->getWidth();
	int sysH = g_system->getHeight();

	float movScale = MIN<float>((float) sysW / _smushWidth, (float) sysH / _smushHeight);
	int movW = _smushWidth * movScale;
	int movH = _smushHeight * movScale;

	offsetX = (int)(offsetX * movScale);
	offsetY = (int)(offsetY * movScale);

	offsetX += (sysW-movW)>>1;
	offsetY += (sysH-movH)>>1;

	if ((movW==_smushWidth) && (movH==_smushHeight)) {
		Graphics::tglBlitFast(_smushImage, offsetX, offsetY);
		return;
	}

	Graphics::BlitTransform bltTransform(offsetX, offsetY);
	bltTransform.scale(movW, movH);

	Graphics::tglBlit(_smushImage, bltTransform);
}

void GfxTinyGL::releaseMovieFrame() {
	Graphics::tglDeleteBlitImage(_smushImage);
}

void GfxTinyGL::prepareMaskedFrame(Graphics::Surface *frame) {
}

void GfxTinyGL::drawMaskedFrame(void) {
}

void GfxTinyGL::releaseMaskedFrame(void) {
}

void GfxTinyGL::setProjection(float angle, float aspect, float zNear, float zFar) {
	Math::Matrix4 mProjection = Math::makePerspectiveMatrix(angle, aspect, zNear, zFar);

	tglMatrixMode(TGL_PROJECTION);
	tglLoadIdentity();
	tglMultMatrixf(mProjection.getData());

	tglMatrixMode(TGL_MODELVIEW);
	tglLoadIdentity();
}

void GfxTinyGL::setModelview(float fromX, float fromY, float fromZ,
	float toX, float toY, float toZ, float upX, float upY, float upZ) {
	Math::Matrix4 mLookAt = Math::makeLookAtMatrix(Math::Vector3d(fromX, fromY, fromZ),
		Math::Vector3d(toX, toY, toZ), Math::Vector3d(upX, upY, upZ));

	tglMatrixMode(TGL_MODELVIEW);
	tglLoadIdentity();
	tglMultMatrixf(mLookAt.getData());

	tglTranslatef(-fromX, -fromY, -fromZ);
}

void GfxTinyGL::rotate(float angle, float ax, float ay, float az) {
	tglRotatef(angle, ax, ay, az);
}

void GfxTinyGL::translate(float tx, float ty, float tz) {
	tglTranslatef(tx, ty, tz);
}

void GfxTinyGL::setBlending(bool enable) {
	if (enable) {
		tglEnable(TGL_BLEND);
		tglBlendFunc(TGL_SRC_ALPHA, TGL_ONE_MINUS_SRC_ALPHA);

		tglEnable(TGL_ALPHA_TEST);
		tglAlphaFunc(TGL_GREATER, 0.5f);
	} else {
		tglDisable(TGL_BLEND);
		tglDisable(TGL_ALPHA_TEST);
	}
}

void GfxTinyGL::setColor(float r, float g, float b) {
	tglColor3f(r, g, b);
}

void GfxTinyGL::setColorMask(bool enable) {
	tglColorMask(enable ? TGL_TRUE : TGL_FALSE, enable ? TGL_TRUE : TGL_FALSE,
		enable ? TGL_TRUE : TGL_FALSE, enable ? TGL_TRUE : TGL_FALSE);
}

void GfxTinyGL::setDepth(bool enable) {
	if (enable) {
		tglEnable(TGL_DEPTH_TEST);
	} else {
		tglDisable(TGL_DEPTH_TEST);
	}
}

void GfxTinyGL::line(Math::Vector3d v0, Math::Vector3d v1) {
	tglDisable(TGL_LIGHTING);
	tglDisable(TGL_TEXTURE_2D);
	tglDisable(TGL_DEPTH_TEST);

	tglBegin(TGL_LINES);
		tglVertex3f(v0.x(), v0.y(), v0.z());
		tglVertex3f(v1.x(), v1.y(), v1.z());
	tglEnd();
}

} // End of namespace Reevengi
