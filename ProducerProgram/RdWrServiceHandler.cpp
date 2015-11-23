﻿#include "stdafx.h"
#include "RdWrServiceHandler.h"
#include <ace/OS.h>
#include "SockMsgHandler.h"

RdWrServiceHandler::RdWrServiceHandler(): producer_(*Producer::instance()) {}

RdWrServiceHandler::RdWrServiceHandler(Producer& producer) : producer_(producer) {}

RdWrServiceHandler::~RdWrServiceHandler() {
    ACE_DEBUG((LM_INFO, "[%x]RdWrServiceHandler: Connection Destroyed.\n", this));
    if (ACE_Handler::handle() != ACE_INVALID_HANDLE)
        ACE_OS::closesocket(ACE_Handler::handle());

    // Remove this handler from producer handlers container.
    producer_.RemoveHandler(this);
}

void RdWrServiceHandler::open(ACE_HANDLE new_handle, ACE_Message_Block& message_block) {
    ACE_DEBUG((LM_INFO, "[%x]Connection established.\n", this));
    this->handle(new_handle);
    if (this->reader_.open(*this) || this->writer_.open(*this)) {
        ACE_ERROR((LM_ERROR, ACE_TEXT("%p\n"),
            ACE_TEXT("RdWrServiceHandler open")
            ));
        delete this;
        return;
    }

    // FIXME @Proactor thread?, add this handler to producer.
    producer_.AddHandler(this);

    InvokeNewRead();
}

void RdWrServiceHandler::handle_read_stream(const ACE_Asynch_Read_Stream::Result& result) {
    ACE_Message_Block& mb = result.message_block();
    ACE_DEBUG((LM_INFO, "bytes transferred: %d\n", result.bytes_transferred()));
    if (!result.success() || result.bytes_transferred() == 0) {  //FIXME do we want to delete this handler?
        mb.release();
        delete this;
    } else {
        ACE_DEBUG((LM_INFO, "%s", mb.rd_ptr()));
        InvokeNewRead();
        //if (this->writer_.write(mb, mb.length()) != 0) {  //write failed
        //    ACE_ERROR((LM_ERROR, ACE_TEXT("%p\n"),
        //        ACE_TEXT("RdWrServiceHandler starting write")));
        //    mb.release();
        //} else {
        //    InvokeNewRead();
        //}
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

void RdWrServiceHandler::InvokeNewRead(const unsigned& nbytes) {
    ACE_Message_Block *mb;
    ACE_NEW_NORETURN(mb, ACE_Message_Block(nbytes));
    ACE_DEBUG((LM_INFO, "Block Space: %d\n", mb->space()));
    if (this->reader_.read(*mb, mb->space()) != 0) {
        ACE_ERROR((LM_ERROR, ACE_TEXT("%p\n"),
            ACE_TEXT("RdWrServiceHandler begin read")));
        mb->release();
        delete this;
    }
}
