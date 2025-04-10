#pragma once

#include <type_traits>
#include <tuple>
#include <string>
#include "macros.h"

struct TaskEntityBase {
    explicit TaskEntityBase(TaskType taskType);

    template<typename T>
    bool isSucceed(T v) {
        if constexpr (std::is_pointer_v<T>) {
            return v != nullptr;
        } else if constexpr (std::is_integral_v<T>) {
            return v != 0;
        }
    }

    virtual bool execute() {
        return true;
    }

    std::string taskResultStr(bool result) const;

    TaskType taskType;
};

template<typename Func, typename...Args>
struct TaskEntity : public TaskEntityBase {
    explicit TaskEntity(TaskType taskType, Func f, Args&&...args) : TaskEntityBase(taskType){
        func = f;
        params = std::forward_as_tuple(std::forward<Args>(args)...);
    }

    bool execute() override{
        return this->isSucceed(std::apply(func, params));
    }

private:
    Func func;
    std::tuple<Args...> params;
};






