#ifndef BOOTSETUP_MESSAGELOOP_H_
#define BOOTSETUP_MESSAGELOOP_H_

#pragma once
#include "CallBack.h"
#include <set>

class Fifo;
class ITask;

typedef CallBack<void> VoidCallBack;
class ApplicationWndWrapper;

/** Message loop
* 
* @author welbon
*
* @date 2012-08-09
*/
class MessageLoopBase {
public:
  MessageLoopBase();
  virtual ~MessageLoopBase();

  virtual void PostTask(ITask* task, DWORD time_out = 0);
  virtual void PostTask(VoidCallBack* call_back, DWORD time_out = 0);
  virtual void RunLoop();
  virtual void Quit();
  
protected:
  Fifo* fifo_;
  bool running_;
};

class MessageLoopUI : public MessageLoopBase {
public:
  MessageLoopUI();
  virtual ~MessageLoopUI();

  virtual void PostTask(ITask* task, DWORD time_out = 0);
  virtual void PostTask(VoidCallBack* call_back, DWORD time_out = 0);
  virtual void RunLoop();
  void SetHwnd(HWND hwnd) { hwnd_ = hwnd; }
  void OnWindowMessage(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
  
private:
  HWND hwnd_;
  long timer_id_offset_;
  typedef std::set<long> TimerIDSet;
  TimerIDSet delay_taskid_set_;
};


#endif //BOOTSETUP_MESSAGELOOP_H_