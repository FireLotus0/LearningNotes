#pragma once

#include <qatomic.h>
#include <qmutex.h>

template<typename T, int AllocSize>
class ArrayObjectCache {
public:
    enum {
        TSize = sizeof(T)
    };

    ArrayObjectCache() {
        data = new T[AllocSize];
    }

    ~ArrayObjectCache() {
        delete[] data;
    }

    void append(const T& newData) {
        auto curDataPos = rightDataPtr.loadRelaxed();
        memcpy(data + curDataPos, &newData, TSize);
        curDataPos++;
        if (curDataPos >= AllocSize) {
            curDataPos = 0;
        }

        auto leftDataPos = leftDataPtr.loadRelaxed();
        if (curDataPos == leftDataPos) {
            leftDataPos++;
            if (leftDataPos >= AllocSize) {
                leftDataPos = 0;
            }
            leftDataPtr.storeRelease(leftDataPos);
        }
        rightDataPtr.storeRelease(curDataPos);
    }

    void clear() {
        leftDataPtr.storeRelease(0);
        rightDataPtr.storeRelease(0);
    }

    QVector<T> getCached() {
        auto left = leftDataPtr.loadAcquire();
        auto right = rightDataPtr.loadAcquire();
        if (left == right) {
            return {};
        }
        int size;
        if (right > left) {
            size = right - left;
            QVector<T> cached(size);
            memcpy(cached.data(), data + left, size * TSize);
            leftDataPtr.storeRelaxed(right);
            return cached;
        } else {
            size = AllocSize - left + right;
            QVector<T> cached(size);
            memcpy(cached.data(), data + left, (AllocSize - left) * TSize);
            if (right != 0) {
                memcpy(cached.data() + (AllocSize - left), data, right * TSize);
            }
            leftDataPtr.storeRelaxed(right);
            return cached;
        }
    }

private:
    T* data;
    QAtomicInt leftDataPtr = 0;
    QAtomicInt rightDataPtr = 0;
};