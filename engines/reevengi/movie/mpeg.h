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

#ifndef REEVENGI_MPEG_PLAYER_H
#define REEVENGI_MPEG_PLAYER_H

#include "engines/reevengi/movie/movie.h"

#ifdef USE_MPEG2

namespace Reevengi {

class MpegPlayer : public MoviePlayer {
public:
	MpegPlayer();
private:
	bool loadFile(const Common::String &filename) override;
	//bool _demo;
};

} // end of namespace Reevengi

#endif // USE_MPEG2

#endif