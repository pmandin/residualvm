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
#include "common/substream.h"

#include "engines/reevengi/formats/rofs.h"

namespace Reevengi {

RofsArchive::RofsArchive() : Common::Archive() {
	_stream = nullptr;
}

RofsArchive::~RofsArchive() {
	close();
}

bool RofsArchive::open(const Common::String &filename) {
	close();

	_stream = SearchMan.createReadStreamForMember(filename);

	if (!_stream)
		return false;

	if (!isArchive())
		return false;
	//debug(3, "rofs: header ok for %s", filename.c_str());

	char dir0[32], dir1[32];
	readFilename(dir0, sizeof(dir0));
	//debug(3, "rofs: dir0=%s", dir0);

	int32 dirLocation= _stream->readUint32LE() << 3;
	//debug(3, "rofs: dir location=%08x", dirLocation);

	int32 dirLength= _stream->readUint32LE();
	//debug(3, "rofs: dir length=%d", dirLength);

	readFilename(dir1, sizeof(dir1));
	//debug(3, "rofs: dir1=%s", dir1);

	Common::String dirPrefix = dir0;
	dirPrefix += "/";
	dirPrefix += dir1;
	dirPrefix += "/";

	_stream->seek(dirLocation);
	enumerateFiles(dirPrefix);

	return true;
}

void RofsArchive::close() {
	delete _stream; _stream = nullptr;
	_map.clear();
}

bool RofsArchive::hasFile(const Common::String &name) const {
	return _map.contains(name);
}

int RofsArchive::listMembers(Common::ArchiveMemberList &list) const {
	for (FileMap::const_iterator it = _map.begin(); it != _map.end(); it++)
		list.push_back(getMember(it->_key));

	return _map.size();
}

const Common::ArchiveMemberPtr RofsArchive::getMember(const Common::String &name) const {
	return Common::ArchiveMemberPtr(new Common::GenericArchiveMember(name, this));
}

Common::SeekableReadStream *RofsArchive::createReadStreamForMember(const Common::String &name) const {
	if (!_stream || !_map.contains(name))
		return nullptr;

	/*const FileEntry &entry = _map[name];

	if (!entry.compressed) {
		return new Common::SeekableSubReadStream(_stream, entry.offset, entry.offset + entry.uncompressedSize);
	}*/

	// TODO: Return decompressed stream

	return nullptr;
}

/* All rofs<n>.dat files start with this */
static const char rofs_id[21]={
	3, 0, 0, 0,
	1, 0, 0, 0,
	4, 0, 0, 0,
	0, 1, 1, 0,
	0, 4, 0, 0,
	0
};

bool RofsArchive::isArchive(void) {
	byte buf[21];

	_stream->read(buf, 21);

	return (memcmp(buf, rofs_id, sizeof(rofs_id)) == 0);
}

void RofsArchive::readFilename(char *filename, int nameLen) {
	char c;

	memset(filename, 0, nameLen);

	do {
		_stream->read(&c, 1);

		*filename++ = c;
	} while (c != '\0');
}

void RofsArchive::enumerateFiles(Common::String &dirPrefix) {
	int fileCount = _stream->readUint32LE();

	for(int i=0; i<fileCount; i++) {
		int32 fileOffset = _stream->readUint32LE() << 3;
		int32 fileCompSize = _stream->readUint32LE();

		char fileName[32];
		readFilename(fileName, sizeof(fileName));
		//debug(3, " entry %d: %s/%s", i, dirPrefix, fileName);

		Common::String name = dirPrefix;
		name += fileName;
		//debug(3, " entry %d: %s", i, name.c_str());

		FileEntry entry;
		entry.compressed = false;
		entry.uncompressedSize = fileCompSize;
		entry.compressedSize = fileCompSize;
		entry.offset = fileOffset;

		/* Now read file header to check for compression */
		//debug(3, "file %s at 0x%08x", name.c_str(), fileOffset);

		int32 arcPos = _stream->pos();
		_stream->seek(entry.offset);
		readFileHeader(entry);
		_stream->seek(arcPos);

		_map[name] = entry;
	}
}

void RofsArchive::readFileHeader(FileEntry &entry) {
	char ident[8];

	entry.offset += _stream->readUint16LE();
	entry.numBlocks = _stream->readUint16LE();
	entry.uncompressedSize = _stream->readUint32LE();
	_stream->read(ident, 8);
	entry.blkOffset = _stream->pos();

	for(int i=0; i<8; i++) {
		ident[i] ^= ident[7];
	}
	entry.compressed = (strcmp("Hi_Comp", ident)==0);
	//debug(3, " 0x%08x %d %s", entry.blkOffset, entry.numBlocks, ident);
}

} // End of namespace Reevengi


