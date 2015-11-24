#include "stdafx.h"
#include "Producer.h"
#include "RdWrServiceHandler.h"
#include <ace/Reactor.h>
#include <ace/Proactor.h>
#include <thread>
#include "ViewRenderer.h"

Producer* Producer::producer_ = nullptr;
std::once_flag Producer::once_flag_;

Producer* Producer::instance() {
    std::call_once(once_flag_, []{ producer_ = new Producer; });
    return producer_;
}

Producer::Producer() {
	table_view_ = dynamic_cast<TableView<PlayTableViewCell>*>(ViewRenderer::instance()->GetView("Play"));
}

Producer::~Producer() {
    if (producer_)
        delete producer_;
}

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
    PROGRAM_DEBUG("Producer: Executing start <%d> command.", num);
    if (table_view_->Size() <= num){ return; }  //FIXME do we want to handle this using exception
    
	// Retrieve cell from table view for sending
	std::shared_ptr<PlayTableViewCell> cell = table_view_->GetCellAt(num);
	std::string msg("start ");
	msg += std::to_string(cell->play_id());
    handlers_[cell->director_id()]->InvokeSend(msg);
}

void Producer::Stop(const unsigned& num) {
    PROGRAM_DEBUG("Producer: Executing stop <%d> command.", num);
    if (handlers_.size() <= num){ return; }  //FIXME do we want to handle this using exception
    std::string msg("stop");
    handlers_[num]->InvokeSend(msg);
}

void Producer::Quit() const {
    PROGRAM_DEBUG("Producer: Executing quit command.");

    // Broadcast a quit message to all Directors.
    std::string msg("quit");
    for_each(handlers_.begin(), handlers_.end(), 
        [&msg](RdWrServiceHandler* handler){ handler->InvokeSend(msg); });

    // Wait on all Director quit, which means all handlers will be removed by proactor
    // from RdWrEventHandler's deconstructor.
    while (handlers_.size())
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // Quit itself
    ACE_Reactor::instance()->end_event_loop();
    ACE_Proactor::instance()->end_event_loop();
}