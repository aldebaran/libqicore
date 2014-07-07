/*
** Author(s):
**  - Cedric GESTES <gestes@aldebaran.com>
**
** Copyright (C) 2014 Aldebaran
*/
#include "logproviderimpl.hpp"
#include <qi/anymodule.hpp>

void registerLibQiCore(qi::ModuleBuilder* mb) {
  qi::registerLogProvider(mb);
}

QI_REGISTER_MODULE("qicore", &registerLibQiCore);
