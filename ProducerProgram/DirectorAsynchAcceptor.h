#ifndef DIRECTOR_ASYNCH_ACCEPTOR_H
#define DIRECTOR_ASYNCH_ACCEPTOR_H
#include <ace/Asynch_Acceptor.h>
#include "RdWrServiceHandler.h"

class DirectorAsynchAcceptor : public ACE_Asynch_Acceptor<RdWrServiceHandler> {
protected:
    // Override this to make the handler contain a Producer reference.
    virtual RdWrServiceHandler* make_handler() override;
};

#endif

