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

#include "engines/stark/services/userinterface.h"

#include "engines/stark/gfx/driver.h"
#include "engines/stark/gfx/texture.h"

#include "engines/stark/resources/level.h"
#include "engines/stark/resources/location.h"
#include "engines/stark/resources/speech.h"
#include "engines/stark/services/global.h"
#include "engines/stark/services/services.h"

namespace Stark {

UserInterface::UserInterface(Gfx::Driver *driver) {
	_gfx = driver;
}

UserInterface::~UserInterface() {
}

void UserInterface::skipCurrentSpeeches() {
	Global *global = StarkServices::instance().global;
	Current *current = global->getCurrent();

	if (!current) {
		return; // No current location, nothing to do
	}

	// Get all speeches
	Common::Array<Resources::Speech *> speeches;
	speeches.push_back(global->getLevel()->listChildrenRecursive<Resources::Speech>());
	speeches.push_back(current->getLevel()->listChildrenRecursive<Resources::Speech>());
	speeches.push_back(current->getLocation()->listChildrenRecursive<Resources::Speech>());

	// Stop them
	for (uint i = 0; i < speeches.size(); i++) {
		Resources::Speech *speech = speeches[i];
		if (speech->isPlaying()) {
			speech->stop();
		}
	}
}

void UserInterface::scrollLocation(int32 dX, int32 dY) {
	Global *global = StarkServices::instance().global;
	Current *current = global->getCurrent();

	if (!current) {
		return; // No current location, nothing to do
	}

	Resources::Location *location = current->getLocation();

	Common::Point scroll = location->getScrollPosition();
	scroll.x += dX;
	scroll.y += dY;
	location->setScrollPosition(scroll);
}

void UserInterface::render() {
	Common::String debugStr;

	Global *global = StarkServices::instance().global;
	Current *current = global->getCurrent();

	int32 chapter = global->getCurrentChapter();

	debugStr += Common::String::format("location: %02x %02x ", current->getLevel()->getIndex(), current->getLocation()->getIndex());
	debugStr += current->getLevel()->getName() + ", " + current->getLocation()->getName();
	debugStr += Common::String::format(" chapter: %d", chapter);

	
	Gfx::Texture *debugTexture = _gfx->createTextureFromString(debugStr, 0xF0FF0F00);
	_gfx->drawSurface(debugTexture, Common::Point(0,0));
	delete debugTexture;
}

} // End of namespace Stark
