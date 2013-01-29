// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <tchar.h>
#endif
#include <stdio.h>

#include "AppConfig.h"
#include "../juce/modules/juce_core/juce_core.h"
#include "../juce/modules/juce_gui_basics/juce_gui_basics.h"
#include "../juce/modules/juce_data_structures/juce_data_structures.h"
#include "../juce/modules/juce_events/juce_events.h"
#include "../juce/modules/juce_graphics/juce_graphics.h"
#include "../juce/modules/juce_video/juce_video.h"
#include "../juce/modules/juce_gui_extra/juce_gui_extra.h"
#include "../juce/modules/juce_cryptography/juce_cryptography.h"

using namespace juce;
// TODO: reference additional headers your program requires here
