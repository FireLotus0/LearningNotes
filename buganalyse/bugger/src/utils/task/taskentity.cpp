#include "taskentity.h"
#include "utils/utils.h"

TaskEntityBase::TaskEntityBase(TaskType taskType)
    : taskType(taskType)
{
}

std::string TaskEntityBase::taskResultStr(bool result) const {
    return TASK_NAME[taskType] + (result ? " Finish! " : " Failed! ");
}

std::string TaskEntityBase::getTaskName() const {
    return  TASK_NAME[taskType];
}
