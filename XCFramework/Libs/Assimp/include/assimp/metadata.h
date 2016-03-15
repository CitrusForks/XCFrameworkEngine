/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

/** @file metadata.h
 *  @brief Defines the data structures for holding node meta information.
 */
#ifndef __AI_METADATA_H_INC__
#define __AI_METADATA_H_INC__

#include <assert.h>

#if defined(_MSC_VER) && (_MSC_VER <= 1500)
#include "Compiler/pstdint.h"
#else
#include <limits.h>
#include <stdint.h>
#endif



// -------------------------------------------------------------------------------
/**
  * Enum used to distinguish data types
  */
 // -------------------------------------------------------------------------------
typedef enum aiMetadataType
{
    AI_BOOL = 0,
    AI_INT = 1,
    AI_UINT64 = 2,
    AI_FLOAT = 3,
    AI_AISTRING = 4,
    AI_AIVECTOR3D = 5,

#ifndef SWIG
    FORCE_32BIT = INT_MAX
#endif
} aiMetadataType;



// -------------------------------------------------------------------------------
/**
  * Metadata entry
  *
  * The type field uniquely identifies the underlying type of the data field
  */
 // -------------------------------------------------------------------------------
struct aiMetadataEntry
{
    aiMetadataType mType;
    void* mData;
};



#ifdef __cplusplus

#include <string>



// -------------------------------------------------------------------------------
/**
  * Helper functions to get the aiType enum entry for a type
  */
 // -------------------------------------------------------------------------------
inline aiMetadataType GetAiType( bool ) { return AI_BOOL; }
inline aiMetadataType GetAiType( int ) { return AI_INT; }
inline aiMetadataType GetAiType( uint64_t ) { return AI_UINT64; }
inline aiMetadataType GetAiType( float ) { return AI_FLOAT; }
inline aiMetadataType GetAiType( aiString ) { return AI_AISTRING; }
inline aiMetadataType GetAiType( aiVector3D ) { return AI_AIVECTOR3D; }



#endif



// -------------------------------------------------------------------------------
/**
  * Container for holding metadata.
  *
  * Metadata is a key-value store using string keys and values.
  */
 // -------------------------------------------------------------------------------
struct aiMetadata
{
    /** Length of the mKeys and mValues arrays, respectively */
    unsigned int mNumProperties;

    /** Arrays of keys, may not be NULL. Entries in this array may not be NULL as well. */
    C_STRUCT aiString* mKeys;

    /** Arrays of values, may not be NULL. Entries in this array may be NULL if the
      * corresponding property key has no assigned value. */
    C_STRUCT aiMetadataEntry* mValues;

#ifdef __cplusplus

    /** Constructor */
    aiMetadata()
        // set all members to zero by default
        : mNumProperties(0)
        , mKeys(NULL)
        , mValues(NULL)
    {}


    /** Destructor */
    ~aiMetadata()
    {
        delete[] mKeys;
        mKeys = NULL;
        if (mValues)
        {
            // Delete each metadata entry
            for (unsigned i=0; i<mNumProperties; ++i)
            {
                void* data = mValues[i].mData;
                switch (mValues[i].mType)
                {
                case AI_BOOL:
                    delete static_cast<bool*>(data);
                    break;
                case AI_INT:
                    delete static_cast<int*>(data);
                    break;
                case AI_UINT64:
                    delete static_cast<uint64_t*>(data);
                    break;
                case AI_FLOAT:
                    delete static_cast<float*>(data);
                    break;
                case AI_AISTRING:
                    delete static_cast<aiString*>(data);
                    break;
                case AI_AIVECTOR3D:
                    delete static_cast<aiVector3D*>(data);
                    break;
                default:
                    assert(false);
                    break;
                }
            }

            // Delete the metadata array
            delete [] mValues;
            mValues = NULL;
        }
    }



    template<typename T>
    inline void Set( unsigned index, const std::string& key, const T& value )
    {
        // In range assertion
        assert(index < mNumProperties);

        // Set metadata key
        mKeys[index] = key;

        // Set metadata type
        mValues[index].mType = GetAiType(value);
        // Copy the given value to the dynamic storage
        mValues[index].mData = new T(value);
    }

    template<typename T>
    inline bool Get( unsigned index, T& value )
    {
        // In range assertion
        assert(index < mNumProperties);

        // Return false if the output data type does
        // not match the found value's data type
        if ( GetAiType( value ) != mValues[ index ].mType ) {
            return false;
        }

        // Otherwise, output the found value and
        // return true
        value = *static_cast<T*>(mValues[index].mData);
        return true;
    }

    template<typename T>
    inline bool Get( const aiString& key, T& value )
    {
        // Search for the given key
        for (unsigned i=0; i<mNumProperties; ++i)
            if (mKeys[i]==key)
                return Get(i, value);
        return false;
    }

    template<typename T>
    inline bool Get( const std::string& key, T& value ) {
        return Get(aiString(key), value);
    }

#endif // __cplusplus

};

#endif // __AI_METADATA_H_INC__


