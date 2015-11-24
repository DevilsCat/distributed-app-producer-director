#ifndef RD_WR_SERVICE_HANDLER_H
#define RD_WR_SERVICE_HANDLER_H

#include <ace/Asynch_IO.h>
#include <ace/Message_Block.h>
#include <string>

class Producer;

class RdWrServiceHandler : public ACE_Service_Handler {
public:
    RdWrServiceHandler();
    explicit RdWrServiceHandler(Producer& producer);

    ~RdWrServiceHandler();

    virtual void open(ACE_HANDLE new_handle, ACE_Message_Block& message_block) override;
    virtual void handle_read_stream(const ACE_Asynch_Read_Stream::Result& result) override;
    virtual void handle_write_stream(const ACE_Asynch_Write_Stream::Result& result) override;

    int InvokeSockSendRequest(std::string& message);
    void InvokeSockReadRequest(const unsigned& nbytes = 1024);
	void UpdateTableView(const std::string&);

private:
    ACE_Asynch_Read_Stream reader_;
    ACE_Asynch_Write_Stream writer_;
    Producer& producer_;
};

#endif

