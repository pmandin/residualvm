/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
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

/*
 * This file is based on WME Lite.
 * http://dead-code.org/redir.php?target=wmelite
 * Copyright (c) 2011 Jan Nedoma
 */


#include "engines/wintermute/base/base_game.h"
#include "engines/wintermute/base/gfx/opengl/render_ticket.h"
#include "engines/wintermute/base/gfx/opengl/base_surface_opengl_texture.h"
#include "graphics/transform_tools.h"
#include "common/textconsole.h"

namespace Wintermute {

RenderTicketOpenGL::RenderTicketOpenGL(BaseSurfaceOpenGLTexture *owner, const Graphics::Surface *surf, Common::Rect *srcRect, Common::Rect *dstRect, Graphics::TransformStruct transform) :
	_owner(owner),
	_srcRect(*srcRect),
	_dstRect(*dstRect),
	_isValid(true),
	_wantsDraw(true),
	_transform(transform) {
	if (surf) {
		_surface = new Graphics::Surface();
		_surface->create((uint16)srcRect->width(), (uint16)srcRect->height(), surf->format);
		assert(_surface->format.bytesPerPixel == 4);
		// Get a clipped copy of the surface
		for (int i = 0; i < _surface->h; i++) {
			memcpy(_surface->getBasePtr(0, i), surf->getBasePtr(srcRect->left, srcRect->top + i), srcRect->width() * _surface->format.bytesPerPixel);
		}
		// Then scale it if necessary
		//
		// NB: The numTimesX/numTimesY properties don't yet mix well with
		// scaling and rotation, but there is no need for that functionality at
		// the moment.
		// NB: Mirroring and rotation are probably done in the wrong order.
		// (Mirroring should most likely be done before rotation. See also
		// TransformTools.)
		if (_transform._angle != Graphics::kDefaultAngle) {
			Graphics::TransparentSurface src(*_surface, false);
			Graphics::Surface *temp;
			if (owner->_gameRef->getBilinearFiltering()) {
				temp = src.rotoscaleT<Graphics::FILTER_BILINEAR>(transform);
			} else {
				temp = src.rotoscaleT<Graphics::FILTER_NEAREST>(transform);
			}
			_surface->free();
			delete _surface;
			_surface = temp;
		} else if ((dstRect->width() != srcRect->width() ||
					dstRect->height() != srcRect->height()) &&
					_transform._numTimesX * _transform._numTimesY == 1) {
			Graphics::TransparentSurface src(*_surface, false);
			Graphics::Surface *temp;
			if (owner->_gameRef->getBilinearFiltering()) {
				temp = src.scaleT<Graphics::FILTER_BILINEAR>(dstRect->width(), dstRect->height());
			} else {
				temp = src.scaleT<Graphics::FILTER_NEAREST>(dstRect->width(), dstRect->height());
			}
			_surface->free();
			delete _surface;
			_surface = temp;
		}
	} else {
		_surface = nullptr;
	}
}

RenderTicketOpenGL::~RenderTicketOpenGL() {
	if (_surface) {
		_surface->free();
		delete _surface;
	}
}

bool RenderTicketOpenGL::operator==(const RenderTicketOpenGL &t) const {
	if ((t._owner != _owner) ||
		(t._transform != _transform)  ||
		(t._dstRect != _dstRect) ||
		(t._srcRect != _srcRect)
	) {
		return false;
	}
	return true;
}

// Replacement for SDL2's SDL_RenderCopy
void RenderTicketOpenGL::drawToSurface(Graphics::Surface *_targetSurface) const {
	Graphics::Surface* converted_surface = getSurface()->convertTo(Graphics::PixelFormat(4, 8, 8, 8, 8, 24, 16, 8, 0));
	Graphics::TransparentSurface src(*converted_surface, false);

	Common::Rect clipRect;
	clipRect.setWidth(getSurface()->w);
	clipRect.setHeight(getSurface()->h);

	if (_owner) {
		if (_transform._alphaDisable) {
			src.setAlphaMode(Graphics::ALPHA_OPAQUE);
		} else if (_transform._angle) {
			src.setAlphaMode(Graphics::ALPHA_FULL);
		} else {
			src.setAlphaMode(_owner->getAlphaType());
		}
	}

	int y = _dstRect.top;
	int w = _dstRect.width() / _transform._numTimesX;
	int h = _dstRect.height() / _transform._numTimesY;

	for (int ry = 0; ry < _transform._numTimesY; ++ry) {
		int x = _dstRect.left;
		for (int rx = 0; rx < _transform._numTimesX; ++rx) {
			src.blit(*_targetSurface, x, y, _transform._flip, &clipRect, _transform._rgbaMod, clipRect.width(), clipRect.height());
			x += w;
		}
		y += h;
	}

	delete converted_surface;
}

void RenderTicketOpenGL::drawToSurface(Graphics::Surface *_targetSurface, Common::Rect *dstRect, Common::Rect *clipRect) const {
	Graphics::Surface* converted_surface = getSurface()->convertTo(Graphics::PixelFormat(4, 8, 8, 8, 8, 24, 16, 8, 0));
	Graphics::TransparentSurface src(*converted_surface, false);
	bool doDelete = false;
	if (!clipRect) {
		doDelete = true;
		clipRect = new Common::Rect();
		clipRect->setWidth(getSurface()->w * _transform._numTimesX);
		clipRect->setHeight(getSurface()->h * _transform._numTimesY);
	}

	if (_owner) {
		if (_transform._alphaDisable) {
			src.setAlphaMode(Graphics::ALPHA_OPAQUE);
		} else if (_transform._angle) {
			src.setAlphaMode(Graphics::ALPHA_FULL);
		} else {
			src.setAlphaMode(_owner->getAlphaType());
		}
	}

	if (_transform._numTimesX * _transform._numTimesY == 1) {

		src.blit(*_targetSurface, dstRect->left, dstRect->top, _transform._flip, clipRect, _transform._rgbaMod, clipRect->width(), clipRect->height(), _transform._blendMode);

	} else {

		// clipRect is a subrect of the full numTimesX*numTimesY rect
		Common::Rect subRect;

		int y = 0;
		int w = getSurface()->w;
		int h = getSurface()->h;
		assert(w == _dstRect.width() / _transform._numTimesX);
		assert(h == _dstRect.height() / _transform._numTimesY);

		int basex = dstRect->left - clipRect->left;
		int basey = dstRect->top - clipRect->top;

		for (int ry = 0; ry < _transform._numTimesY; ++ry) {
			int x = 0;
			for (int rx = 0; rx < _transform._numTimesX; ++rx) {

				subRect.left = x;
				subRect.top = y;
				subRect.setWidth(w);
				subRect.setHeight(h);

				if (subRect.intersects(*clipRect)) {
					subRect.clip(*clipRect);
					subRect.translate(-x, -y);
					src.blit(*_targetSurface, basex + x + subRect.left, basey + y + subRect.top, _transform._flip, &subRect, _transform._rgbaMod, subRect.width(), subRect.height(), _transform._blendMode);

				}

				x += w;
			}
			y += h;
		}
	}

	if (doDelete) {
		delete clipRect;
	}

	delete converted_surface;
}

} // End of namespace Wintermute
