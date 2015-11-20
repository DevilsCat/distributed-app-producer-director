#ifndef RD_WR_SERVICE_HANDLER
#define RD_WR_SERVICE_HANDLER

#include <ace/Asynch_IO.h>
#include <ace/Message_Block.h>
#include "Producer.h"

class RdWrServiceHandler : public ACE_Service_Handler {
public:
    RdWrServiceHandler();
    explicit RdWrServiceHandler(Producer& producer);

    ~RdWrServiceHandler();

    virtual void open(ACE_HANDLE new_handle, ACE_Message_Block& message_block) override;

    virtual void handle_read_stream(const ACE_Asynch_Read_Stream::Result& result) override;

    virtual void handle_write_stream(const ACE_Asynch_Write_Stream::Result& result) override;

    int InvokeSend(std::string& message);
private:
    void InvokeNewRead(const unsigned& nbytes = 1024);

    ACE_Asynch_Read_Stream reader_;
    ACE_Asynch_Write_Stream writer_;
    Producer& producer_;
};

#endif

