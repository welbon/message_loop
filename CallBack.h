#ifndef BOOTSETUP_CALLBACK_H_
#define BOOTSETUP_CALLBACK_H_

#pragma once

template<typename ReturnType>
class CallBack {
public:
  virtual ~CallBack() {}
  virtual ReturnType Exec() = 0;
};

template<typename ReturnType, typename ClassType>
class CallBack_0 : public CallBack<ReturnType> {
public:
  typedef ReturnType (ClassType::*Method)();
  CallBack_0(ClassType* type, Method method)
    : class_type_(type)
    , method_(method){
  }

  virtual ReturnType Exec() {
    if (class_type_) {
      (class_type_->*method_)();
    }
  }

private:
  ClassType* class_type_;
  Method method_;
};

template<typename ReturnType, typename ClassType, typename P1>
class CallBack_1 : public CallBack<ReturnType> {
public:
  typedef ReturnType (ClassType::*Method)(P1 p1);
  CallBack_1(ClassType* type, Method method, P1 p1)
    : class_type_(type)
    , p1_(p1) 
    , method_(method){
  }

  virtual ReturnType Exec() {
    if (class_type_) {
      (class_type_->*method_)(p1_);
    }
  }

private:
  ClassType* class_type_;
  P1 p1_;
  Method method_;
};

template<typename ReturnType, typename ClassType, typename P1, typename P2>
class CallBack_2 : public CallBack<ReturnType> {
public:
  typedef ReturnType (ClassType::*Method)(P1 p1, P2 p2);
  CallBack_2(ClassType* type, Method method, P1 p1, P2 p2)
    : class_type_(type)
    , p1_(p1)
    , p2_(p2)
    , method_(method){
  }

  virtual ReturnType Exec() {
    if (class_type_) {
      (class_type_->*method_)(p1_, p2_);
    }
  }

private:
  ClassType* class_type_;
  P1 p1_;
  P2 p2_;
  Method method_;
};

template<typename ReturnType, typename ClassType, typename P1, typename P2, typename P3>
class CallBack_3 : public CallBack<ReturnType> {
public:
  typedef ReturnType (ClassType::*Method)(P1, P2, P3);
  CallBack_3(ClassType* type, Method method, P1 p1, P2 p2, P3 p3)
    : class_type_(type)
    , p1_(p1)
    , p2_(p2)
    , p3_(p3)
    , method_(method){
  }

  virtual ReturnType Exec() {
    if (class_type_) {
      (class_type_->*method_)(p1_, p2_, p3_);
    }
  }

private:
  ClassType* class_type_;
  P1 p1_;
  P2 p2_;
  P3 p3_;
  Method method_;
};

template< typename ReturnType, typename ClassType>
extern CallBack<ReturnType>* 
MakeCallBack(ClassType* type, ReturnType(ClassType::*Method)()) {
  return new CallBack_0<ReturnType, ClassType>(type, Method);
}

template< typename ReturnType, typename ClassType,  typename P1>
extern CallBack<ReturnType>* 
MakeCallBack(ClassType* type, ReturnType(ClassType::*Method)(P1), P1 p1) {
  return new CallBack_1<ReturnType, ClassType, P1>(type, Method, p1);
}

template< typename ReturnType, typename ClassType,  typename P1, typename P2>
extern CallBack<ReturnType>* 
MakeCallBack(ClassType* type, ReturnType(ClassType::*Method)(P1, P2), P1 p1, P2 p2) {
  return new CallBack_2<ReturnType, ClassType, P1, P2>(type, Method, p1, p2);
}

template< typename ReturnType, typename ClassType,  typename P1, typename P2, typename P3>
extern CallBack<ReturnType>* 
MakeCallBack(ClassType* type, ReturnType(ClassType::*Method)(P1, P2, P3), P1 p1, P2 p2, P3 p3) {
  return new CallBack_3<ReturnType, ClassType, P1, P2, P3>(type, Method, p1, p2, p3);
}

#endif //