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

#ifndef REEVENGI_BSSSLD_H
#define REEVENGI_BSSSLD_H

#include "graphics/surface.h"
#include "image/image_decoder.h"

#include "reevengi/formats/tim.h"

namespace Common {
class SeekableReadStream;
}

namespace Reevengi {

class BssSldDecoder : public TimDecoder {
public:
	BssSldDecoder();
	virtual ~BssSldDecoder();

	// ImageDecoder API
	virtual void destroy() override;
	virtual bool loadStream(Common::SeekableReadStream &pak) override;
	virtual const Graphics::Surface *getSurface() const { return TimDecoder::getSurface(); } override;

private:
	uint8 *_dstPointer;
	int _dstBufLen;

	void depack(Common::SeekableReadStream &bsssld);
	void memcpyOverlap(uint8 *dest, uint8 *src, int count);
};

} // End of namespace Reevengi

#endif
