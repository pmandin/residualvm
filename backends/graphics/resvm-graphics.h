/* ResidualVM - Graphic Adventure Engine
 *
 * ResidulVM is the legal property of its developers, whose names
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

#ifndef BACKENDS_GRAPHICS_RESVM_ABSTRACT_H
#define BACKENDS_GRAPHICS_RESVM_ABSTRACT_H

#include "common/system.h"
#include "common/noncopyable.h"
#include "common/keyboard.h"

#include "graphics/mode.h"
#include "graphics/palette.h"

#include "backends/graphics/graphics.h"

/**
 * Abstract class for graphics manager. Subclasses
 * implement the real functionality.
 */
class ResVmGraphicsManager : public GraphicsManager {
public:
	// Methods not used by ResidualVM
#ifdef USE_RGB_COLOR
	virtual Common::List<Graphics::PixelFormat> getSupportedFormats() const {
		Common::List<Graphics::PixelFormat> supportedFormats;
		return supportedFormats;
	}
#endif
	virtual void initSize(uint width, uint height, const Graphics::PixelFormat *format = NULL) {}
	virtual void beginGFXTransaction() {}
	virtual OSystem::TransactionError endGFXTransaction() { return OSystem::kTransactionSuccess; }
	virtual void setPalette(const byte *colors, uint start, uint num) {}
	virtual void grabPalette(byte *colors, uint start, uint num) const {}
	virtual void copyRectToScreen(const void *buf, int pitch, int x, int y, int w, int h) {}
	virtual Graphics::Surface *lockScreen() { return nullptr; }
	virtual void unlockScreen() {}
	virtual void fillScreen(uint32 col) {}
	virtual void setShakePos(int shakeXOffset, int shakeYOffset) {};
	virtual void setFocusRectangle(const Common::Rect& rect) {}
	virtual void clearFocusRectangle() {}
	virtual void setMouseCursor(const void *buf, uint w, uint h, int hotspotX, int hotspotY, uint32 keycolor, bool dontScale = false, const Graphics::PixelFormat *format = NULL) {}
	virtual void setCursorPalette(const byte *colors, uint start, uint num) {}

	// ResidualVM specific methods
	virtual void setupScreen(uint screenW, uint screenH, bool fullscreen, bool accel3d) = 0;
	virtual Graphics::PixelBuffer getScreenPixelBuffer() = 0;
	virtual void suggestSideTextures(Graphics::Surface *left, Graphics::Surface *right) = 0;
	virtual void saveScreenshot() {}
	virtual bool lockMouse(bool lock) = 0;
};

#endif
