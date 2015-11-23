#include "stdafx.h"
#include "RdWrServiceHandler.h"
#include <ace/OS.h>
#include "ViewRenderer.h"
#include "SockMsgHandler.h"

RdWrServiceHandler::RdWrServiceHandler(): producer_(*Producer::instance()) {}

RdWrServiceHandler::RdWrServiceHandler(Producer& producer) : producer_(producer) {}

RdWrServiceHandler::~RdWrServiceHandler() {
    PROGRAM_DEBUG("[%x]RdWrServiceHandler: Connection Destroyed.", this);
    if (ACE_Handler::handle() != ACE_INVALID_HANDLE)
        ACE_OS::closesocket(ACE_Handler::handle());

    // Remove this handler from producer handlers container.
    producer_.RemoveHandler(this);
}

void RdWrServiceHandler::open(ACE_HANDLE new_handle, ACE_Message_Block& message_block) {
    PROGRAM_DEBUG("[%x]Connection established.", this);
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

    InvokeRead();
}

void RdWrServiceHandler::handle_read_stream(const ACE_Asynch_Read_Stream::Result& result) {
    ACE_Message_Block& mb = result.message_block();
    if (!result.success() || result.bytes_transferred() == 0) {  // Delete this handler since it's broken.
        mb.release();
        delete this;
    } else {
        PROGRAM_DEBUG("%s", mb.rd_ptr());
        InvokeRead();
    }
	SockMsgHandler::RecvMsg msg = SockMsgHandler::instance()->Recive(mb.rd_ptr());
	msg.print();
}

void RdWrServiceHandler::handle_write_stream(const ACE_Asynch_Write_Stream::Result& result) {
    result.message_block().release();
}

int RdWrServiceHandler::InvokeSend(std::string& message) {
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

void RdWrServiceHandler::InvokeRead(const unsigned& nbytes) {
    ACE_Message_Block *mb;
    ACE_NEW_NORETURN(mb, ACE_Message_Block(nbytes));
    if (this->reader_.read(*mb, mb->space()) != 0) {
        ACE_ERROR((LM_ERROR, ACE_TEXT("%p\n"),
            ACE_TEXT("RdWrServiceHandler begin read")));
        mb->release();
        delete this;
    }
}
