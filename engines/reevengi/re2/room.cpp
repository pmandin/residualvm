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

#include "common/stream.h"

#include "engines/reevengi/game/room.h"
#include "engines/reevengi/re2/room.h"

namespace Reevengi {

/*--- Defines ---*/

#define RDT2_OFFSET_COLLISION 6
#define RDT2_OFFSET_CAMERAS	7
#define RDT2_OFFSET_CAM_SWITCHES	8
#define RDT2_OFFSET_CAM_LIGHTS	9
#define RDT2_OFFSET_TEXT_LANG1	13
#define RDT2_OFFSET_TEXT_LANG2	14
#define RDT2_OFFSET_INIT_SCRIPT	16
#define RDT2_OFFSET_ROOM_SCRIPT	17
#define RDT2_OFFSET_ANIMS	18

/*--- Types ---*/

typedef struct {
	uint8	unknown0;
	uint8	numCameras;
	uint8	unknown1[6];
	uint32	offsets[21];
} rdt2_header_t;

RE2Room::RE2Room(Common::SeekableReadStream *stream): Room(stream) {
	//
}

int RE2Room::getNumCameras(void) {
	int result = 0;

	if (_roomPtr) {
		result = ((rdt2_header_t *) _roomPtr)->numCameras;
	}

	return result;
}

} // End of namespace Reevengi
