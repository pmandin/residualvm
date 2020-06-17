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

#define RDT1_RVD_BOUNDARY 9

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

/* Cameras and masks */

typedef struct {
	uint32 priOffset;
	uint32 timOffset;
	int32 fromX, fromY, fromZ;
	int32 toX, toY, toZ;
	uint32 unknown[3];
} rdt1_rid_t;

typedef struct {
	uint16 numOffset;
	uint16 numMasks;
} rdt1_pri_header_t;

typedef struct {
	uint16 count;
	uint16 unknown;
	int16 dstX, dstY;
} rdt1_pri_offset_t;

typedef struct {
	uint8 srcX, srcY;
	uint8 dstX, dstY;
	uint16 depth;
	uint8 unknown;
	uint8 size;
} rdt1_pri_square_t;

typedef struct {
	uint8 srcX, srcY;
	uint8 dstX, dstY;
	uint16 depth, zero;
	uint16 width, height;
} rdt1_pri_rect_t;

/* Cameras switches, offset 0 */

typedef struct {
	uint16 toCam, fromCam;	/* to = RDT_RVD_BOUNDARY if boundary, not camera switch */
	int16 x1,y1; /* Coordinates to use to calc when player crosses switch zone */
	int16 x2,y2;
	int16 x3,y3;
	int16 x4,y4;
} rdt1_rvd_t;

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

int RE1Room::checkCamSwitch(int curCam, Math::Vector2d fromPos, Math::Vector2d toPos) {
	if (!_roomPtr)
		return -1;

	int32 offset = FROM_LE_32( ((rdt1_header_t *) _roomPtr)->offsets[RDT1_OFFSET_CAM_SWITCHES] );
	rdt1_rvd_t *camSwitchArray = (rdt1_rvd_t *) ((byte *) &_roomPtr[offset]);

	while (FROM_LE_16(camSwitchArray->toCam) != 0xffff) {
		bool isBoundary = (FROM_LE_16(camSwitchArray->toCam) == RDT1_RVD_BOUNDARY);

		if (isBoundary) {
			/* boundary, not a switch */
		} else {
			/* Check objet triggered camera switch */
			Math::Vector2d quad[4];
			quad[0] = Math::Vector2d(FROM_LE_16(camSwitchArray->x1), FROM_LE_16(camSwitchArray->y1));
			quad[1] = Math::Vector2d(FROM_LE_16(camSwitchArray->x2), FROM_LE_16(camSwitchArray->y2));
			quad[2] = Math::Vector2d(FROM_LE_16(camSwitchArray->x3), FROM_LE_16(camSwitchArray->y3));
			quad[3] = Math::Vector2d(FROM_LE_16(camSwitchArray->x4), FROM_LE_16(camSwitchArray->y4));

			if ((curCam==camSwitchArray->fromCam) && !isInside(fromPos, quad) && isInside(toPos, quad)) {
				return camSwitchArray->toCam;
			}
		}

		++camSwitchArray;
	}

	return -1;
}

bool RE1Room::checkCamBoundary(int curCam, Math::Vector2d fromPos, Math::Vector2d toPos) {
	if (!_roomPtr)
		return false;

	int32 offset = FROM_LE_32( ((rdt1_header_t *) _roomPtr)->offsets[RDT1_OFFSET_CAM_SWITCHES] );
	rdt1_rvd_t *camBoundaryArray = (rdt1_rvd_t *) ((byte *) &_roomPtr[offset]);

	while (FROM_LE_16(camBoundaryArray->toCam) != 0xffff) {
		bool isBoundary = (FROM_LE_16(camBoundaryArray->toCam) == RDT1_RVD_BOUNDARY);

		if (isBoundary) {
			/* Check objet got outside boundary */
			Math::Vector2d quad[4];
			quad[0] = Math::Vector2d(FROM_LE_16(camBoundaryArray->x1), FROM_LE_16(camBoundaryArray->y1));
			quad[1] = Math::Vector2d(FROM_LE_16(camBoundaryArray->x2), FROM_LE_16(camBoundaryArray->y2));
			quad[2] = Math::Vector2d(FROM_LE_16(camBoundaryArray->x3), FROM_LE_16(camBoundaryArray->y3));
			quad[3] = Math::Vector2d(FROM_LE_16(camBoundaryArray->x4), FROM_LE_16(camBoundaryArray->y4));

			if ((curCam==camBoundaryArray->fromCam) && isInside(fromPos, quad) && !isInside(toPos, quad)) {
				return true;
			}
		} else {
			/* Switch, not a boundary */
		}

		++camBoundaryArray;
	}

	return false;
}

} // End of namespace Reevengi
