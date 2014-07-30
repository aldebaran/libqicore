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
# include <qi/anyobject.hpp>

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
    unsigned int id;           // Unique message ID
  };
}
QI_TYPE_STRUCT(::qi::LogMessage, source, level, timestamp, category, location, message, id);

#endif // !QICORE_LOG_HPP_
