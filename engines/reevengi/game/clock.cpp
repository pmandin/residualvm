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

#include "common/system.h"

#include "engines/reevengi/game/clock.h"

namespace Reevengi {

Clock::Clock(): runningTime(0), paused(false) {
	startPause = endPause = g_system->getMillis();
}

Clock::~Clock() {
}

void Clock::pause(void) {
	if (paused) {
		return;
	}

	paused = true;
	startPause = g_system->getMillis();
	/* endPause is end of previous pause */
	runningTime += startPause - endPause;
}

void Clock::unpause(void) {
	if (!paused) {
		return;
	}

	paused = false;
	endPause = g_system->getMillis();
}

uint32 Clock::getRunningTime(void) {
	if (paused) {
		return runningTime;
	}

	return runningTime + g_system->getMillis() - endPause;
}

uint32 Clock::getGameTic(void) {
	return (getRunningTime() * 30) / 1000;
}

} // End of namespace Reevengi
