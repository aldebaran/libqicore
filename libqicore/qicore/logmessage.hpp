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
    std::string source;        // File:function:line
    qi::LogLevel level;        // Level of verbosity of the message
    qi::os::timeval timestamp; // timestamp when the message have been posted
    std::string category;      // Category of the message
    std::string location;      // machineID:PID
    std::string message;       // The message itself
  };
}
QI_TYPE_STRUCT(::qi::LogMessage, source, level, timestamp, category, location, message);

#endif // !QICORE_LOG_HPP_
