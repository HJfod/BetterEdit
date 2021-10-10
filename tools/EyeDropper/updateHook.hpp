#pragma once

#include <functional>

void scheduleOneTimeFuncInGDThread(std::function<void()>);
