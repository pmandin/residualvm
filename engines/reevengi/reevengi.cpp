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

#include "common/config-manager.h"
#include "common/debug.h"
#include "common/error.h"
#include "common/events.h"
#include "common/scummsys.h"
#include "common/system.h"
#include "engines/util.h"
#include "graphics/pixelbuffer.h"
#include "graphics/renderer.h"
#include "graphics/screen.h"
#include "graphics/surface.h"
#include "image/bmp.h"
#include "image/jpeg.h"

#ifdef USE_OPENGL
#include "graphics/opengl/context.h"
#endif

#include "engines/reevengi/reevengi.h"
#include "engines/reevengi/formats/adt.h"
#include "engines/reevengi/formats/pak.h"
#include "engines/reevengi/formats/tim.h"
#include "engines/reevengi/game/clock.h"
#include "engines/reevengi/game/room.h"
#include "engines/reevengi/gfx/gfx_base.h"
#include "engines/reevengi/gfx/gfx_opengl.h"
#include "engines/reevengi/gfx/gfx_tinygl.h"
#include "engines/reevengi/movie/movie.h"

namespace Reevengi {

GfxBase *g_driver = nullptr;

ReevengiEngine::ReevengiEngine(OSystem *syst, ReevengiGameType gameType, const ADGameDescription *gameDesc) :
	Engine(syst), _gameType(gameType), _character(0), _softRenderer(true),
	_stage(1), _room(0), _camera(0), _bgImage(nullptr), _roomScene(nullptr) {
	memcpy(&_gameDesc, gameDesc, sizeof(_gameDesc));
	g_movie = nullptr;
	_clock = new Clock();
}

ReevengiEngine::~ReevengiEngine() {
	destroyBgImage();
	destroyRoom();

	delete g_movie;
	g_movie = nullptr;

	delete _clock;
	_clock = nullptr;
}

bool ReevengiEngine::hasFeature(EngineFeature f) const {
	return (f == kSupportsArbitraryResolutions);
}

GfxBase *ReevengiEngine::createRenderer(int screenW, int screenH, bool fullscreen) {
	Common::String rendererConfig = ConfMan.get("renderer");
	Graphics::RendererType desiredRendererType = Graphics::parseRendererTypeCode(rendererConfig);
	Graphics::RendererType matchingRendererType = Graphics::getBestMatchingAvailableRendererType(desiredRendererType);

	_softRenderer = matchingRendererType == Graphics::kRendererTypeTinyGL;
	g_system->setupScreen(screenW, screenH, fullscreen, !_softRenderer);

#if defined(USE_OPENGL)
	// Check the OpenGL context actually supports shaders
	if (matchingRendererType == Graphics::kRendererTypeOpenGLShaders && !OpenGLContext.shadersSupported) {
		matchingRendererType = Graphics::kRendererTypeOpenGL;
	}
#endif

	if (matchingRendererType != desiredRendererType && desiredRendererType != Graphics::kRendererTypeDefault) {
		// Display a warning if unable to use the desired renderer
		warning("Unable to create a '%s' renderer", rendererConfig.c_str());
	}

	GfxBase *renderer = nullptr;
/*#if defined(USE_GLES2) || defined(USE_OPENGL_SHADERS)
	if (matchingRendererType == Graphics::kRendererTypeOpenGLShaders) {
		renderer = CreateGfxOpenGLShader();
	}
#endif*/
#if defined(USE_OPENGL) /*&& !defined(USE_GLES2)*/
	if (matchingRendererType == Graphics::kRendererTypeOpenGL) {
		renderer = CreateGfxOpenGL();
	}
#endif
	if (matchingRendererType == Graphics::kRendererTypeTinyGL) {
		renderer = CreateGfxTinyGL();
	}

	if (!renderer) {
		error("Unable to create a '%s' renderer", rendererConfig.c_str());
	}

	renderer->setupScreen(screenW, screenH, fullscreen);
	return renderer;
}

void ReevengiEngine::initPreRun(void) {
}

Common::Error ReevengiEngine::run() {
	initPreRun();

	bool fullscreen = ConfMan.getBool("fullscreen");
	g_driver = createRenderer(640, 480, fullscreen);

	//TimDecoder *my_image = testLoadImage();
	//testLoadMovie();
	loadRoom();
	loadBgImage();

	while (!shouldQuit()) {

		if (_bgImage) {
			testDisplayImage(_bgImage);
		}
		//testDisplayImage(my_image);
		//testPlayMovie();

		// Tell the system to update the screen.
		g_driver->flipBuffer();

		// Get new events from the event manager so the window doesn't appear non-responsive.
		processEvents();

		// FIXME: Continue processing input events, till game tic elapsed
		_clock->waitGameTic();
	}

	g_driver->releaseMovieFrame();
	//delete my_image;

	return Common::kNoError;
}

void ReevengiEngine::processEvents(void) {
	Common::Event e;
	while (g_system->getEventManager()->pollEvent(e)) {
		// Handle any buttons, keys and joystick operations

		if (isPaused()) {
			// Only pressing key P to resume the game is allowed when the game is paused
			if (e.type == Common::EVENT_KEYDOWN && e.kbd.keycode == Common::KEYCODE_p) {
				pauseEngine(false);
			}
			continue;
		}

		if (e.type == Common::EVENT_KEYDOWN) {
			if (e.kbdRepeat) {
				continue;
			}

			processEventsKeyDown(e);

			if (e.kbd.keycode == Common::KEYCODE_d && (e.kbd.hasFlags(Common::KBD_CTRL))) {
				/*_console->attach();*/
				/*_console->onFrame();*/
			} else if ((e.kbd.keycode == Common::KEYCODE_RETURN || e.kbd.keycode == Common::KEYCODE_KP_ENTER)
						&& e.kbd.hasFlags(Common::KBD_ALT)) {
					//StarkGfx->toggleFullscreen();
			} else if (e.kbd.keycode == Common::KEYCODE_p) {
				/*if (StarkUserInterface->isInGameScreen()) {
					pauseEngine(true);
					debug("The game is paused");
				}*/
			} else {
				//StarkUserInterface->handleKeyPress(e.kbd);
			}

		} else if (e.type == Common::EVENT_SCREEN_CHANGED) {
			//debug(3, "onScreenChanged");
			onScreenChanged();
		}
	}
}

void ReevengiEngine::processEventsKeyDown(Common::Event e) {
	bool updateBgImage = false;
	bool updateRoom = false;

	/* Depend on game/demo */
	if (e.kbd.keycode == Common::KEYCODE_z) {
		--_stage;
		if (_stage<1) {
			_stage=7;
		}
		updateBgImage = true;
		updateRoom = true;
	}
	if (e.kbd.keycode == Common::KEYCODE_s) {
		++_stage;
		if (_stage>7) {
			_stage=1;
		}
		updateBgImage = true;
		updateRoom = true;
	}
	if (e.kbd.keycode == Common::KEYCODE_x) {
		_stage=1;
		updateBgImage = true;
		updateRoom = true;
	}

	/* Depend on game/stage */
	if (e.kbd.keycode == Common::KEYCODE_e) {
		--_room;
		if (_room<0) {
			_room=0x1c;
		}
		updateBgImage = true;
		updateRoom = true;
	}
	if (e.kbd.keycode == Common::KEYCODE_d) {
		++_room;
		if (_room>0x1c) {
			_room=0;
		}
		updateBgImage = true;
		updateRoom = true;
	}
	if (e.kbd.keycode == Common::KEYCODE_c) {
		_room=0;
		updateBgImage = true;
		updateRoom = true;
	}

	/* Room dependant */
	if (e.kbd.keycode == Common::KEYCODE_r) {
		--_camera;
		updateBgImage = true;
	}
	if (e.kbd.keycode == Common::KEYCODE_f) {
		++_camera;
		updateBgImage = true;
	}
	if (e.kbd.keycode == Common::KEYCODE_v) {
		_camera=0;
		updateBgImage = true;
	}

	debug(3, "switch to stage %d, room %d, camera %d", _stage, _room, _camera);

	if (updateRoom) {
		destroyRoom();
		loadRoom();
	}
	if (updateBgImage) {
		destroyBgImage();
		loadBgImage();
	}
}

void ReevengiEngine::onScreenChanged(void) {
	bool changed = g_driver->computeScreenViewport();
}

void ReevengiEngine::destroyBgImage(void) {
	delete _bgImage;
	_bgImage = nullptr;
}

void ReevengiEngine::loadBgImage(void) {
	if (!_bgImage) {
		return;
	}

	Graphics::Surface *bgSurf = (Graphics::Surface *) _bgImage->getSurface();
	if (bgSurf) {
		g_driver->prepareMovieFrame(bgSurf);
	}
}

void ReevengiEngine::destroyRoom(void) {
	delete _roomScene;
	_roomScene = nullptr;
}

void ReevengiEngine::loadRoom(void) {
	//
}

TimDecoder *ReevengiEngine::testLoadImage(void) {
	/*debug(3, "loading jopt06.tim");
	Common::SeekableReadStream *s1 = SearchMan.createReadStreamForMember("jopt06.tim");
	TimDecoder *my_image1 = new TimDecoder();
	my_image1->loadStream(*s1);*/

	debug(3, "loading gwarning.adt");
	Common::SeekableReadStream *s3 = SearchMan.createReadStreamForMember("common/datp/gwarning.adt");
	AdtDecoder *my_image1 = new AdtDecoder();
	my_image1->loadStream(*s3);

	/*debug(3, "loading rc1060.pak");
	Common::SeekableReadStream *s2 = SearchMan.createReadStreamForMember("rc1060.pak");
	PakDecoder *my_image1 = new PakDecoder();
	my_image1->loadStream(*s2);*/

	if (my_image1) {
		Graphics::Surface *surf = (Graphics::Surface *) my_image1->getSurface();
		if (surf) {
			g_driver->prepareMovieFrame(surf);
		}
	}

	return my_image1;
}

void ReevengiEngine::testDisplayImage(Image::ImageDecoder *img) {
	g_driver->drawMovieFrame(0, 0);
}

void ReevengiEngine::testLoadMovie(void) {
	g_movie = CreatePsxPlayer();
	g_movie->play("capcom.str", false, 0, 0);
	//g_movie = CreateAviPlayer();
	//g_movie->play("sample.avi", false, 0, 0);
	//g_movie = CreateMpegPlayer();
	//g_movie->play("zmovie/roopne.dat", false, 0, 0);
}

void ReevengiEngine::testPlayMovie(void) {

	//if (g_movie->getMovieTime()>1000) { g_movie->pause(true); }

	if (g_movie->isPlaying() /*&& _movieSetup == _currSet->getCurrSetup()->_name*/) {
		//_movieTime = g_movie->getMovieTime();
		if (g_movie->isUpdateNeeded()) {
			Graphics::Surface *frame = g_movie->getDstSurface();
			if (frame) {
				g_driver->clearScreen();
				g_driver->prepareMovieFrame(frame);
			}
			g_movie->clearUpdateNeeded();
		}
		if (g_movie->getFrame() >= 0)
			g_driver->drawMovieFrame(0, 0);
		else
			g_driver->releaseMovieFrame();
	}
}

} // End of namespace Reevengi
