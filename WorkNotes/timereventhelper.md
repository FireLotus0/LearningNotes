### 定时器事件辅助工具类

* 头文件
```cpp
#pragma once

#include <qobject.h>
#include <qtimer.h>
#include <qhash.h>

class TimerEventHelper : public QObject
{

public:
    static void registerEvent(QObject* object, const QList<std::function<void()>> &func);

    static void registerEvent(QObject* object, const std::function<void()> &func);

    static TimerEventHelper& getInstance();

    static void startListen();

    static void stopListen();

private:
    explicit TimerEventHelper(QObject *parent = nullptr);
    ~TimerEventHelper() = default;

private:
    QHash<QObject*, QList<std::function<void()>> > timerEvents;
    QList<std::function<void()>> globalTimerEvents;
    QTimer* timer;
};

```

* CPP文件
```cpp
#include "timereventhelper.h"

TimerEventHelper::TimerEventHelper(QObject *parent)
    : QObject(parent)
{
    timer = new QTimer(this);
    timer->setInterval(500);
    QObject::connect(timer, &QTimer::timeout, [&]{
        for(const auto& key : timerEvents.keys()) {
            for(const auto& func : timerEvents.value(key)) {
                if(func != nullptr) {
                    func();
                }
            }
        }
    });
}

TimerEventHelper &TimerEventHelper::getInstance() {
    static TimerEventHelper timerEventHelper;
    return timerEventHelper;
}

void TimerEventHelper::registerEvent(QObject* object, const QList<std::function<void()>> &func) {
    connect(object, &QObject::destroyed, [object]{
        getInstance().timerEvents.remove(object);
    });
    auto funcs = getInstance().timerEvents.value(object);
    funcs.append(func);
    getInstance().timerEvents[object] = funcs;
}

void TimerEventHelper::startListen() {
    getInstance().timer->start();
}

void TimerEventHelper::stopListen() {
    getInstance().timer->stop();
}

void TimerEventHelper::registerEvent(QObject *object, const std::function<void()> &func) {
    connect(object, &QObject::destroyed, [object]{
        getInstance().timerEvents.remove(object);
    });
    auto funcs = getInstance().timerEvents.value(object);
    funcs.append(func);
    getInstance().timerEvents[object] = funcs;
}

```


