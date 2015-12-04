#include "stdafx.h"
#include "Producer.h"
#include "RdWrServiceHandler.h"
#include <ace/Reactor.h>
#include <ace/Proactor.h>
#include <thread>
#include "ViewRenderer.h"
#include "SockMsgHandler.h"

Producer* Producer::producer_ = nullptr;
std::once_flag Producer::once_flag_;

Producer* Producer::instance() {
    std::call_once(once_flag_, []{ producer_ = new Producer; });
    return producer_;
}

Producer::Producer() {
    table_view_ = ViewRenderer::instance()->GetView<TableView<PlayTableViewCell>>("Play");
}

Producer::Producer(const Producer&other) : table_view_(other.table_view_) {}

Producer::~Producer() {
    if (producer_)
        delete producer_;
}

Producer& Producer::operator=(const Producer&) { return *this; }

void Producer::AddHandler(RdWrServiceHandler* handler) {
    handlers_.push_back(handler);
}

void Producer::RemoveHandler(RdWrServiceHandler* handler) {
    std::vector<RdWrServiceHandler*>::iterator it = find(handlers_.begin(), handlers_.end(), handler);
    if (it == handlers_.end()) { return; }  // FIXME this is a very big logic error.
    handlers_.erase(it);
}

size_t Producer::GetHandlerIndex(RdWrServiceHandler* handler) const {
	std::vector<RdWrServiceHandler*>::const_iterator it = find(handlers_.begin(), handlers_.end(), handler);
	return it - handlers_.begin();
}

void Producer::Start(const unsigned& num) {
    if (table_view_->Size() <= num) {
        PROGRAM_DEBUG("Producer: Executing start command, but no connection exist.");
        return;
    }
	// Retrieve cell from table view for sending
	std::shared_ptr<PlayTableViewCell> cell = table_view_->GetCellAt(num);
    std::string msg = SockMsgHandler::instance()->MakeSendMsg(SockMsgHandler::kStart, cell->play_id());
    handlers_[cell->director_id()]->InvokeSockSendRequest(msg);

    PROGRAM_DEBUG("Producer: Executing start <%d> command on Id[%d].", num, cell->director_id());
}

void Producer::Stop(const unsigned& num) {
    if (table_view_->Size() <= num) {
        PROGRAM_DEBUG("Producer: Executing stop command, but no connection exist.");
        return;
    }
    std::shared_ptr<PlayTableViewCell> cell = table_view_->GetCellAt(num);
    std::string msg = SockMsgHandler::instance()->MakeSendMsg(SockMsgHandler::kStop, cell->play_id());
    handlers_[cell->director_id()]->InvokeSockSendRequest(msg);

    PROGRAM_DEBUG("Producer: Executing stop <%d> command on Id[%d].", num, cell->director_id());
}

void Producer::Quit() const {
    PROGRAM_DEBUG("Producer: Executing quit command.");

    // Broadcast a quit message to all Directors.
    std::string msg = SockMsgHandler::instance()->MakeSendMsg(SockMsgHandler::kQuit);
    for_each(handlers_.begin(), handlers_.end(), [&msg](RdWrServiceHandler* handler) {
        handler->InvokeSockSendRequest(msg);
    });

    // Wait on all Director quit, which means all handlers will be removed by proactor
    // from RdWrEventHandler's deconstructor.
    while (handlers_.size())
        std::this_thread::yield();

    // Quit itself
    ACE_Reactor::instance()->end_event_loop();
    ACE_Proactor::instance()->end_event_loop();

    raise(SIGABRT);  // this one stop the UI thread.
}

RdWrServiceHandler* Producer::make_handler() {
    return new RdWrServiceHandler(*this);
}
