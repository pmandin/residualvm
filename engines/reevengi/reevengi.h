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

#ifndef REEVENGI_H
#define REEVENGI_H

#include "engines/engine.h"

struct ADGameDescription;

namespace Reevengi {

class GfxBase;
class TimDecoder;

enum ReevengiGameType {
	RType_None,
	RType_RE1,
	RType_RE2_LEON,
	RType_RE2_CLAIRE,
	RType_RE3
};

class ReevengiEngine : public Engine {
public:
	ReevengiEngine(OSystem *syst, ReevengiGameType gameType, const ADGameDescription *gameDesc);
	~ReevengiEngine() override;

	virtual Common::Error run(void);

protected:
	ReevengiGameType _gameType;
	int _character;
	int _stage, _room, _camera;

	bool hasFeature(EngineFeature f) const override;
	GfxBase *createRenderer(int screenW, int screenH, bool fullscreen);

private:
	bool _softRenderer;

	void processEvents(void);
	void onScreenChanged(void);

	TimDecoder *testLoadImage(void);
	void testDisplayImage(TimDecoder *img);
	void testLoadMovie(void);
	void testPlayMovie(void);

};

} // End of namespace Reevengi

#endif
