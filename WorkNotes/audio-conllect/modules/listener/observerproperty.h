#pragma once

#include <qobject.h>
#include <qvariant.h>
#include <functional>

#include "dataobserver.h"

template<typename T>
class ObserverProperty : public DataNotifier<T> {
public:
    explicit ObserverProperty(const QString&& propertyKey, T defaultValue = T())
        : propertyKey(propertyKey)
        , defaultValue(defaultValue)
    {}

    T& operator()() {
        return holdValue;
    }

    const T& operator()() const {
        return holdValue;
    }

    ObserverProperty& operator=(const T& value) {
        DataNotifier<T>::dataSetNotify(value);
        return *this;
    }

    const T& last() const {
        return lastValue;
    }

    void save() {
        saveData(propertyKey, holdValue);
    }

    void init() {
        holdValue = readData(propertyKey, defaultValue);
        lastValue = holdValue;
    }

    void resetDefault() {
        DataNotifier<T>::dataSetNotify(defaultValue);
    }

    void notifyValueChanged() {
        DataNotifier<T>::dataSetNotify(holdValue);
    }

    template <typename L>
    void observe(L* context,
                 const std::function<void(const T& value)>& changeNotify,
                 bool notifyOnChanged = true)
    {
        auto observer = new DataObserver<L, T>(context, notifyOnChanged);
        observer->caller = changeNotify;
        DataNotifier<T>::registerNotifier(observer);
    }

protected:
    bool checkValueChanged(const T& other) override {
        return holdValue != other;
    }

    void doValueUpdate(const T& newValue) override {
        lastValue = holdValue;
        holdValue = newValue;
        save();
    }

    virtual void saveData(const QString& key, const T& value) = 0;
    virtual T readData(const QString& key, const T& defaultValue) = 0;

private:
    QString propertyKey;
    T defaultValue;
    T holdValue;
    T lastValue;
};