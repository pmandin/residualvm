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

#include "common/debug.h"
#include "common/memstream.h"
#include "common/stream.h"

#include "engines/reevengi/formats/pak.h"
#include "engines/reevengi/re1/re1.h"

namespace Reevengi {

/*--- Defines ---*/

#define NUM_COUNTRIES 8

/*--- Constant ---*/

static const char *re1_country[NUM_COUNTRIES]={
	"horr/usa",
	"horr/ger",
	"horr/jpn",
	"horr/fra",
	"usa",
	"ger",
	"jpn",
	"fra"
};

static const char *RE1PCGAME_BG = "%s/stage%d/rc%d%02x%d.pak";

RE1Engine::RE1Engine(OSystem *syst, ReevengiGameType gameType, const ADGameDescription *desc) :
		ReevengiEngine(syst, gameType, desc) {
	_room = 6;
}

RE1Engine::~RE1Engine() {
}

void RE1Engine::initPreRun(void) {
	char filePath[32];

	/* Country detect */
	for (int i=0; i<NUM_COUNTRIES; i++) {
		sprintf(filePath, "%s/data/capcom.ptc", re1_country[i]);

		if (SearchMan.hasFile(filePath)) {
			_country = i;
			debug(3, "re1: country %d", i);
			break;
		}
	}
}

void RE1Engine::loadBgImage(void) {
	debug(3, "re1: loadBgImage");

	/* Stages 6,7 use images from stages 1,2 */
	int stage = (_stage>5 ? _stage-5 : _stage);

	switch(_gameDesc.platform) {
		case Common::kPlatformWindows:
			{
				loadBgImagePc(stage);
			}
			break;
		case Common::kPlatformPSX:
			{
				loadBgImagePsx(stage);
			}
			break;
		default:
			return;
	}

	ReevengiEngine::loadBgImage();
}

void RE1Engine::loadBgImagePc(int stage) {
	char *filePath;

	filePath = (char *) malloc(strlen(RE1PCGAME_BG)+32);
	if (!filePath) {
		return;
	}
	sprintf(filePath, RE1PCGAME_BG, re1_country[_country], stage, stage, _room, _camera);

	Common::SeekableReadStream *stream = SearchMan.createReadStreamForMember(filePath);
	if (stream) {
		_bgImage = new PakDecoder();
		((PakDecoder *) _bgImage)->loadStream(*stream);
	}
	delete stream;

	free(filePath);
}

void RE1Engine::loadBgImagePsx(int stage) {
	// TODO
}

} // end of namespace Reevengi

