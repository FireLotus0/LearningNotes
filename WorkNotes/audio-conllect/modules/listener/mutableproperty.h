#pragma once

#include "observerproperty.h"

template<typename T>
class MutableProperty : public ObserverProperty<T> {
public:
    explicit MutableProperty(const T&& value = T())
        : ObserverProperty<T>(QString(), value)
    {
        ObserverProperty<T>::init();
    }

    using ObserverProperty<T>::operator=;

protected:
    void saveData(const QString &, const T &) override {}

    T readData(const QString &, const T &defaultValue) override {
        return defaultValue;
    }
};

class MutableVariantProperty : public MutableProperty<QVariant> {
public:
    using MutableProperty<QVariant>::MutableProperty;

    template<typename T>
    MutableVariantProperty& operator=(const T& value) {
        setAdapterValue<T>(value);
        return *this;
    }

private:
    template<typename T>
    void setAdapterValue(const typename std::enable_if<std::is_constructible<QVariant, T>::value, T>::type& value) {
        MutableProperty<QVariant>::operator=(QVariant(value));
    }

    template<typename T>
    void setAdapterValue(const typename std::enable_if<!std::is_constructible<QVariant, T>::value, T>::type& value) {
        MutableProperty<QVariant>::operator=(QVariant::fromValue(value));
    }
};