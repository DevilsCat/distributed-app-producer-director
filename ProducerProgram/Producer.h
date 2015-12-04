//
//Defines a singleton producer class for this lab.
//
#ifndef PRODUCER_H
#define PRODUCER_H
#include <mutex>
#include <vector>
#include <ace/Asynch_Acceptor.h>
#include "Views.h"
#include "RdWrServiceHandler.h"
#include "TableViewCells.h"

class Producer : public ACE_Asynch_Acceptor<RdWrServiceHandler>{
    friend class RdWrServiceHandler;
public:
	//A unique instance used for singleton pattern.
    static Producer* instance();
    ~Producer();
	//
	//Start()
	//send a start instruction to director.
	//
    void Start(const unsigned&);
	//
	//Stop()
	//send a stop instruction to director.
	//
    void Stop(const unsigned&);
	//
	//Quit()
	//send a quit instruction to director.
	//
    void Quit() const;

protected:
	//
	//make_handler()
	//allocate a new RdWrServiceHandler on heap.
	//
    virtual RdWrServiceHandler* make_handler() override;

private:
	//
	//Define copy constructor and copy-assignment operator as private to make sure
	//unique copy of producer class.
	//
	Producer();
    Producer(const Producer&);
    Producer& operator= (const Producer&);
	//
	//AddHandler()
	//add a handler to handler buffer of producer.
	//
    void AddHandler(RdWrServiceHandler* handler);
	//
	//RemoveHandler()
	//remove a handler from handler buffer of producer.
	//
    void RemoveHandler(RdWrServiceHandler* handler);
	//
	//GetHandlerIndex()
	//get index of a given handler in the handler buffer of producer.
	//
	size_t GetHandlerIndex(RdWrServiceHandler* handler) const;

    // Used for Singleton Pattern
    static Producer* producer_;
    static std::once_flag once_flag_;

    // Used for caching connection event handler
    std::vector<RdWrServiceHandler*> handlers_;
	//Pointer to the container of the play menu.
	TableView<PlayTableViewCell>* table_view_;
};


#endif
