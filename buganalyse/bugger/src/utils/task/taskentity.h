#pragma once

#include <type_traits>
#include <tuple>
#include <string>
#include "macros.h"

class Session;

struct TaskEntityBase {
    explicit TaskEntityBase(TaskType taskType);

    template<typename T>
    bool isSucceed(T v) {
        if constexpr (std::is_pointer_v<T>) {
            return v != nullptr;
        } else if constexpr (std::is_integral_v<T>) {
            return v != 0;
        } else if constexpr (std::is_same_v<bool, T>) {
            return v;
        }
    }

    virtual bool execute() {
        return true;
    }

    std::string taskResultStr(bool result) const;

    std::string getTaskName() const;

    virtual Session* getSessionPtr() = 0;

    TaskType taskType;
};

template<typename Func, typename...Args>
struct TaskEntity : public TaskEntityBase {
    TaskEntity(const volatile TaskEntity<Func, Args...>&) = delete;

    explicit TaskEntity(TaskType taskType, Func&& f, Args&&...args) : TaskEntityBase(taskType){
        func = f;
        params = std::forward_as_tuple(std::forward<Args>(args)...);
        session = std::get<0>(params);
    }

    bool execute() override{
        return this->isSucceed(std::apply(func, params));
    }

    Session * getSessionPtr() override {
        return session;
    }

private:
    Func func;
    std::tuple<Args...> params;
    Session* session;
};






