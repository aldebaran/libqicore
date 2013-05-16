/*
**  Copyright (C) 2012 Aldebaran Robotics
**  See COPYING for the license
*/

#pragma once

#ifndef QICORE_API_H_
#define QICORE_API_H_

#include <qi/macro.hpp>

# ifndef SWIG
#  define QICORECOMPAT_API QI_LIB_API(qicore-compat)
# else
#  define QICORECOMPAT_API
# endif

#endif  /* !QICORE_API_H_ */
