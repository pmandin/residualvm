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


/* FIXME: Emulate CD reading from file */

#include "common/system.h"
#include "engines/advancedDetector.h"
#include "video/psx_decoder.h"

#include "engines/reevengi/movie/psx.h"
#include "engines/reevengi/reevengi.h"


namespace Reevengi {

MoviePlayer *CreatePsxPlayer() {
	return new PsxPlayer();
}

PsxPlayer::PsxPlayer() : MoviePlayer() {
	_videoDecoder = new Video::PSXStreamDecoder(Video::PSXStreamDecoder::kCD2x);
}

bool PsxPlayer::loadFile(const Common::String &filename) {
	//_fname = Common::String("Video/") + filename + ".pss";
	_fname = filename;

	Common::SeekableReadStream *stream = SearchMan.createReadStreamForMember(_fname);
	if (!stream)
		return false;

	_videoDecoder->setDefaultHighColorFormat(Graphics::PixelFormat(4, 8, 8, 8, 0, 8, 16, 24, 0));
	_videoDecoder->loadStream(stream);
	_videoDecoder->start();

	return true;
}

} // end of namespace Reevengi
