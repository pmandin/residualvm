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

#include "common/endian.h"
#include "common/stream.h"

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

typedef struct {
	uint16 unk0;
	uint16 const0; /* 0x683c, or 0x73b7 */
	/* const0>>7 used for engine */
	int32 fromX, fromY, fromZ;
	int32 toX, toY, toZ;
	uint32 masksOffset;
} rdt2_rid_t;

RE2Room::RE2Room(Common::SeekableReadStream *stream): Room(stream) {
	//
}

int RE2Room::getNumCameras(void) {
	if (!_roomPtr)
		return 0;

	return ((rdt2_header_t *) _roomPtr)->numCameras;
}

void RE2Room::getCameraPos(int numCamera, RdtCameraPos_t *cameraPos) {
	if (!_roomPtr)
		return;

	int32 offset = FROM_LE_32( ((rdt2_header_t *) _roomPtr)->offsets[RDT2_OFFSET_CAMERAS] );
	rdt2_rid_t *cameraPosArray = (rdt2_rid_t *) ((byte *) &_roomPtr[offset]);

	cameraPos->fromX = FROM_LE_32( cameraPosArray[numCamera].fromX );
	cameraPos->fromY = FROM_LE_32( cameraPosArray[numCamera].fromY );
	cameraPos->fromZ = FROM_LE_32( cameraPosArray[numCamera].fromZ );

	cameraPos->toX = FROM_LE_32( cameraPosArray[numCamera].toX );
	cameraPos->toY = FROM_LE_32( cameraPosArray[numCamera].toY );
	cameraPos->toZ = FROM_LE_32( cameraPosArray[numCamera].toZ );
}

} // End of namespace Reevengi
