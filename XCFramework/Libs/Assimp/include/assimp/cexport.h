/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

/** @file  cexport.h
*  @brief Defines the C-API for the Assimp export interface
*/
#ifndef AI_EXPORT_H_INC
#define AI_EXPORT_H_INC

#ifndef ASSIMP_BUILD_NO_EXPORT

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

struct aiScene;  // aiScene.h
struct aiFileIO; // aiFileIO.h

// --------------------------------------------------------------------------------
/** Describes an file format which Assimp can export to. Use #aiGetExportFormatCount() to
* learn how many export formats the current Assimp build supports and #aiGetExportFormatDescription()
* to retrieve a description of an export format option.
*/
struct aiExportFormatDesc
{
    /// a short string ID to uniquely identify the export format. Use this ID string to
    /// specify which file format you want to export to when calling #aiExportScene().
    /// Example: "dae" or "obj"
    const char* id;

    /// A short description of the file format to present to users. Useful if you want
    /// to allow the user to select an export format.
    const char* description;

    /// Recommended file extension for the exported file in lower case.
    const char* fileExtension;
};


// --------------------------------------------------------------------------------
/** Returns the number of export file formats available in the current Assimp build.
 * Use aiGetExportFormatDescription() to retrieve infos of a specific export format.
 */
ASSIMP_API size_t aiGetExportFormatCount(void);


// --------------------------------------------------------------------------------
/** Returns a description of the nth export file format. Use #aiGetExportFormatCount()
 * to learn how many export formats are supported. The description must be released by 
 * calling aiReleaseExportFormatDescription afterwards.
 * @param pIndex Index of the export format to retrieve information for. Valid range is
 *    0 to #aiGetExportFormatCount()
 * @return A description of that specific export format. NULL if pIndex is out of range.
 */
ASSIMP_API const C_STRUCT aiExportFormatDesc* aiGetExportFormatDescription( size_t pIndex);

// --------------------------------------------------------------------------------
/** Release a description of the nth export file format. Must be returned by 
* aiGetExportFormatDescription
* @param desc Pointer to the description
*/
ASSIMP_API void aiReleaseExportFormatDescription( const C_STRUCT aiExportFormatDesc *desc );

// --------------------------------------------------------------------------------
/** Create a modifiable copy of a scene.
 *  This is useful to import files via Assimp, change their topology and
 *  export them again. Since the scene returned by the various importer functions
 *  is const, a modifiable copy is needed.
 *  @param pIn Valid scene to be copied
 *  @param pOut Receives a modifyable copy of the scene. Use aiFreeScene() to
 *    delete it again.
 */
ASSIMP_API void aiCopyScene(const C_STRUCT aiScene* pIn,
    C_STRUCT aiScene** pOut);


// --------------------------------------------------------------------------------
/** Frees a scene copy created using aiCopyScene() */
ASSIMP_API void aiFreeScene(const C_STRUCT aiScene* pIn);

// --------------------------------------------------------------------------------
/** Exports the given scene to a chosen file format and writes the result file(s) to disk.
* @param pScene The scene to export. Stays in possession of the caller, is not changed by the function.
*   The scene is expected to conform to Assimp's Importer output format as specified
*   in the @link data Data Structures Page @endlink. In short, this means the model data
*   should use a right-handed coordinate systems, face winding should be counter-clockwise
*   and the UV coordinate origin is assumed to be in the upper left. If your input data
*   uses different conventions, have a look at the last parameter.
* @param pFormatId ID string to specify to which format you want to export to. Use
* aiGetExportFormatCount() / aiGetExportFormatDescription() to learn which export formats are available.
* @param pFileName Output file to write
* @param pPreprocessing Accepts any choice of the #aiPostProcessSteps enumerated
*   flags, but in reality only a subset of them makes sense here. Specifying
*   'preprocessing' flags is useful if the input scene does not conform to
*   Assimp's default conventions as specified in the @link data Data Structures Page @endlink.
*   In short, this means the geometry data should use a right-handed coordinate systems, face
*   winding should be counter-clockwise and the UV coordinate origin is assumed to be in
*   the upper left. The #aiProcess_MakeLeftHanded, #aiProcess_FlipUVs and
*   #aiProcess_FlipWindingOrder flags are used in the import side to allow users
*   to have those defaults automatically adapted to their conventions. Specifying those flags
*   for exporting has the opposite effect, respectively. Some other of the
*   #aiPostProcessSteps enumerated values may be useful as well, but you'll need
*   to try out what their effect on the exported file is. Many formats impose
*   their own restrictions on the structure of the geometry stored therein,
*   so some preprocessing may have little or no effect at all, or may be
*   redundant as exporters would apply them anyhow. A good example
*   is triangulation - whilst you can enforce it by specifying
*   the #aiProcess_Triangulate flag, most export formats support only
*   triangulate data so they would run the step anyway.
*
*   If assimp detects that the input scene was directly taken from the importer side of
*   the library (i.e. not copied using aiCopyScene and potetially modified afterwards),
*   any postprocessing steps already applied to the scene will not be applied again, unless
*   they show non-idempotent behaviour (#aiProcess_MakeLeftHanded, #aiProcess_FlipUVs and
*   #aiProcess_FlipWindingOrder).
* @return a status code indicating the result of the export
* @note Use aiCopyScene() to get a modifiable copy of a previously
*   imported scene.
*/
ASSIMP_API aiReturn aiExportScene( const C_STRUCT aiScene* pScene,
    const char* pFormatId,
    const char* pFileName,
    unsigned int pPreprocessing);


