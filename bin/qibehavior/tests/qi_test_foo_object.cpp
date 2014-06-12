/*
** Author(s):
**  - Cedric GESTES <gestes@aldebaran-robotics.com>
**
** Copyright (C) 2014 Aldebaran Robotics
*/
#include <qi/macro.hpp>

#define QI_TEST_FOO_OBJECT_API QI_LIB_API(qi_test_foo_object)

#include <qitype/anyvalue.hpp>
#include <qitype/objectfactory.hpp>
#include <qimessaging/session.hpp>

class QI_TEST_FOO_OBJECT_API Foo
{
public:
  Foo(qi::SessionPtr s) {}

  int getv() { return v.get();}
  void setv(int i) { v.set(i);}
  int add(int i) { int res =  i+v.get(); lastRes = res; onAdd(res); return res;}
  int lastAdd() { return lastRes;}
  qi::Property<int> v;
  qi::Signal<int> onAdd;
  int lastRes;
};
QI_REGISTER_OBJECT(Foo, add, getv, setv, v, onAdd, lastAdd);
QI_REGISTER_OBJECT_FACTORY_CONSTRUCTOR(Foo, qi::SessionPtr);


class QI_TEST_FOO_OBJECT_API Bar
{
public:
  Bar(qi::SessionPtr s) {}

  int getv() { return v.get();}
  void setv(int i) { v.set(i);}
  int add(int i) { int res =  i+v.get(); lastRes = res; onAdd(res); return res;}
  int lastAdd() { return lastRes;}
  qi::Property<int> v;
  qi::Signal<int> onAdd;
  int lastRes;
};
QI_REGISTER_OBJECT(Bar, add, getv, setv, v, onAdd, lastAdd);

QI_REGISTER_OBJECT_FACTORY_CONSTRUCTOR(Bar, qi::SessionPtr);
