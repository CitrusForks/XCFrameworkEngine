/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

/** @file  version.h
 *  @brief Functions to query the version of the Assimp runtime, check
 *    compile flags, ...
 */
#ifndef INCLUDED_AI_VERSION_H
#define INCLUDED_AI_VERSION_H

#include "defs.h"

#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------------------------------
/** @brief Returns a string with legal copyright and licensing information
 *  about Assimp. The string may include multiple lines.
 *  @return Pointer to static string.
 */
ASSIMP_API const char*  aiGetLegalString  (void);

// ---------------------------------------------------------------------------
/** @brief Returns the current minor version number of Assimp.
 *  @return Minor version of the Assimp runtime the application was
 *    linked/built against
 */
ASSIMP_API unsigned int aiGetVersionMinor (void);

// ---------------------------------------------------------------------------
/** @brief Returns the current major version number of Assimp.
 *  @return Major version of the Assimp runtime the application was
 *    linked/built against
 */
ASSIMP_API unsigned int aiGetVersionMajor (void);

// ---------------------------------------------------------------------------
/** @brief Returns the repository revision of the Assimp runtime.
 *  @return SVN Repository revision number of the Assimp runtime the
 *          application was linked/built against.
 */
ASSIMP_API unsigned int aiGetVersionRevision (void);

//! Assimp was compiled as a shared object (Windows: DLL)
#define ASSIMP_CFLAGS_SHARED  0x1
//! Assimp was compiled against STLport
#define ASSIMP_CFLAGS_STLPORT 0x2
//! Assimp was compiled as a debug build
#define ASSIMP_CFLAGS_DEBUG   0x4

//! Assimp was compiled with ASSIMP_BUILD_BOOST_WORKAROUND defined
#define ASSIMP_CFLAGS_NOBOOST           0x8
//! Assimp was compiled with ASSIMP_BUILD_SINGLETHREADED defined
#define ASSIMP_CFLAGS_SINGLETHREADED    0x10

// ---------------------------------------------------------------------------
/** @brief Returns assimp's compile flags
 *  @return Any bitwise combination of the ASSIMP_CFLAGS_xxx constants.
 */
ASSIMP_API unsigned int aiGetCompileFlags (void);

#ifdef __cplusplus
} // end extern "C"
#endif

#endif // !! #ifndef INCLUDED_AI_VERSION_H
