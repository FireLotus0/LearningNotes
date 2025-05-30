#pragma once

#include "datanotifier.h"

#include <qobject.h>
#include <functional>
#include <qpointer.h>

/**
 * @tparam T Observer
 * @tparam K Data Type
 */
template <typename T, typename K>
class DataObserver : public DataNotifyInterface<K> {

    QPointer<T> contextPointer;

public:
    explicit DataObserver(T* p, bool notifyOnChanged): contextPointer(p), notifyOnChanged(notifyOnChanged) {}

protected:
    bool canBeReference() override {
        return !contextPointer.isNull();
    }

    bool isNotifyOnChanged() override  {
        return notifyOnChanged;
    }

private:
    bool notifyOnChanged;
};
