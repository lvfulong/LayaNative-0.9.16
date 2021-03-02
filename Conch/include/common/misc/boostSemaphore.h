
#include <condition_variable>
#include <thread> 

namespace laya {
    class semaphore {
        unsigned int count_;
        std::mutex mutex_;
        std::condition_variable condition_;
    public:
        //����һ��semaphore ��ʼֵΪinitial.
        explicit semaphore(unsigned int initial) : count_(initial) {}

        //���Ӽ���������֪ͨ������wait������顣
        void signal() {
            {
                std::unique_lock<std::mutex> lock(mutex_);
                ++count_;
            }
            condition_.notify_one();
        }

        //�ȴ���ֱ������ signal��signal��count>0,wait�ɹ����count
        void wait() {
            std::unique_lock<std::mutex> lock(mutex_);
            while (count_ == 0) {
                condition_.wait(lock);
            }
            --count_;
        }

        //�ȴ������û�������Ӽ����ĵĻ���ʱ�䵽����������ء��ɹ����count
        //�������Ҫ����count
        void wait(int ms) {
            std::unique_lock<std::mutex> lock(mutex_);
            condition_.wait_until(lock, std::chrono::system_clock::now() + std::chrono::milliseconds(ms));
            --count_;
        }
    };
}