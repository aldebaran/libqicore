/*
** Author(s):
**  - Cedric GESTES <gestes@aldebaran.com>
**
** Copyright (C) 2014 Aldebaran
*/
#include "logproviderimpl.hpp"
#include <qi/anymodule.hpp>

//namespace qi {
//  void registerBehavior(qi::ModuleBuilder* mb);
//}

void registerLibQiCore(qi::ModuleBuilder* mb) {
  qi::registerLogProvider(mb);
  //qi::registerBehavior(mb);
}

QI_REGISTER_MODULE("qicore", &registerLibQiCore);
