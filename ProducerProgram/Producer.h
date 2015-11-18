#ifndef PRODUCER_H
#define PRODUCER_H
#include <mutex>
#include <vector>

class Producer {
    friend class RdWrServiceHandler;
public:
    static Producer* instance();
    ~Producer();

    Producer(const Producer&) = delete;
    Producer& operator= (const Producer&) = delete;

    void Start(const unsigned&);
    void Stop(const unsigned&);
    void Quit() const;

private:
    Producer() = default;

    void AddHandler(class RdWrServiceHandler* handler);
    void RemoveHandler(RdWrServiceHandler* handler);
    
    // Used for Singleton Pattern
    static Producer* producer_;
    static std::once_flag once_flag_;

    // Used for caching connection event handler
    // This is a critcal section
    std::vector<RdWrServiceHandler*> handlers_;
};


#endif
