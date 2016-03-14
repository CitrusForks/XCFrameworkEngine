#include "stdafx.h"

#include "WorldEditor.h"


WorldEditor::~WorldEditor(void)
{
}

void WorldEditor::Init(TaskManager& taskMgr)
{
    World::Init(taskMgr);
}

void WorldEditor::Update(float dt)
{
    if (m_canUpdate)
    {
        World::Update(dt);
    }
}

void WorldEditor::Draw(XC_Graphics& graphicsSystem)
{
    if (m_canUpdate)
    {
        World::Draw(graphicsSystem);
    }
}

void WorldEditor::Destroy()
{
    World::Destroy();
}
