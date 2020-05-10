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

#include "common/error.h"

#include "engines/advancedDetector.h"
#include "engines/util.h"

#include "engines/reevengi/reevengi.h"


namespace Reevengi {

ReevengiEngine::ReevengiEngine(OSystem *syst, ReevengiGameType gameType, const ADGameDescription *gameDesc) :
	Engine(syst), _gameType(gameType) {
	_character = 0;

	_stage = 1;
	_room = 0;
	_camera = 0;
}

ReevengiEngine::~ReevengiEngine() {
}

Common::Error ReevengiEngine::run() {
	initGraphics(320, 240);

	return Common::kNoError;
}

} // End of namespace Reevengi
