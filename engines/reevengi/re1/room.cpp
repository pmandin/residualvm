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

#include "engines/reevengi/re1/room.h"

namespace Reevengi {

/*--- Defines ---*/

#define RDT1_OFFSET_CAM_SWITCHES	0
#define RDT1_OFFSET_COLLISION		1
#define RDT1_OFFSET_INIT_SCRIPT		6
#define RDT1_OFFSET_ROOM_SCRIPT		7
#define RDT1_OFFSET_EVENTS		8
#define RDT1_OFFSET_TEXT		11

/*--- Types ---*/

/*--- Types ---*/

typedef struct {
	int32	x,y,z;
	uint32	unknown[2];
} rdt1_header_part_t;

typedef struct {
	uint8	unknown0;
	uint8	numCameras;
	uint8	unknown1[4];
	uint16	unknown2[3];
	rdt1_header_part_t	unknown3[3];
	uint32	offsets[19];
} rdt1_header_t;

typedef struct {
	uint32 priOffset; /* see rdt_pri.h */
	uint32 timOffset;	/* see rdt_pri.h */
	int32 fromX, fromY, fromZ;
	int32 toX, toY, toZ;
	uint32 unknown[3];
} rdt1_rid_t;

RE1Room::RE1Room(Common::SeekableReadStream *stream): Room(stream) {
	//
}

int RE1Room::getNumCameras(void) {
	if (!_roomPtr)
		return 0;

	return ((rdt1_header_t *) _roomPtr)->numCameras;
}

void RE1Room::getCameraPos(int numCamera, RdtCameraPos_t *cameraPos) {
	if (!_roomPtr)
		return;

	rdt1_rid_t *cameraPosArray = (rdt1_rid_t *) ((byte *) &_roomPtr[sizeof(rdt1_header_t)]);

	cameraPos->fromX = FROM_LE_32( cameraPosArray[numCamera].fromX );
	cameraPos->fromY = FROM_LE_32( cameraPosArray[numCamera].fromY );
	cameraPos->fromZ = FROM_LE_32( cameraPosArray[numCamera].fromZ );

	cameraPos->toX = FROM_LE_32( cameraPosArray[numCamera].toX );
	cameraPos->toY = FROM_LE_32( cameraPosArray[numCamera].toY );
	cameraPos->toZ = FROM_LE_32( cameraPosArray[numCamera].toZ );
}

} // End of namespace Reevengi
