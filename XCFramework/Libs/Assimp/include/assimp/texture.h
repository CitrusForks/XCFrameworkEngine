/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

/** @file texture.h
 *  @brief Defines texture helper structures for the library
 *
 * Used for file formats which embed their textures into the model file.
 * Supported are both normal textures, which are stored as uncompressed
 * pixels, and "compressed" textures, which are stored in a file format
 * such as PNG or TGA.
 */

#ifndef AI_TEXTURE_H_INC
#define AI_TEXTURE_H_INC

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif


// --------------------------------------------------------------------------------
/** @def AI_MAKE_EMBEDDED_TEXNAME
 *  Used to build the reserved path name used by the material system to
 *  reference textures that are embedded into their corresponding
 *  model files. The parameter specifies the index of the texture
 *  (zero-based, in the aiScene::mTextures array)
 */
#if (!defined AI_MAKE_EMBEDDED_TEXNAME)
#   define AI_MAKE_EMBEDDED_TEXNAME(_n_) "*" # _n_
#endif


#include "./Compiler/pushpack1.h"

// --------------------------------------------------------------------------------
/** @brief Helper structure to represent a texel in a ARGB8888 format
*
*  Used by aiTexture.
*/
struct aiTexel
{
    unsigned char b,g,r,a;

#ifdef __cplusplus
    //! Comparison operator
    bool operator== (const aiTexel& other) const
    {
        return b == other.b && r == other.r &&
               g == other.g && a == other.a;
    }

    //! Inverse comparison operator
    bool operator!= (const aiTexel& other) const
    {
        return b != other.b || r != other.r ||
               g != other.g || a != other.a;
    }

    //! Conversion to a floating-point 4d color
    operator aiColor4D() const
    {
        return aiColor4D(r/255.f,g/255.f,b/255.f,a/255.f);
    }
#endif // __cplusplus

} PACK_STRUCT;

#include "./Compiler/poppack1.h"

// --------------------------------------------------------------------------------
/** Helper structure to describe an embedded texture
 *
 * Normally textures are contained in external files but some file formats embed
 * them directly in the model file. There are two types of embedded textures:
 * 1. Uncompressed textures. The color data is given in an uncompressed format.
 * 2. Compressed textures stored in a file format like png or jpg. The raw file
 * bytes are given so the application must utilize an image decoder (e.g. DevIL) to
 * get access to the actual color data.
 *
 * Embedded textures are referenced from materials using strings like "*0", "*1", etc.
 * as the texture paths (a single asterisk character followed by the
 * zero-based index of the texture in the aiScene::mTextures array).
 */
struct aiTexture
{
    /** Width of the texture, in pixels
     *
     * If mHeight is zero the texture is compressed in a format
     * like JPEG. In this case mWidth specifies the size of the
     * memory area pcData is pointing to, in bytes.
     */
    unsigned int mWidth;

    /** Height of the texture, in pixels
     *
     * If this value is zero, pcData points to an compressed texture
     * in any format (e.g. JPEG).
     */
    unsigned int mHeight;

    /** A hint from the loader to make it easier for applications
     *  to determine the type of embedded compressed textures.
     *
     * If mHeight != 0 this member is undefined. Otherwise it
     * is set set to '\\0\\0\\0\\0' if the loader has no additional
     * information about the texture file format used OR the
     * file extension of the format without a trailing dot. If there
     * are multiple file extensions for a format, the shortest
     * extension is chosen (JPEG maps to 'jpg', not to 'jpeg').
     * E.g. 'dds\\0', 'pcx\\0', 'jpg\\0'.  All characters are lower-case.
     * The fourth character will always be '\\0'.
     */
    char achFormatHint[4];

    /** Data of the texture.
     *
     * Points to an array of mWidth * mHeight aiTexel's.
     * The format of the texture data is always ARGB8888 to
     * make the implementation for user of the library as easy
     * as possible. If mHeight = 0 this is a pointer to a memory
     * buffer of size mWidth containing the compressed texture
     * data. Good luck, have fun!
     */
    C_STRUCT aiTexel* pcData;

#ifdef __cplusplus

    //! For compressed textures (mHeight == 0): compare the
    //! format hint against a given string.
    //! @param s Input string. 3 characters are maximally processed.
    //!        Example values: "jpg", "png"
    //! @return true if the given string matches the format hint
    bool CheckFormat(const char* s) const
    {
        return (0 == ::strncmp(achFormatHint,s,3));
    }

    // Construction
    aiTexture ()
        : mWidth  (0)
        , mHeight (0)
        , pcData  (NULL)
    {
        achFormatHint[0] = achFormatHint[1] = 0;
        achFormatHint[2] = achFormatHint[3] = 0;
    }

    // Destruction
    ~aiTexture ()
    {
        delete[] pcData;
    }
#endif
};


#ifdef __cplusplus
}
#endif

#endif // AI_TEXTURE_H_INC
