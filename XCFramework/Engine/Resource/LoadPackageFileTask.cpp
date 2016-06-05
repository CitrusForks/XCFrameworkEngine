/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "EnginePrecompiledHeader.h"

#include "ResourceManager.h"
#include "Engine/Utils/FileIO.h"
#include "LoadPackageFileTask.h"

LoadPackageFileTask::LoadPackageFileTask(std::string packageFileName)
    : m_packageFileName(packageFileName)
{
    m_resourceManager = &SystemLocator::GetInstance()->RequestSystem<ResourceManager>("ResourceManager");
}

LoadPackageFileTask::~LoadPackageFileTask()
{
}

void LoadPackageFileTask::Init()
{
    AsyncTask::Init();
}

void LoadPackageFileTask::Run()
{
    AsyncTask::Run();

    //Read the file and create resources and load contents based on it
    //TODO : Creating FILE API and then reading it from there
    FILE* fp = FILEIO_MANAGER->openFile(m_packageFileName);
    
    if (fp)
    {
        while (true)
        {
            //Read the resources here
            char resourceType[256];
            
            if (fscanf_s(fp, "%s", resourceType) == EOF)
            {
                //Reached EOF, don't read anymore
                break;
            }
            
            if (resourceType)
            {
                //Insert this into our ResourcePool and read the resource path
                char userFriendlyName[256];
                fscanf_s(fp, "%s", userFriendlyName);
                
                std::string usrName(userFriendlyName);
                
                IResource* resource = m_resourceManager->GetResourceFactory().createResource(resourceType, userFriendlyName);
                
                if (resource)
                {
                    //TODO: May be now it's not required to not load -> take it from data, if wanted to load at some other time
                    //Anyway load the resource
                    m_resourceManager->GetResourceFactory().loadResource(fp, resource);
                    
                    ResourceHandle resHandle = {};
                    resHandle.m_Resource = resource;
                    m_resourceManager->AddResource(resHandle);
                }
            }
        };
    }

    Destroy();
}

void LoadPackageFileTask::Destroy()
{
    FILEIO_MANAGER->closeFile(m_packageFileName);
    AsyncTask::Destroy();
}