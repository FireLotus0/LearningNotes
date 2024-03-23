同步队列中m_needStop的作用：为true时，条件变量会通过，也就说调用push和take的同步服务层的线程不会阻塞在这里，才能让这些线程能够退出。

```cpp
template<typename T>
class SyncQueue
{
private:
    std::mutex mt;
    std::condition_variable m_notFull;
    std::condition_variable m_notEmpty;
    std::list<T> m_queue; 
    std::size_t maxSize;
    bool m_needStop;

private:
    bool notFull() const {
        return m_queue.size() < maxSize;
    }

    bool notEmpty() const {
        return m_queue.size() != 0;
    }

    template<typename T>
    void add(T&& t) {
        std::unique_lock<std::mutex> lk(mt);
        m_notFull.wait(lk, [this]{ return m_needStop || notFull(); });
        if(m_needStop) {
            return;
        }
        m_queue.push_back(std::forward<T>(t));
        m_notEmpty.notify_one();
    }

public:
    SyncQueue(std::size_t maxSize)
    : maxSize(maxSize) 
    , m_needStop(false)
    {}

    void push(const T& t) {
        add(x);
    }

    void push(T&& t) {
        add(x);
    }
    
    void take(T& t) {
        std::unique_lock<std::mutex> lk(mt);
        m_notEmpty.wait(lk, [&]{ return m_needStop || notEmpty(); });
        if(m_needStop) {
            return;
        }
        t= m_queue.front();
        m_queue.pop_front();
        m_notFull.notify_one();
    }

    void take(std::list<T>& list) } {
        std::unique_lock<std::mutex> lk(mt);
        m_notEmpty.wait(lk, [this] { return m_needStop || notEmpty(); });
        if(m_needStop) {
            return;
        }
        list = std::move(m_queue);
        m_notFull.notify_one();
    }

    void stop() {
        {
            std::lock_guard<std::mutex> lk(mt);
            m_needStop = true;
        }
        m_notFull.notify_all();
        m_notEmpty.notify_all();
    }

    bool full() {
        std::lock_guard<std::mutex> lk(mt);
        return m_queue.size == m_maxSize;
    }

    bool empty() {
        std::lock_guard<std::mutex> lk(mt);
        return m_queue.empty();
    }

    bool size() {
        std::lock_guard<std::mutex> lk(mt);
        return m_queue.size();
    }
};

constexpr int MaxTaskNum = 100;
class ThreadPool
{
public:
    using Task = std::function<void()>;

    ThreadPool(int numThreads = std::thread::hardware_conccurency)
    : m_queue(MaxTaskNum)
    {
        start(numThreads);
    }

    ~ThreadPool()
    {
        stop();
    }

    void stop()
    {
        std::call_once(m_flag, [this]{ stopThreadGroup(); });
    }

    void addTask(Task&& task)
    {
    m_queue}.push(std::forward<Task>(task));
    }

    void addTask(const Task& task)
    {
        m_queue.push(task);
    }


private:
    void start(int threadsNum) {
        m_running = true;
        //创建线程
        for(int i = 0; i < threadNums; i++) {
            m_threadGroup.push_back(std::make_shared<std::thread>(&ThreadPool::runInThread, this));
        }
    }

    void runInThread()
    {
        while(m_running) 
        {
            //取出任务执行
            std::list<Task> list;
            m_queue.take(list);
            for(auto& task : list) {
                if(!m_running) {
                    return;
                }
                task();
            }
        }
    }

    void stopThreadGroup()
    {
        //让同步队列中的线程停止
        m_queue.stop();
        //让内部线程退出
        m_running = false;
        for(auto& thread : m_threadGroup)
        {
            if(thread) 
            {
                thread->join();
            }
        }
        m_threadGroup.clear();
    }


private:
    //线程组
    std::list<std::shared_ptr<std::thread>> m_threadGroup; 
    //同步队列
    SyncQueue<Task> m_queue;
    //停止标记 
    atomic_bool m_running;  
    std::once_flag m_flag;
};
```

