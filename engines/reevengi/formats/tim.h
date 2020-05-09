/* ResidualVM - A 3D game interpreter
 *
 * ResidualVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the AUTHORS
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

#ifndef REEVENGI_TIM_H
#define REEVENGI_TIM_H

#include "graphics/surface.h"
#include "image/image_decoder.h"

namespace Common {
class SeekableReadStream;
}

namespace Reevengi {

class TimDecoder : public Image::ImageDecoder {
public:
	TimDecoder();
	virtual ~TimDecoder();

	// ImageDecoder API
	virtual void destroy();
	virtual bool loadStream(Common::SeekableReadStream &str);
	virtual const Graphics::Surface *getSurface() const { return &_surface; }

private:
	// Color-map:
	byte *_colorMap;
	int16 _colorMapCount;	/* Number of color maps */
	int16 _colorMapLength;	/* Number of colors per color map */

	Graphics::PixelFormat _format;
	Graphics::Surface _surface;

	// Loading helpers
	bool readHeader(Common::SeekableReadStream &tim, byte &imageType);
	bool readData(Common::SeekableReadStream &tim, byte imageType);
	bool readColorMap(Common::SeekableReadStream &tim, byte imageType);
};

} // End of namespace Reevengi

#endif
