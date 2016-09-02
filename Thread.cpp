#include "StdAfx.h"
#include "Thread.h"
#include "MessageLoop.h"

Thread::Thread(void)
  : thread_(NULL) 
  , loop_(NULL) {

  init_event_ = ::CreateEvent(NULL, TRUE, FALSE, NULL);
}

Thread::~Thread(void) {
  Shutdown();
  CloseHandle(init_event_);
}

DWORD __stdcall WorkThreadMain(LPVOID lpThreadParameter) {

  Thread* thread = static_cast<Thread*>(lpThreadParameter);
  if (thread) {
    thread->Run();
  }
  return 0;
}

void Thread::Start() {

  if (thread_) {
    return ;
  }

  thread_ = ::CreateThread(NULL, 0, WorkThreadMain, this, NULL, NULL);
  ::ResetEvent(init_event_);
  ::WaitForSingleObject(init_event_, INFINITE);
}

void Thread::Shutdown() {
  LOG(INFO) << __FUNCTION__ " | Enter";

  if (loop_) {
    loop_->Quit();
  }

  if (thread_) {
    ::WaitForSingleObject(thread_, INFINITE);
    ::CloseHandle(thread_);
    thread_ = NULL;
  }

  LOG(INFO) << __FUNCTION__ " | Exit";
}

void Thread::Run() {
  loop_ = new MessageLoopBase();
  ::SetEvent(init_event_);
  loop_->RunLoop();
  delete loop_;
  loop_ = NULL;
}