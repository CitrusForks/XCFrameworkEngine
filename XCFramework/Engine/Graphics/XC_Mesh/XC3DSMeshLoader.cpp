/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"
#include "XC3DSMeshLoader.h"

#if defined(WIN32)
#include <io.h>
#elif defined(XC_ORBIS)

unsigned int _filelength(FILE* fp)
{
    unsigned originalPosition = ftell(fp);
    fseek(fp, 0, SEEK_END);
    unsigned int endPosition = ftell(fp);

    fseek(fp, originalPosition, SEEK_SET);
    return endPosition;
}
#endif

XC3DSMeshLoader::XC3DSMeshLoader(void)
{
}


XC3DSMeshLoader::~XC3DSMeshLoader(void)
{
}


bool XC3DSMeshLoader::loadMeshFromFile(std::string fileName, XCMesh* const outMesh)
{
    //Open the file
    if(!fileName.c_str())
    {
        Logger("[XC3DSMeshLoader] Invalid file name");
        return false;
    }

    //Open the file
    FILE* fp = fopen(fileName.c_str(), "rb");

#if defined(WRITE_DATA_TO_FILE)
    FILE* outFile = fopen(getPlatformPath("MeshLoader.txt").c_str(), "wt");
#endif

    if(!fp)
    {
        Logger ("[XC3DSMeshLoader] Failed to open the file");
        return false;
    }

    Chunk tempChunk = {0};
    int retValue = 0;

    MeshData* subMesh;

#if defined(XC_ORBIS)
    while(ftell(fp) < _filelength(fp))   //Till the file pointer reaches the EOF
#elif defined(WIN32)
    while(ftell(fp) < _filelength(_fileno(fp)))   
#endif
    {
        fread(&tempChunk.chunkId, 2, 1, fp);
        fread(&tempChunk.chunkLength, 4, 1, fp);

        switch(tempChunk.chunkId)
        {
        case CHUNK_MAIN:
            //MAIN CHUNK
            break;

        case CHUNK_3DEDITOR:
            //3D EDITOR CHUNK
            break;

        case CHUNK_OBJECT:
            {
                subMesh = outMesh->CreateAndGetSubMesh();

                //OBJECT CHUNK
                int i = 0;
                char ch;
                char* pObjectName = new char[20];

                do 
                {
                    fread(&ch, 1, 1, fp);
                    pObjectName[i] = ch;
                    i++;
                } while (ch != '\0' && i < 20);

                pObjectName[i] = '\0';
                subMesh->SetObjectName(pObjectName);

                //cout<<"Object Name : "<< obj.objectName;
#if defined(WRITE_DATA_TO_FILE)
                fprintf(outFile, "Object Name : %s\n",  pObjectName);
                fprintf(outFile, "\n\n\n---------------------------------\n\n\n");
#endif
                break;
            }

        case CHUNK_TRIANGULARMESHLIST:
            break;

        case CHUNK_VERTICESLIST:
            {
                fread(&tempChunk.amtOfData, sizeof(unsigned short), 1, fp);
                subMesh->SetNoOfVertices(tempChunk.amtOfData);

                //cout<<"No of vertices : "<< obj.noOfVertices;
#if defined(WRITE_DATA_TO_FILE)
                    fprintf(outFile, "No of vertices : %d\n", tempChunk.amtOfData);
#endif
                    float x, y, z;
    
                    for(unsigned int i = 0; i < tempChunk.amtOfData; i++)
                    {
                        fread(&x, sizeof(float), 1, fp);
                        fread(&y, sizeof(float), 1, fp);
                        fread(&z, sizeof(float), 1, fp);
    
                        //printf("Vertex [%d] : %f %f %f\n", obj.vertex[i].x, obj.vertex[i].y, obj.vertex[i].z);
                        subMesh->AddVertex(x, y, z);
#if defined(WRITE_DATA_TO_FILE)
                    fprintf(outFile, "Vertex [%d] : %f  %f  %f\n", i, x, y, z);
#endif
                }
#if defined(WRITE_DATA_TO_FILE)
                fprintf(outFile, "\n\n\n---------------------------------\n\n\\n");
#endif
                break;
            }

        case CHUNK_FACESLIST:
            {

                fread(&tempChunk.amtOfData, sizeof(unsigned short), 1, fp);
                subMesh->SetNoOfFaces(tempChunk.amtOfData);

                //cout<<"No of Faces : "<< obj.noOfFaces;
#if defined(WRITE_DATA_TO_FILE)
                fprintf(outFile, "No of Faces : %d\n", tempChunk.amtOfData);
#endif
                unsigned short a, b, c, flags;

                for(unsigned int i = 0; i < tempChunk.amtOfData; i++)
                {
                    fread(&a, sizeof(unsigned short), 1, fp);
                    fread(&b, sizeof(unsigned short), 1, fp);
                    fread(&c, sizeof(unsigned short), 1, fp);
                    fread(&flags, sizeof(unsigned short), 1, fp);
                    subMesh->AddFace(a, b, c, flags);

                    //printf("Faces [%d] : %d %d %d\n", obj.face[i].a, obj.face[i].b, obj.face[i].c);
#if defined(WRITE_DATA_TO_FILE)
                    fprintf(outFile, "Faces [%d] : %d  %d  %d\n", i, a, b, c);
#endif
                }

#if defined(WRITE_DATA_TO_FILE)
                fprintf(outFile, "\n\n\n---------------------------------\n\n\n");
#endif
                break;
            }

        case CHUNK_MAPCOORDLIST:
            {

                fread(&tempChunk.amtOfData, sizeof(unsigned short), 1, fp);

                float u, v;
                for(unsigned int i = 0; i <tempChunk.amtOfData; i++)
                {
                    fread(&u, sizeof(float), 1, fp);
                    fread(&v, sizeof(float), 1, fp);
                    
                    subMesh->AddMapCoord(u, v);
                    //printf("Map Coord [%d] : %f %f\n", obj.mapCoord[i].u, obj.mapCoord[i].v);
#if defined(WRITE_DATA_TO_FILE)
                    fprintf(outFile, "Map Coord [%d] : %f  %f \n", i, u, v);
#endif
                }

                break;
            }
        default:
            retValue = fseek(fp, tempChunk.chunkLength - 6, SEEK_CUR);
        }
    }

    fclose(fp);

#if defined(WRITE_DATA_TO_FILE)
    fclose(outFile);
#endif

    return true;
}