#pragma once

#include "SquidTasks/Task.h"

namespace OpenedProjectUI {
void ProcessMenuBar();
void ProcessModalPopups();
void ProcessWindows();

Task<> ManageOpenedProject();
}
