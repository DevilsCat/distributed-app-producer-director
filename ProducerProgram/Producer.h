#ifndef PRODUCER_H
#define PRODUCER_H
#include <mutex>
#include <vector>
#include <ace/Asynch_Acceptor.h>
#include "Views.h"
#include "RdWrServiceHandler.h"

class Producer : public ACE_Asynch_Acceptor<RdWrServiceHandler>{
    friend class RdWrServiceHandler;
public:
    static Producer* instance();
    ~Producer();

    Producer(const Producer&) = delete;
    Producer& operator= (const Producer&) = delete;

    void Start(const unsigned&);
    void Stop(const unsigned&);
    void Quit() const;

protected:
    virtual RdWrServiceHandler* make_handler() override;

private:
	Producer();

    void AddHandler(RdWrServiceHandler* handler);
    void RemoveHandler(RdWrServiceHandler* handler);
	size_t GetHandlerIndex(RdWrServiceHandler* handler) const;

    // Used for Singleton Pattern
    static Producer* producer_;
    static std::once_flag once_flag_;

    // Used for caching connection event handler
    std::vector<RdWrServiceHandler*> handlers_;

	TableView<PlayTableViewCell>* table_view_;
};


#endif
