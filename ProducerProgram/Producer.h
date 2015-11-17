#ifndef PRODUCER_H
#define PRODUCER_H
#include <mutex>

class Producer {
public:
    static Producer* instance();
    ~Producer();

    Producer(const Producer&) = delete;
    Producer& operator= (const Producer&) = delete;

    void Start(const unsigned&);
    void Stop(const unsigned&);
    void Quit();

private:
    Producer() = default;

    static Producer* producer_;
    static std::once_flag once_flag_;
};


#endif
