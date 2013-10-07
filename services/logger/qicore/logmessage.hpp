/*
** Author(s):
**  - Herve Cuche <hcuche@aldebaran-robotics.com>
**  - Matthieu Nottale <mnottale@aldebaran-robotics.com>
**
** Copyright (C) 2013 Aldebaran Robotics
*/

#ifndef QICORE_LOG_HPP_
# define QICORE_LOG_HPP_

# include <qi/log.hpp>
# include <qitype/anyobject.hpp>

QI_TYPE_ENUM_REGISTER(qi::LogLevel)

namespace qi
{
  struct LogMessage
  {
    std::string source;
    qi::LogLevel level;
    qi::os::timeval timestamp;
    std::string category;
    std::string location;
    std::string message;
  };
}
QI_TYPE_STRUCT_REGISTER(::qi::LogMessage, source, level, timestamp, category, location, message);

#endif // !QICORE_LOG_HPP_
