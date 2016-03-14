/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include <string>
#include <map>
//TODO: Lot of things

class FileIO
{
public:
    static FileIO*                      getInstance();
    ~FileIO(void);

    FILE*                               openFile(std::string path);
    void                                closeFile(std::string path);
    void                                closeFile(FILE* fp);

    void                                readAll(std::string path);
    void                                readLine();

    FILE*                               getFile(std::string path);
private:
    FileIO(void);

    static FileIO*                      ms_pFileIO;
    char*                               m_buffer;
    std::map<std::string, FILE*>        m_files;
};

#define FILEIO_MANAGER                  FileIO::getInstance()