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


/* FIXME: Finish emulate CD reading from file */

#include "common/system.h"
#include "engines/advancedDetector.h"
#include "video/psx_decoder.h"

#include "engines/reevengi/movie/psx.h"
#include "engines/reevengi/reevengi.h"


namespace Reevengi {

MoviePlayer *CreatePsxPlayer(bool emul_cd) {
	return new PsxPlayer(emul_cd);
}

PsxPlayer::PsxPlayer(bool emul_cd) : MoviePlayer() {
	_emul_cd = emul_cd;
	_videoDecoder = new Video::PSXStreamDecoder(Video::PSXStreamDecoder::kCD2x);
}

bool PsxPlayer::loadFile(const Common::String &filename) {
	//_fname = Common::String("Video/") + filename + ".pss";
	_fname = filename;

	Common::SeekableReadStream *stream = SearchMan.createReadStreamForMember(_fname);
	if (_emul_cd) {
		stream = new PsxCdStream(stream);
	}
	if (!stream)
		return false;

	_videoDecoder->setDefaultHighColorFormat(Graphics::PixelFormat(4, 8, 8, 8, 0, 8, 16, 24, 0));
	_videoDecoder->loadStream(stream);
	_videoDecoder->start();

	return true;
}

#define RAW_CD_SECTOR_SIZE	2352
#define DATA_CD_SECTOR_SIZE	2048

#define CDXA_TYPE_DATA     0x08
#define CDXA_TYPE_AUDIO    0x04

#define CD_SYNC_SIZE 12
#define CD_SEC_SIZE 4
#define CD_XA_SIZE 8
#define CD_DATA_SIZE 2048

#define STR_MAGIC 0x60010180

PsxCdStream::PsxCdStream(Common::SeekableReadStream *srcStream):
	Common::SeekableReadStream() {
	_srcStream = srcStream;

	_srcStream->seek(0, SEEK_END);
	_size = (_srcStream->pos() / DATA_CD_SECTOR_SIZE) * RAW_CD_SECTOR_SIZE;

	_pos = 0;
	_srcStream->seek(0, SEEK_SET);
}

uint32 PsxCdStream::read(void *dataPtr, uint32 dataSize) {
	uint32 size_read = 0;
	uint8 *buf = (uint8 *) dataPtr;

	//debug(3, "psx: read %d from pos %d", dataSize, _pos);

	while (dataSize>0) {
		int sector_pos = _pos % RAW_CD_SECTOR_SIZE;
		uint32 max_size;
		int pos_data_type = -1; /* need to set data type */
		int is_video = 0;

		//logMsg(2,"cd:  generate sector %d, pos %d, remains %d\n",
		//	emul_cd_pos / RAW_CD_SECTOR_SIZE, sector_pos,
		//	dataSize);
		while ((sector_pos<CD_SYNC_SIZE) && (dataSize>0)) {
			buf[size_read++] = ((sector_pos==0) || (sector_pos==11)) ? 0 : 0xff;
			dataSize--;
			sector_pos++;
			_pos++;
		}
		while ((sector_pos<CD_SYNC_SIZE+CD_SEC_SIZE+CD_XA_SIZE) && (dataSize>0)) {
			if (sector_pos == 0x12) {
				pos_data_type = size_read;
			}
			buf[size_read++] = (sector_pos == 0x11 ? 1 : 0);
			dataSize--;
			sector_pos++;
			_pos++;
		}
		while ((sector_pos<CD_SYNC_SIZE+CD_SEC_SIZE+CD_XA_SIZE+CD_DATA_SIZE) && (dataSize>0)) {
			max_size = CD_SYNC_SIZE+CD_SEC_SIZE+CD_XA_SIZE+CD_DATA_SIZE-sector_pos;
			max_size = (max_size>dataSize) ? dataSize : max_size;
			//debug(3, "cd: reading real data at 0x%08x in file, %d", _srcStream->pos(), max_size);
			_srcStream->read(&buf[size_read], max_size);
			if ((sector_pos == CD_SYNC_SIZE+CD_SEC_SIZE+CD_XA_SIZE) && (max_size>=4)) {
				/* Read first bytes */
				uint32 magic = *((uint32 *) &buf[size_read]);
				is_video = (FROM_BE_32(magic) == STR_MAGIC);
			}
			if (!is_video) {
				// FIXME: Avoid static sound (bad format ?) for now
				memset(&buf[size_read], 0, max_size);
			}
			dataSize -= max_size;
			size_read += max_size;
			sector_pos += max_size;
			_pos += max_size;
		}
		while ((sector_pos<RAW_CD_SECTOR_SIZE) && (dataSize>0)) {
			buf[size_read++] = 0;
			dataSize--;
			sector_pos++;
			_pos++;
		}

		/* set data type */
		if (pos_data_type>=0) {
			if (is_video) {
				//debug(3, "cd: generate video 0x%08x", _srcStream->pos());
				buf[pos_data_type] = CDXA_TYPE_DATA;
			} else {
				//debug(3, "cd: generate audio 0x%08x", _srcStream->pos());
				buf[pos_data_type] = CDXA_TYPE_AUDIO;
			}
		}
	}

	return size_read;
}

bool PsxCdStream::seek(int32 offs, int whence) {
	uint32 srcOffset;
	int sectorNum, sectorPos;

	switch(whence) {
		case SEEK_SET:
		case SEEK_END:
			_pos = offs;
			break;
		case SEEK_CUR:
			_pos += offs;
			break;
	}

	sectorNum = _pos / RAW_CD_SECTOR_SIZE;
	sectorPos = _pos % RAW_CD_SECTOR_SIZE;

	if (sectorPos<CD_SYNC_SIZE+CD_SEC_SIZE+CD_XA_SIZE) {
		sectorPos = 0;
	} else if ((sectorPos>=CD_SYNC_SIZE+CD_SEC_SIZE+CD_XA_SIZE) &&
		(sectorPos<CD_SYNC_SIZE+CD_SEC_SIZE+CD_XA_SIZE+CD_DATA_SIZE))
	{
		sectorPos -= CD_SYNC_SIZE+CD_SEC_SIZE+CD_XA_SIZE;
	} else if (sectorPos>=CD_SYNC_SIZE+CD_SEC_SIZE+CD_XA_SIZE+CD_DATA_SIZE) {
		sectorPos = 0;
		sectorNum++;
	}

	srcOffset = (sectorNum * DATA_CD_SECTOR_SIZE) + sectorPos;
	_srcStream->seek(srcOffset, whence);

	return true;
}

} // end of namespace Reevengi