// --------------------------------------------------------------------------------
/** Exports the given scene to a chosen file format using custom IO logic supplied by you.
* @param pScene The scene to export. Stays in possession of the caller, is not changed by the function.
* @param pFormatId ID string to specify to which format you want to export to. Use
* aiGetExportFormatCount() / aiGetExportFormatDescription() to learn which export formats are available.
* @param pFileName Output file to write
* @param pIO custom IO implementation to be used. Use this if you use your own storage methods.
*   If none is supplied, a default implementation using standard file IO is used. Note that
*   #aiExportSceneToBlob is provided as convenience function to export to memory buffers.
* @param pPreprocessing Please see the documentation for #aiExportScene
* @return a status code indicating the result of the export
* @note Include <aiFileIO.h> for the definition of #aiFileIO.
* @note Use aiCopyScene() to get a modifiable copy of a previously
*   imported scene.
*/
ASSIMP_API aiReturn aiExportSceneEx( const C_STRUCT aiScene* pScene,
    const char* pFormatId,
    const char* pFileName,
    C_STRUCT aiFileIO* pIO,
    unsigned int pPreprocessing );


// --------------------------------------------------------------------------------
/** Describes a blob of exported scene data. Use #aiExportSceneToBlob() to create a blob containing an
* exported scene. The memory referred by this structure is owned by Assimp.
* to free its resources. Don't try to free the memory on your side - it will crash for most build configurations
* due to conflicting heaps.
*
* Blobs can be nested - each blob may reference another blob, which may in turn reference another blob and so on.
* This is used when exporters write more than one output file for a given #aiScene. See the remarks for
* #aiExportDataBlob::name for more information.
*/
struct aiExportDataBlob
{
    /// Size of the data in bytes
    size_t size;

    /// The data.
    void* data;

    /** Name of the blob. An empty string always
        indicates the first (and primary) blob,
        which contains the actual file data.
        Any other blobs are auxiliary files produced
        by exporters (i.e. material files). Existence
        of such files depends on the file format. Most
        formats don't split assets across multiple files.

        If used, blob names usually contain the file
        extension that should be used when writing
        the data to disc.
     */
    C_STRUCT aiString name;

    /** Pointer to the next blob in the chain or NULL if there is none. */
    C_STRUCT aiExportDataBlob * next;

#ifdef __cplusplus
    /// Default constructor
    aiExportDataBlob() { size = 0; data = next = NULL; }
    /// Releases the data
    ~aiExportDataBlob() { delete [] static_cast<unsigned char*>( data ); delete next; }

private:
    // no copying
    aiExportDataBlob(const aiExportDataBlob& );
    aiExportDataBlob& operator= (const aiExportDataBlob& );
#endif // __cplusplus
};

// --------------------------------------------------------------------------------
/** Exports the given scene to a chosen file format. Returns the exported data as a binary blob which
* you can write into a file or something. When you're done with the data, use #aiReleaseExportBlob()
* to free the resources associated with the export.
* @param pScene The scene to export. Stays in possession of the caller, is not changed by the function.
* @param pFormatId ID string to specify to which format you want to export to. Use
* #aiGetExportFormatCount() / #aiGetExportFormatDescription() to learn which export formats are available.
* @param pPreprocessing Please see the documentation for #aiExportScene
* @return the exported data or NULL in case of error
*/
ASSIMP_API const C_STRUCT aiExportDataBlob* aiExportSceneToBlob( const C_STRUCT aiScene* pScene, const char* pFormatId,  unsigned int pPreprocessing );


// --------------------------------------------------------------------------------
/** Releases the memory associated with the given exported data. Use this function to free a data blob
* returned by aiExportScene().
* @param pData the data blob returned by #aiExportSceneToBlob
*/
ASSIMP_API void aiReleaseExportBlob( const C_STRUCT aiExportDataBlob* pData );

#ifdef __cplusplus
}
#endif

#endif // ASSIMP_BUILD_NO_EXPORT
#endif // AI_EXPORT_H_INC

