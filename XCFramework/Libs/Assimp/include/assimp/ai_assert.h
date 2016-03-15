/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#ifndef AI_DEBUG_H_INC
#define AI_DEBUG_H_INC

#ifdef ASSIMP_BUILD_DEBUG
#   include <assert.h>
#   define  ai_assert(expression) assert(expression)
#else
#   define  ai_assert(expression)
#endif


#endif
