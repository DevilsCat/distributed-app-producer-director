#include "stdafx.h"
#include "CommandQueue.h"

CommandQueue* CommandQueue::queue_ = nullptr;
std::once_flag CommandQueue::once_flag_;