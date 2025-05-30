#include "threadmanage.h"

#include <qcoreapplication.h>
#include <qloggingcategory.h>

Q_LOGGING_CATEGORY(threadManage, "threadManage")

ThreadManage::ThreadManage() {
    connect(qApp, &QCoreApplication::aboutToQuit, this, &ThreadManage::appQuitHandler);
}

ThreadManage &ThreadManage::instance() {
    static ThreadManage threadManage;
    return threadManage;
}

void ThreadManage::initThread(QThread *thread) {
    connect(thread, &QThread::finished, [&, thread] {
        removeThr(thread);
        thread->deleteLater();
    });

    recordThr(thread);
}

void ThreadManage::initThread(EventDrivenThread *thread) {
    thread->thr = new QThread;
    thread->moveToThread(thread->thr);

    connect(thread->thr, &QThread::finished, [&, thread] {
        removeThr(thread);
        thread->deleteLater();
        thread->thr->deleteLater();
    });

    connect(thread, &EventDrivenThread::aboutToQuit, thread, [=] {
        thread->thr->quit();
    });

    thread->thr->start();

    recordThr(thread);
}

void ThreadManage::appQuitHandler() {
    notifyAllQuit();
    forever {
        mutex.lock();
        if (threadInstances.isEmpty()) {
            mutex.unlock();
            break;
        }
        releaseWait.wait(&mutex);
        mutex.unlock();
    }
    qCInfo(threadManage) << "All thread released!";
}

void ThreadManage::recordThr(QObject *thread) {
    QMutexLocker locker(&mutex);
    threadInstances << thread;
}

void ThreadManage::removeThr(QObject *thread) {
    QMutexLocker locker(&mutex);
    threadInstances.removeOne(thread);
    releaseWait.notify_all();
}

void ThreadManage::notifyAllQuit() {
    QMutexLocker locker(&mutex);
    for (auto& thr : threadInstances) {
        if (auto thread = dynamic_cast<QThread*>(thr)) {
            thread->requestInterruption();
        }
        if (auto eThread = dynamic_cast<EventDrivenThread*>(thr)) {
            eThread->quit();
        }
    }
    if (threadInstances.isEmpty()) {
        qCInfo(threadManage) << "There is no running thread instance!";
    }
}
