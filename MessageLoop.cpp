#include "StdAfx.h"
#include "MessageLoop.h"
#include <list>
#include "Task.h"
#include <assert.h>

class Fifo {
public:
  Fifo(void) {
    ::InitializeCriticalSection(&cs_);
  }

  virtual ~Fifo(void) {
    {
      ::EnterCriticalSection(&cs_);

      TASKFIFO::iterator it = task_fifo_.begin();
      for (; it != task_fifo_.end(); ++ it) {
        delete (*it);
      }
      task_fifo_.clear();

      ::LeaveCriticalSection(&cs_);
    }
    ::DeleteCriticalSection(&cs_);
  }

  void AddTask(ITask* task, DWORD time_out = 0) {
    if (!task) {
      return ;
    }

    FifoElement* element = new FifoElement(task, time_out);
    {
      ::EnterCriticalSection(&cs_);
      task_fifo_.push_back(element);
      ::LeaveCriticalSection(&cs_);
    }
  }

  void DispatchTask() {
    if (task_fifo_.empty()) {
      return ;
    }

    FifoElement* element = NULL;

    ::EnterCriticalSection(&cs_);
    {
      element = task_fifo_.front();
      task_fifo_.pop_front();
      if (element->spend_time_ > 0 && 
         (GetTickCount() - element->generate_time_) < element->spend_time_) 
      { // If this task is delay task, we put it in fifo back
        task_fifo_.push_back(element);
        element = NULL;
      }
    }
    ::LeaveCriticalSection(&cs_);

    if (element && element->task_) {
      element->task_->Run();
      delete element;
      element = NULL;
    }
  }

  void DispatchDelayTask() {
    if (task_fifo_.empty()) {
      return ;
    }

    FifoElement* element = NULL;

    ::EnterCriticalSection(&cs_);
    do  {
      element = task_fifo_.front();
      task_fifo_.pop_front();
      if (element->spend_time_ <= 0) 
      { // If this task is delay task, we put it in fifo back
        task_fifo_.push_back(element);
        element = NULL;
      }
      else if ((GetTickCount() - element->generate_time_) < element->spend_time_){
        LOG(ERR) << __FUNCTION__ << L"delay time not correct!";
        break;
      }
      else {
        break;
      }
    } while (1);
    ::LeaveCriticalSection(&cs_);

    if (element && element->task_) {
      element->task_->Run();
      delete element;
      element = NULL;
    }
  }
protected:

  class FifoElement {
  public:
    FifoElement(ITask* task, DWORD time_out) 
      : task_(task), spend_time_(time_out) {
      if (task) {
        task->AddRef();
      }
      generate_time_ = GetTickCount();
    }

    virtual ~FifoElement() {
      if (task_) {
        task_->Release();
      }
    }

    ITask* task_;
    DWORD spend_time_;
    DWORD generate_time_;
  };

protected:
  typedef std::list<FifoElement*> TASKFIFO;
  TASKFIFO task_fifo_;
  CRITICAL_SECTION cs_;
};

////////////////////////////////////////////////////////////////////////
// Call back task

class CallBackTask : public ITask {
public:
  CallBackTask(VoidCallBack* call_back)
    : call_back_(call_back) {

  }

  virtual ~CallBackTask() {
    delete call_back_;
  }

  void Run() {
    if (call_back_) {
      call_back_->Exec();
    }
  }

  void Abort() {}

protected:
  VoidCallBack* call_back_;
};

////////////////////////////////////////////////////////////////////////
// 

static const int kMsgHaveWork = WM_USER + 1;

MessageLoopBase::MessageLoopBase()
  : running_(false)
  , fifo_(new Fifo()) {

  LOG(INFO) << __FUNCTION__;
}

MessageLoopBase::~MessageLoopBase() {

  LOG(INFO) << __FUNCTION__;

  if (fifo_) {
    delete fifo_;
    fifo_ = NULL;
  }
}

void MessageLoopBase::PostTask(ITask* task, DWORD time_out) {
  fifo_->AddTask(task, time_out);
}

void MessageLoopBase::PostTask(VoidCallBack* call_back, DWORD time_out) {
  CallBackTask* task = new CallBackTask(call_back);
  fifo_->AddTask(task, time_out);
}

void MessageLoopBase::RunLoop() {
  running_ = true;
  while (running_) {
    fifo_->DispatchTask();
    Sleep(50);
  }
}

void MessageLoopBase::Quit() {
  running_ = false;
}

///////////////////////////////////////////////////////////////////////////////
// MessageLoopUI

MessageLoopUI::MessageLoopUI() : hwnd_(NULL), timer_id_offset_(0) {
  LOG(INFO) << __FUNCTION__;
}

MessageLoopUI::~MessageLoopUI() {
  LOG(INFO) << __FUNCTION__;
}

void MessageLoopUI::PostTask(ITask* task, DWORD time_out) {

  MessageLoopBase::PostTask(task, time_out);

  if (time_out <= 0) {
    PostMessage(hwnd_, kMsgHaveWork, 0, 0);
  }
  else {
    ++ timer_id_offset_;
    ::SetTimer(hwnd_, timer_id_offset_, time_out, NULL);
    delay_taskid_set_.insert(timer_id_offset_);
  }
}

void MessageLoopUI::PostTask(VoidCallBack* call_back, DWORD time_out) {

  CallBackTask* task = new CallBackTask(call_back);
  MessageLoopUI::PostTask(task, time_out);
}

void MessageLoopUI::OnWindowMessage(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
  if (!hwnd_) {
    return ;
  }

  if (hwnd_ != hwnd) {
    assert(false);
    return ;
  }

  if (message == kMsgHaveWork) {
    fifo_->DispatchTask();
  }
  else if (message == WM_TIMER) {
    fifo_->DispatchDelayTask();
    long id = static_cast<long>(wParam);
    if (id != 0) {
      ::KillTimer(hwnd, id);
    }

    TimerIDSet::iterator it = delay_taskid_set_.find(id);
    if (it != delay_taskid_set_.end()) {
      delay_taskid_set_.erase(it);
    }
  }
}

void MessageLoopUI::RunLoop() {

  running_ = true;

  MSG msg = { 0 };

  // Main message loop:
  for (;;) {
    BOOL ret = ::GetMessage(&msg, NULL, 0, 0);
    if (ret) {
      // Dispatch window message
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    if (!running_) {
      break;
    }
  }
}
