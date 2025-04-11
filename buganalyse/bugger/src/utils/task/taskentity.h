#pragma once

#include "utils/macros.h"
//#include "utils/session/session.h"
#include <type_traits>
#include <tuple>
#include <string>

class Session;

struct TaskEntityBase {
    explicit TaskEntityBase(TaskType taskType);
    virtual ~TaskEntityBase() = default;

    template<typename T>
    bool isSucceed(T v) {
        if constexpr (std::is_pointer_v<T>) {
            return v != nullptr;
        } else if constexpr (std::is_integral_v<T>) {
            return v != 0;
        } else if constexpr (std::is_same_v<bool, T>) {
            return v;
        } else {
            return true;
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
    ~TaskEntity() override= default;

    explicit TaskEntity(TaskType taskType, Func&& f, Args&&...args) : TaskEntityBase(taskType){
        func = std::move(f);
        params = std::forward_as_tuple(std::forward<Args>(args)...);
    }

    bool execute() override {
        bool succeed = true;
        if constexpr (std::is_invocable_v<Func>) {
            succeed = this->isSucceed(std::apply(func, params));
        }
        std::get<0>(params)->executeCallback(taskType);
        return succeed;
    }

    Session * getSessionPtr() override {
        if constexpr (std::is_invocable_v<Func>) {
            return (Session *)std::get<0>(params);
        } else {
            return nullptr;
        }
    }

private:
    Func func;
    std::tuple<Args...> params;
};







