/**
 * @brief 线程管理类
 *
 * 通过ThreadManage创建线程，使得应用程序退出时将通知所有管理的线程退出，并在所有线程全部退出时
 * 再结束应用程序。受管理的线程分为两种：持续计算型线程、基于事件循环线程。两种线程有不同的使用
 * 方法，如下举例说明：
 *
 * 1. 持续计算型线程
 * 该类线程需要继承QThread，并且重写run函数用于执行线程操作：
 *
 * @code
 * class MyThread : public QThread {
 * public:
 *    MyThread(int data1, int data2);
 *
 * protected:
 *    void run() override {
 *        while(!isInterruptionRequested()) {
 *            //do something...
 *        }
 *    }
 * }
 *
 * //create and run
 * auto myThread = ThreadManage::create<MyThread>(1, 2);
 * myThread->start();
 *
 * //close
 * myThread->requestInterruption();
 *
 * @endcode
 *
 * 上述例子中，注意使用isInterruptionRequested()判断当前线程是否需要退出计算，其他线程请求该线程
 * 退出时调用requestInterruption()，如果线程的生命周期跟App一致，ThreadManage会自动在App退出时
 * 用requestInterruption()
 *
 * 2. 基于事件循环线程
 * 该类型线程需要继承EventDrivenThread：
 *
 * @code
 * class MyThread : public EventDrivenThread {
 *     Q_OBJECT
 *
 * public:
 *     MyThread(int data1, int data2) {
 *          connect(this, &MyThread::doSomethingInWorkThread, this, &MyThread::doSomething);
 *     }
 *
 * signals:
 *     void doSomethingInWorkThread();
 *
 * private slots:
 *     void doSomething() {
 *        while(!thr->isInterruptionRequested()) {
 *            //do something...
 *        }
 *     }
 * }
 *
 * //create and run
 * auto myThread = ThreadManage::create<MyThread>(1, 2);
 * myThread->doSomethingInWorkThread();
 *
 * //close
 * myThread->quit();
 *
 * @endcode
 *
 * 上述例子中，注意如果线程执行函数doSomething中存在计算性任务，同样需要使用thr->isInterruptionRequested()
 * 判断当前线程是否应该退出，退出时调用quit()，如果线程的生命周期跟App一致，ThreadManage会自动在App退出时
 * 用quit()
 *
 * @warning 不可在工作线程中使用create函数创建其他子线程，该操作未测试
 */
#pragma once

#include <qobject.h>
#include <qthread.h>
#include <qmutex.h>
#include <qwaitcondition.h>

class EventDrivenThread : public QObject {
    Q_OBJECT

public:
    QThread* thr = nullptr;
public slots:
    void quit() {
        thr->requestInterruption();
        aboutToQuit(QPrivateSignal());
    }

signals:
    void aboutToQuit(QPrivateSignal);
};

class ThreadManage : public QObject {
public:
    /**
     * 创建线程实例
     * @tparam T 线程类型T
     * @tparam Args 构造函数参数
     * @param args
     * @return
     */
    template<typename T, typename... Args>
    static T* create(Args... args) {
        auto t = new T(args...);
        instance().initThread(t);
        return t;
    }

private:
    ThreadManage();

    /**
     * 单例，返回ThreadManage实例
     * @return
     */
    static ThreadManage& instance();

    /**
     * 初始化计算型线程
     * @param thread
     */
    void initThread(QThread* thread);

    /**
     * 初始化基于事件循环的线程，对象会移动到工作线程中
     * @param thread
     */
    void initThread(EventDrivenThread* thread);

    /**
     * 应用退出处理
     */
    void appQuitHandler();

    /**
     * 记录线程对象
     * @param thread
     */
    void recordThr(QObject* thread);

    /**
     * 移除线程对象
     * @param thread
     */
    void removeThr(QObject* thread);

    /**
     * 通知所有线程即将退出应用程序
     */
    void notifyAllQuit();

private:
    QList<QObject*> threadInstances;
    QMutex mutex;
    QWaitCondition releaseWait;
};
