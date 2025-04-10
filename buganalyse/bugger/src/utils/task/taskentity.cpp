#include "taskentity.h"
#include "utils.h"

TaskEntityBase::TaskEntityBase(TaskType taskType)
    : taskType(taskType)
{
}

std::string TaskEntityBase::taskResultStr(bool result) const {
    return  Utils::curTime() + ": "  + TASK_NAME[taskType] + (result ? " Finish! " : " Failed! ");
}

std::string TaskEntityBase::getTaskName() const {
    return  TASK_NAME[taskType];
}
