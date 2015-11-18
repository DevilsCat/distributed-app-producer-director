#include "stdafx.h"
#include "DirectorAsynchAcceptor.h"

RdWrServiceHandler* DirectorAsynchAcceptor::make_handler() {
    return new RdWrServiceHandler(*Producer::instance());
}
