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

#include "common/archive.h"
#include "common/debug.h"
#include "common/memstream.h"
#include "common/stream.h"
#include "image/jpeg.h"

#include "engines/reevengi/formats/bss.h"
#include "engines/reevengi/formats/rofs.h"
#include "engines/reevengi/formats/tim.h"
#include "engines/reevengi/re3/re3.h"

namespace Reevengi {

/*--- Constant ---*/

static const char *RE3PCROFS_DAT = "rofs%d.dat";
static const char *RE3PC_BG = "data_a/bss/r%d%02x%02x.jpg";

static const char *RE3PSX_BG = "cd_data/stage%d/r%d%02x.bss";

RE3Engine::RE3Engine(OSystem *syst, ReevengiGameType gameType, const ADGameDescription *desc) :
		ReevengiEngine(syst, gameType, desc) {
	_room = 13;
}

RE3Engine::~RE3Engine() {
}

void RE3Engine::initPreRun(void) {
	char filePath[32];

	switch(_gameDesc.platform) {
		case Common::kPlatformWindows:
			{
				_gameDesc.flags &= ~ADGF_DEMO;

				/* Use all ROFS<n>.DAT files as archives */
				for (int i=1;i<16;i++) {
					sprintf(filePath, RE3PCROFS_DAT, i);

					RofsArchive *archive = new RofsArchive();
					if (archive->open(filePath))
						SearchMan.add(filePath, archive, 0, true);
					else {
						if (i==2) {
							/* no rofs2.dat for demo */
							_gameDesc.flags |= ADGF_DEMO;
						}
						delete archive;
					}
				}
			}
			break;
		case Common::kPlatformPSX:
			{
			}
			break;
		default:
			break;
	}
}

void RE3Engine::loadBgImage(void) {
	debug(3, "re3: loadBgImage");

	switch(_gameDesc.platform) {
		case Common::kPlatformWindows:
			{
				loadBgImagePc();
			}
			break;
		case Common::kPlatformPSX:
			{
				loadBgImagePsx();
			}
			break;
		default:
			return;
	}

	ReevengiEngine::loadBgImage();
}

void RE3Engine::loadBgImagePc(void) {
	char *filePath;

	filePath = (char *) malloc(strlen(RE3PC_BG)+32);
	if (!filePath) {
		return;
	}
	sprintf(filePath, RE3PC_BG, _stage, _room, _camera);

	Common::SeekableReadStream *stream = SearchMan.createReadStreamForMember(filePath);
	if (stream) {
		_bgImage = new Image::JPEGDecoder();
		((Image::JPEGDecoder *) _bgImage)->loadStream(*stream);
	}
	delete stream;

	free(filePath);
}

void RE3Engine::loadBgImagePsx(void) {
	char *filePath;

	filePath = (char *) malloc(strlen(RE3PSX_BG)+8);
	if (!filePath) {
		return;
	}
	sprintf(filePath, RE3PSX_BG, _stage, _stage, _room);

	Common::SeekableReadStream *arcStream = SearchMan.createReadStreamForMember(filePath);
	if (arcStream) {
		byte *imgBuffer = new byte[65536];
		memset(imgBuffer, 0, 65536);

		arcStream->seek(65536 * _camera);
		arcStream->read(imgBuffer, 65536);

		Common::BitStreamMemoryStream *imgStream = new Common::BitStreamMemoryStream(imgBuffer, 65536, DisposeAfterUse::YES);
		if (imgStream) {
			/* FIXME: Would be simpler to call PSXStreamDecoder::PSXVideoTrack::decodeFrame() on imgBuffer
			   instead of duplicating implementation in formats/bss.[cpp,h] */
			PSXVideoTrack *vidDecoder = new PSXVideoTrack(/*imgStream,*/ 1, 32);
			vidDecoder->decodeFrame(imgStream, 32);

			const Graphics::Surface *frame = vidDecoder->decodeNextFrame();
			if (frame) {
				Graphics::PixelFormat fmt;
				memcpy(&fmt, &(frame->format), sizeof(Graphics::PixelFormat));

				_bgImage = new TimDecoder();
				((TimDecoder *)_bgImage)->CreateTimSurface(frame->w, frame->h, fmt);

				const Graphics::Surface *dstFrame = _bgImage->getSurface();

				const byte *src = (const byte *) frame->getPixels();
				byte *dst = (byte *) dstFrame->getPixels();
				if (frame->pitch == dstFrame->pitch) {
					memcpy(dst, src, frame->h * frame->pitch);
				} else {
					for (int y = frame->h; y > 0; --y) {
						memcpy(dst, src, frame->w * fmt.bytesPerPixel);
						src += frame->pitch;
						dst += dstFrame->pitch;
					}
				}
			}

			delete vidDecoder;
		}
		delete imgStream;
	}
	delete arcStream;
}

} // end of namespace Reevengi

