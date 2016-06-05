/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "EnginePrecompiledHeader.h"
#include "FileIO.h"

FileIO* FileIO::ms_pFileIO = nullptr;

FileIO::FileIO(void)
{
    Logger("[FILEIO] Initialized");
}


FileIO::~FileIO(void)
{
}

FileIO* FileIO::getInstance()
{
    if(ms_pFileIO == nullptr)
    {
        ms_pFileIO = new FileIO();
    }

    return ms_pFileIO;
}

FILE* FileIO::openFile(std::string path)
{
    FILE* fp;

    fopen_s(&fp, path.c_str(), "r");

    if(fp)
    {
        m_files[path] = fp;
    }

    return fp;
}

void FileIO::closeFile(std::string path)
{
    if(m_files[path])
    {
        fclose(m_files[path]);
        m_files.erase(m_files.find(path));
    }
}

void FileIO::closeFile(FILE* fp)
{
    if(fp)
    {
        std::map<std::string, FILE*>::iterator it;
        for(it = m_files.begin(); it != m_files.end(); it++)
        {
            if(it->second == fp)
            {
                fclose(it->second);
                m_files.erase(it);
            }
        }
    }
}

void FileIO::readAll(std::string path)
{
    //fread(m_buffer, 
}