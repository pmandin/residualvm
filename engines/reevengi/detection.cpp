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

#include "engines/advancedDetector.h"
#include "engines/reevengi/reevengi.h"

namespace Reevengi {

static const PlainGameDescriptor reevengiGames[] = {
	{ "re1", "Resident Evil" },
	{ "re2", "Resident Evil 2" },
	{ "re3", "Resident Evil 3" },
	{ nullptr, nullptr }
};

static const char *directoryGlobs[] = {
	/* RE1 */
	"horr",
	"usa", "ger", "jpn", "fra",
	"data",
	/* RE2 */
	"pl0", "pl1", "regist",
	"zmovie",
	0
};

#define REEVENGI_ENTRY(gameid, lang, platform, filename, extra)  \
	{                                                            \
		gameid,                                                  \
		extra,                                                   \
		{                                                        \
			{ filename, 0, nullptr, -1 },                        \
			{ nullptr, 0, nullptr, -1 },                         \
		},                                                       \
		lang,                                                    \
		platform,                                                \
		ADGF_NO_FLAGS,                                           \
		GUIO_NONE                                                \
	},

static const ADGameDescription gameDescriptions[] = {
	REEVENGI_ENTRY("re1", Common::EN_USA, Common::kPlatformWindows, "horr/usa/data/capcom.ptc", "")
	REEVENGI_ENTRY("re1", Common::DE_DEU, Common::kPlatformWindows, "horr/ger/data/capcom.ptc", "")
	REEVENGI_ENTRY("re1", Common::JA_JPN, Common::kPlatformWindows, "horr/jpn/data/capcom.ptc", "")
	REEVENGI_ENTRY("re1", Common::FR_FRA, Common::kPlatformWindows, "horr/fra/data/capcom.ptc", "")
	REEVENGI_ENTRY("re1", Common::EN_USA, Common::kPlatformWindows, "usa/data/capcom.ptc", "")
	REEVENGI_ENTRY("re1", Common::DE_DEU, Common::kPlatformWindows, "ger/data/capcom.ptc", "")
	REEVENGI_ENTRY("re1", Common::JA_JPN, Common::kPlatformWindows, "jpn/data/capcom.ptc", "")
	REEVENGI_ENTRY("re1", Common::FR_FRA, Common::kPlatformWindows, "fra/data/capcom.ptc", "")

	REEVENGI_ENTRY("re1", Common::JA_JPN, Common::kPlatformPSX, "slpm_800.27", "Trial")
	REEVENGI_ENTRY("re1", Common::JA_JPN, Common::kPlatformPSX, "ntsc.exe", "1.0")
	REEVENGI_ENTRY("re1", Common::EN_GRB, Common::kPlatformPSX, "sles_002.00", "")
	REEVENGI_ENTRY("re1", Common::FR_FRA, Common::kPlatformPSX, "sles_002.27", "")
	REEVENGI_ENTRY("re1", Common::DE_DEU, Common::kPlatformPSX, "sles_002.28", "")
	REEVENGI_ENTRY("re1", Common::EN_GRB, Common::kPlatformPSX, "sles_009.69", "Director's Cut")
	REEVENGI_ENTRY("re1", Common::FR_FRA, Common::kPlatformPSX, "sles_009.70", "Director's Cu")
	REEVENGI_ENTRY("re1", Common::DE_DEU, Common::kPlatformPSX, "sles_009.71", "Director's Cut")
	REEVENGI_ENTRY("re1", Common::JA_JPN, Common::kPlatformPSX, "slpm_867.70", "5th Anniversary LE")
	REEVENGI_ENTRY("re1", Common::JA_JPN, Common::kPlatformPSX, "slps_002.22", "")
	REEVENGI_ENTRY("re1", Common::JA_JPN, Common::kPlatformPSX, "slps_009.98", "Director's Cut")
	REEVENGI_ENTRY("re1", Common::JA_JPN, Common::kPlatformPSX, "slps_015.12", "Director's Cut Dual Shock")
	REEVENGI_ENTRY("re1", Common::EN_USA, Common::kPlatformPSX, "slus_001.70", "")
	REEVENGI_ENTRY("re1", Common::EN_USA, Common::kPlatformPSX, "slus_005.51", "Director's Cut")
	REEVENGI_ENTRY("re1", Common::EN_USA, Common::kPlatformPSX, "slus_007.47", "Director's Cut Dual Shock")

	REEVENGI_ENTRY("re2", Common::EN_ANY, Common::kPlatformWindows, "pl0/zmovie/r108l.bin", "Leon")
	REEVENGI_ENTRY("re2", Common::EN_ANY, Common::kPlatformWindows, "pl1/zmovie/r108l.bin", "Claire")
	REEVENGI_ENTRY("re2", Common::EN_ANY, Common::kPlatformWindows, "regist/leonp.exe", "Preview")
	REEVENGI_ENTRY("re2", Common::EN_ANY, Common::kPlatformWindows, "regist/leonu.exe", "Preview")

	REEVENGI_ENTRY("re2", Common::EN_GRB, Common::kPlatformPSX, "sced_003.60", "Preview")
	REEVENGI_ENTRY("re2", Common::EN_GRB, Common::kPlatformPSX, "sced_008.27", "Preview")
	REEVENGI_ENTRY("re2", Common::EN_GRB, Common::kPlatformPSX, "sled_009.77", "Preview")
	REEVENGI_ENTRY("re2", Common::JA_JPN, Common::kPlatformPSX, "slps_009.99", "Trial Edition")
	REEVENGI_ENTRY("re2", Common::EN_USA, Common::kPlatformPSX, "slus_900.09", "Preview")
	REEVENGI_ENTRY("re2", Common::EN_GRB, Common::kPlatformPSX, "sced_011.14", "Preview")
	REEVENGI_ENTRY("re2", Common::EN_GRB, Common::kPlatformPSX, "sles_009.72", "Leon")
	REEVENGI_ENTRY("re2", Common::FR_FRA, Common::kPlatformPSX, "sles_009.73", "Leon")
	REEVENGI_ENTRY("re2", Common::DE_DEU, Common::kPlatformPSX, "sles_009.74", "Leon")
	REEVENGI_ENTRY("re2", Common::IT_ITA, Common::kPlatformPSX, "sles_009.75", "Leon")
	REEVENGI_ENTRY("re2", Common::ES_ESP, Common::kPlatformPSX, "sles_009.76", "Leon")
	REEVENGI_ENTRY("re2", Common::EN_GRB, Common::kPlatformPSX, "sles_009.72", "Leon")
	REEVENGI_ENTRY("re2", Common::JA_JPN, Common::kPlatformPSX, "slps_012.22", "Leon")
	REEVENGI_ENTRY("re2", Common::JA_JPN, Common::kPlatformPSX, "slps_015.10", "Leon Dual Shock")
	REEVENGI_ENTRY("re2", Common::EN_USA, Common::kPlatformPSX, "slus_004.21", "Leon")
	REEVENGI_ENTRY("re2", Common::EN_USA, Common::kPlatformPSX, "slus_007.48", "Leon Dual Shock")
	REEVENGI_ENTRY("re2", Common::EN_GRB, Common::kPlatformPSX, "sles_109.72", "Claire")
	REEVENGI_ENTRY("re2", Common::FR_FRA, Common::kPlatformPSX, "sles_109.73", "Claire")
	REEVENGI_ENTRY("re2", Common::DE_DEU, Common::kPlatformPSX, "sles_109.74", "Claire")
	REEVENGI_ENTRY("re2", Common::IT_ITA, Common::kPlatformPSX, "sles_109.75", "Claire")
	REEVENGI_ENTRY("re2", Common::ES_ESP, Common::kPlatformPSX, "sles_109.76", "Claire")
	REEVENGI_ENTRY("re2", Common::EN_GRB, Common::kPlatformPSX, "sles_109.72", "Claire")
	REEVENGI_ENTRY("re2", Common::JA_JPN, Common::kPlatformPSX, "slps_012.23", "Claire")
	REEVENGI_ENTRY("re2", Common::JA_JPN, Common::kPlatformPSX, "slps_015.11", "Claire Dual Shock")
	REEVENGI_ENTRY("re2", Common::EN_USA, Common::kPlatformPSX, "slus_005.92", "Claire")
	REEVENGI_ENTRY("re2", Common::EN_USA, Common::kPlatformPSX, "slus_007.56", "Claire Dual Shock")

	REEVENGI_ENTRY("re3", Common::EN_ANY, Common::kPlatformWindows, "rofs2.dat", "")
	REEVENGI_ENTRY("re3", Common::EN_ANY, Common::kPlatformWindows, "rofs1.dat", "Preview")

	REEVENGI_ENTRY("re3", Common::EN_GRB, Common::kPlatformPSX, "sles_025.28", "")
	REEVENGI_ENTRY("re3", Common::EN_GRB, Common::kPlatformPSX, "sles_025.29", "")
	REEVENGI_ENTRY("re3", Common::FR_FRA, Common::kPlatformPSX, "sles_025.30", "")
	REEVENGI_ENTRY("re3", Common::DE_DEU, Common::kPlatformPSX, "sles_025.31", "")
	REEVENGI_ENTRY("re3", Common::ES_ESP, Common::kPlatformPSX, "sles_025.32", "")
	REEVENGI_ENTRY("re3", Common::IT_ITA, Common::kPlatformPSX, "sles_025.33", "")
	REEVENGI_ENTRY("re3", Common::EN_GRB, Common::kPlatformPSX, "sles_026.98", "")
	REEVENGI_ENTRY("re3", Common::JA_JPN, Common::kPlatformPSX, "slps_023.00", "")
	REEVENGI_ENTRY("re3", Common::EN_USA, Common::kPlatformPSX, "slus_009.23", "")
	REEVENGI_ENTRY("re3", Common::EN_USA, Common::kPlatformPSX, "slus_900.64", "Trial")

	AD_TABLE_END_MARKER
};

class ReevengiMetaEngine : public AdvancedMetaEngine {
public:
	ReevengiMetaEngine() : AdvancedMetaEngine(gameDescriptions, sizeof(ADGameDescription), reevengiGames, nullptr) {
		_maxScanDepth = 4;
		_directoryGlobs = directoryGlobs;
		_matchFullPaths = true;
	}

	const char *getName() const override {
		return "Reevengi";
	}

	const char *getEngineId() const override {
		return "reevengi";
	}

	const char *getOriginalCopyright() const override {
		return "(C) Capcom";
	}

	bool createInstance(OSystem *syst, Engine **engine, const ADGameDescription *desc) const override {
		if (desc)
			*engine = new ReevengiEngine(syst, desc);

		return desc != nullptr;
	}
};

} // End of namespace Reevengi

#if PLUGIN_ENABLED_DYNAMIC(REEVENGI)
	REGISTER_PLUGIN_DYNAMIC(REEVENGI, PLUGIN_TYPE_ENGINE, Reevengi::ReevengiMetaEngine);
#else
	REGISTER_PLUGIN_STATIC(REEVENGI, PLUGIN_TYPE_ENGINE, Reevengi::ReevengiMetaEngine);
#endif
