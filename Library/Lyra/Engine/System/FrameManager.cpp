#include <Lyra/Engine/System/FrameManager.h>

using namespace lyra;

FrameManager::FrameManager(uint frames_in_flight)
{
    frames.resize(frames_in_flight);
}

void FrameManager::bind(Application& app)
{
    // bind frame manager events
    app.bind<AppEvent::UPDATE>(&FrameManager::update, this);
}

void FrameManager::update(Blackboard& blackboard)
{
}
