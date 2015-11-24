#include "stdafx.h"
#include "RdWrServiceHandler.h"
#include <ace/OS.h>
#include "ViewRenderer.h"
#include "SockMsgHandler.h"

RdWrServiceHandler::RdWrServiceHandler(): producer_(*Producer::instance()) {}

RdWrServiceHandler::RdWrServiceHandler(Producer& producer) : producer_(producer) {}

RdWrServiceHandler::~RdWrServiceHandler() {
    PROGRAM_DEBUG("RdWrServiceHandler: Id[%d] Connection Destroyed.", producer_.GetHandlerIndex(this));
    if (ACE_Handler::handle() != ACE_INVALID_HANDLE)
        ACE_OS::closesocket(ACE_Handler::handle());
    
    // Delete the cells associated with this on deleting director connection.
    producer_.table_view_->Delete([this](const PlayTableViewCell& cell) {
        return cell.director_id() == producer_.GetHandlerIndex(this);
    });

    // Update other cells decreasing index by 1 after "this" index.
    producer_.table_view_->Update(
        [this](const PlayTableViewCell& cell) { return size_t(cell.director_id()) > producer_.GetHandlerIndex(this); },
        [](PlayTableViewCell& cell) { cell.set_director_id(cell.director_id() - 1); }
    );
    // Remove this handler from producer handlers container.
    producer_.RemoveHandler(this);

    // Render the updated play if is present.
    ViewRenderer::instance()->Render("Play");
}

void RdWrServiceHandler::open(ACE_HANDLE new_handle, ACE_Message_Block& message_block) {
    this->handle(new_handle);
    if (this->reader_.open(*this) || this->writer_.open(*this)) {
        ACE_ERROR((LM_ERROR, ACE_TEXT("%p\n"),
            ACE_TEXT("RdWrServiceHandler open")
            ));
        delete this;
        return;
    }
    // Add this handler to producer.
    producer_.AddHandler(this);
    InvokeSockReadRequest();

    PROGRAM_DEBUG("RdWrServiceHandler: Id[%d] Connection established.", producer_.GetHandlerIndex(this));
}

void RdWrServiceHandler::handle_read_stream(const ACE_Asynch_Read_Stream::Result& result) {
    ACE_Message_Block& mb = result.message_block();
    if (!result.success() || result.bytes_transferred() == 0) {  // Delete this handler since it's broken.
        mb.release();
        delete this;
    } else {
        PROGRAM_DEBUG("%s", mb.rd_ptr());
        InvokeSockReadRequest();
        UpdateTableView(mb.rd_ptr());
    }
}

void RdWrServiceHandler::handle_write_stream(const ACE_Asynch_Write_Stream::Result& result) {
    result.message_block().release();
}

int RdWrServiceHandler::InvokeSockSendRequest(std::string& message) {
    ACE_Message_Block* mb;
    ACE_NEW_RETURN(mb, ACE_Message_Block(message.size()+1), -1);  //FIXME do we want to return -1
    mb->copy(message.c_str(), message.size() + 1);
    int ret = this->writer_.write(*mb, mb->length());
    if (ret != 0) {
        ACE_ERROR((LM_ERROR, ACE_TEXT("%p\n"),
            ACE_TEXT("RdWrServiceHandler starting write")));
        mb->release();
    }
    return ret;
}

void RdWrServiceHandler::InvokeSockReadRequest(const unsigned& nbytes) {
    ACE_Message_Block *mb;
    ACE_NEW_NORETURN(mb, ACE_Message_Block(nbytes));
    if (this->reader_.read(*mb, mb->space()) != 0) {
        ACE_ERROR((LM_ERROR, ACE_TEXT("%p\n"),
            ACE_TEXT("RdWrServiceHandler begin read")));
        mb->release();
        delete this;
    }
}

void RdWrServiceHandler::UpdateTableView(const std::string& msg) {
    std::vector<std::string> MsgToken = tokenize(msg);
    if (SockMsgHandler::instance()->Validate(SockMsgHandler::FeedBackMsgType::kPlaylist, MsgToken)) {	//add new play table view cell
        for (size_t i = 2; i < MsgToken.size(); i++) {
            producer_.table_view_->AddCell(new PlayTableViewCell(
                producer_.GetHandlerIndex(this),
                i - 2,
                MsgToken[i], 
                PlayTableViewCell::kAvailable)
            );
            ViewRenderer::instance()->Render("Play");
        }
    }
    if (SockMsgHandler::instance()->Validate(SockMsgHandler::FeedBackMsgType::kStatus, MsgToken)) {	//update play table view cell
        producer_.table_view_->Update (
            [this](const PlayTableViewCell& cell) { return size_t(cell.director_id()) == producer_.GetHandlerIndex(this); },
            [this, &MsgToken](PlayTableViewCell& cell) {
                if (MsgToken[1] == "available") {
                    cell.set_status(PlayTableViewCell::kAvailable);
                } else {
                    int inprog_idx = to_number(MsgToken[2]);
                    cell.play_id() == inprog_idx ?
                        cell.set_status(PlayTableViewCell::kInProgress) : 
                        cell.set_status(PlayTableViewCell::kUnavailable);
                }
            }
        );
        ViewRenderer::instance()->Render("Play");
    }

}