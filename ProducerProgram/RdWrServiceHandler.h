#ifndef RD_WR_SERVICE_HANDLER_H
#define RD_WR_SERVICE_HANDLER_H

#include <ace/Asynch_IO.h>
#include <ace/Message_Block.h>
#include "Producer.h"

class RdWrServiceHandler : public ACE_Service_Handler {
public:
	//
	//default constructor
	//
    RdWrServiceHandler();
	//
	//When a director is disconnected, delete all cells related to that director in display table view
	//update id of other cells after "this" cell by decreading id by 1
	//
	explicit RdWrServiceHandler(Producer& producer);

    ~RdWrServiceHandler();

	//
	//open()
	//virtual function overrided from ACE_Service_Handler. Open read/write stream, add this handler
	//to handler buffer of producer.And invoke another read event.
	//
    virtual void open(ACE_HANDLE new_handle, ACE_Message_Block& message_block) override;
	//
	//handle_read_stream()
	//virtual function overried from ACE_Service_Handler. Read and analyse message sent by director
	//and update play menu.
	//
    virtual void handle_read_stream(const ACE_Asynch_Read_Stream::Result& result) override;
	//
	//handler_write_stream()
	//release the message block after send an instruction to director.
	//
    virtual void handle_write_stream(const ACE_Asynch_Write_Stream::Result& result) override;

	//
	//InvokeSockSendRequest()
	//Send an instruction to director.
	//
    int InvokeSockSendRequest(std::string& message);
	//
	//InvokeSockReadRequest()
	//Read a message from director.
	//
    void InvokeSockReadRequest(const unsigned& nbytes = 1024);
	//
	//UpdateTableView()
	//Update the play menu if new dircetor connects to producer or play status is changed.
	//
	void UpdateTableView(const std::string&);

private:
	//A stream for reciving message.
    ACE_Asynch_Read_Stream reader_;
	//A stream for sending message.
    ACE_Asynch_Write_Stream writer_;
	//A reference of producer.
    Producer& producer_;
};

#endif

