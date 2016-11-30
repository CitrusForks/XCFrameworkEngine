/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "EnginePrecompiledHeader.h"

#include "IActor.h"
#include "Assets/Packages/PackageConsts.h"

IActor::IActor(void)
    : m_userFriendlyName("IActor")
    , m_actorLoaded(false)
    , m_invalidated(false)
    , m_actorState(ActorState_Initialize)
    , m_metaData(nullptr)
{
}

IActor::~IActor(void)
{
    Logger("[IActor] Destructor called for %s", m_userFriendlyName.c_str());
}

IActor::IActor::ActorReturnState IActor::Init()
{
    return IActor::ActorReturnState_Success;
}

IActor::IActor::ActorReturnState IActor::LoadMetaData(const void* metaData)
{
    if (metaData)
    {
        const FBIActor* iactorBuff = (FBIActor*)metaData;

        m_currentPosition.SetValues(iactorBuff->Position()->x(), iactorBuff->Position()->y(), iactorBuff->Position()->z(), iactorBuff->Position()->w());
        m_initialRotation.SetValues(iactorBuff->Rotation()->x(), iactorBuff->Rotation()->y(), iactorBuff->Rotation()->z(), iactorBuff->Rotation()->w());
        m_initialScaling.SetValues(iactorBuff->Scaling()->x(), iactorBuff->Scaling()->y(), iactorBuff->Scaling()->z(), iactorBuff->Scaling()->w());

        m_material.Ambient = XCVec4Unaligned(iactorBuff->Material()->Ambient()->x(), iactorBuff->Material()->Ambient()->y(), iactorBuff->Material()->Ambient()->z(), iactorBuff->Material()->Ambient()->w());
        m_material.Diffuse = XCVec4Unaligned(iactorBuff->Material()->Diffuse()->x(), iactorBuff->Material()->Diffuse()->y(), iactorBuff->Material()->Diffuse()->z(), iactorBuff->Material()->Diffuse()->w());
        m_material.Specular = XCVec4Unaligned(iactorBuff->Material()->Specular()->x(), iactorBuff->Material()->Specular()->y(), iactorBuff->Material()->Specular()->z(), iactorBuff->Material()->Specular()->w());
    }

    return IActor::ActorReturnState_Success;
}

IActor::IActor::ActorReturnState IActor::Load()
{
    IActor::ActorReturnState result = IActor::ActorReturnState_Success;

    for (auto& actor : m_childNodes)
    {
        if (result == IActor::ActorReturnState_Success)
            result = actor->Load();
        else
            break;
    }

    m_actorLoaded = true;

    return result;
}

IActor::IActor::ActorReturnState IActor::OnLoaded()
{
    IActor::ActorReturnState result = IActor::ActorReturnState_Success;

    m_invalidated = false;
    m_isRenderable = true;

    for (auto& actor : m_childNodes)
    {
        if (result == IActor::ActorReturnState_Success)
            result = actor->OnLoaded();
        else
            break;
    }

    return result;
}

IActor::IActor::ActorReturnState IActor::OnUnloaded()
{
    IActor::ActorReturnState result = IActor::ActorReturnState_Success;

    for (auto& actor : m_childNodes)
    {
        if (result == IActor::ActorReturnState_Success)
            result = actor->OnUnloaded();
        else
            break;
    }

    return result;
}

IActor::IActor::ActorReturnState IActor::Update(f32 dt)
{
    IActor::ActorReturnState result = IActor::ActorReturnState_Success;

    for (auto& actor : m_childNodes)
    {
        if (result == IActor::ActorReturnState_Success)
            result = actor->Update(dt);
        else
            break;
    }

    return result;
}

bool IActor::Draw(RenderContext& renderContext)
{
    bool result = false;

    for (auto& actor : m_childNodes)
    {
        if (result)
            result = actor->Draw(renderContext);
        else
            break;
    }

    return result;
}

IActor::IActor::ActorReturnState IActor::Unload()
{
    IActor::ActorReturnState result = IActor::ActorReturnState_Success;

    for (auto& actor : m_childNodes)
    {
        if (result == IActor::ActorReturnState_Success)
            result = actor->Unload();
        else
            break;
    }

    return result;
}

IActor::IActor::ActorReturnState IActor::Destroy()
{
    IActor::ActorReturnState result = IActor::ActorReturnState_Success;

    for (auto& actor : m_childNodes)
    {
        if (result == IActor::ActorReturnState_Success)
            result = actor->Destroy();
        else
            break;
    }

    return result;
}

void IActor::Invalidate()
{
    m_invalidated   = true;
    m_actorLoaded    = false;
    m_isRenderable  = false;

    for (auto& actor : m_childNodes)
    {
        actor->Invalidate();
    }
}