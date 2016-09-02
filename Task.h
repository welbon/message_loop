#ifndef BOOTSETUP_TASK_H_
#define BOOTSETUP_TASK_H_

#pragma once

class ITask {
public:
  ITask() : reference_(0) {}
  virtual ~ITask() {}

  virtual void AddRef() {
    ++ reference_;
  }

  virtual void Release() {
    -- reference_;
    if (reference_ <= 0) {
      delete this;
    }
  }

  virtual void Run() = 0;
  virtual void Abort() = 0;

private:
  volatile long reference_;
};

#endif //BOOTSETUP_FIFO_H_