#ifndef BOOTSETUP_THREAD_H_
#define BOOTSETUP_THREAD_H_

#pragma once
class MessageLoopBase;

class Thread
{
public:
  Thread(void);
  virtual ~Thread(void);

  void Start();
  void Shutdown();
  MessageLoopBase* GetMessageLoop() { 
    return loop_; 
  }

  // Called by work thread
  // don't call this thread by your self.
  void Run();

protected:
  HANDLE thread_;
  HANDLE init_event_;
  MessageLoopBase* loop_;
};

#endif //BOOTSETUP_THREAD_H_