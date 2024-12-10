### memory model
- C++11 引入了一个新的内存模型，它为多线程程序的开发提供了更好的支持。这个内存模型的主要目的是为了解决在多个线程中共享数据时的同步问题，并确保在不同平台上具有一致的行为
 1. 原子操作（Atomic Operations）:C++11 提供了一组原子类型（原子变量），通过 <atomic> 头文件实现。这些原子类型可以在多线程环境中安全地进行操作，而不会引发数据竞争。原子操作保证在多个线程中对同一变量的读写不会被中断
 2. 内存序（Memory Order）:内存模型定义了内存操作的顺序。C++11 提供了几种内存序，用于控制原子操作的可见性和顺序性。主要有以下几种类型
 	- memory_order_relaxed：不保证任何顺序关系，仅保证原子性。
		- 使用场景：不依赖于其他内存操作的顺序，适合于简单的计数和性能敏感的应用。
	- memory_order_acquire：确保在该操作之前的所有读操作在其之后可见。
		- 使用场景：在一个线程中获取数据后，确保其他线程看到这些数据。
	- memory_order_release：确保在该操作之前的所有写操作在其之前可见。
		- 使用场景：在一个线程中更新数据后，发布这些数据供其他线程使用。
	- memory_order_acq_rel：同时保证 acquire 和 release 的语义。
		- 在锁或其他同步原语中，同时保证数据的读取和写入。
	- memory_order_seq_cst：提供最强的顺序保证，所有使用 seq_cst 的操作在逻辑上是顺序一致的。
		- 当你需要强一致性的情况下，适合大多数情况下使用。
 3. 数据竞争（Data Races）:数据竞争是指两个或多个线程同时访问同一变量，并且至少有一个线程在进行写操作，且没有适当的同步机制来保证对该变量的访问顺序。C++11 的内存模型明确规定，数据竞争会导致未定义行为，因此要确保在多线程程序中使用合适的同步机制。
 4. 同步原语:
 	- 互斥量（std::mutex）：用于保护共享数据的互斥锁，确保同一时间内只有一个线程可以访问共享资源。
	- 条件变量（std::condition_variable）：用于线程间的通信，允许线程通过条件变量等待某个条件的发生。
	- 读写锁（std::shared_mutex）：允许多个线程同时读，只有一个线程可以写。

- atomic和volatile:
	-  volatile: is for special objects, on which optimised read or write operations are not allowed
	-  std::atomic: defines atomic variables, which are meant for a thread-safe reading and writing
