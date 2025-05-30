#pragma once

#include <qobject.h>
#include <functional>

template <typename T>
class DataNotifyInterface {
public:
    std::function<void(const T&)> caller;

    virtual bool canBeReference() = 0;

    virtual bool isNotifyOnChanged() = 0;

    virtual ~DataNotifyInterface() = default;
};

template <typename T> class DataNotifier;

template <typename T>
class DataNotifier {
public:
    void registerNotifier(DataNotifyInterface<T>* dataNotifier) {
        notifier.append(dataNotifier);
    }

protected:
    virtual bool checkValueChanged(const T& other) = 0;

    virtual void doValueUpdate(const T& newValue) = 0;

    void dataSetNotify(const T& value) {
        bool valueChanged = checkValueChanged(value);
        if (valueChanged) {
            doValueUpdate(value);
        }
        for (int i = 0; i < notifier.size(); ) {
            if (notifier[i]->canBeReference()) {
                if ((notifier[i]->isNotifyOnChanged() && valueChanged) || !notifier[i]->isNotifyOnChanged()) {
                    notifier[i]->caller(value);
                }
                i++;
                continue;
            }
            delete notifier.takeAt(i);
        }
    }

private:
    QList<DataNotifyInterface<T>*> notifier;
};

