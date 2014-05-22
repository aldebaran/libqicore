/*
** Author(s):
**  - Herve Cuche <hcuche@aldebaran-robotics.com>
**  - Matthieu Nottale <mnottale@aldebaran-robotics.com>
**
** Copyright (C) 2013 Aldebaran Robotics
*/

#ifndef LOGPROVIDER_HPP_
# define LOGPROVIDER_HPP_

# include <string>
# include <utility> // std::pair
# include <vector>

# include <qi/log.hpp>

# include <qicore/api.hpp>
# include <qicore/logmessage.hpp>

# include <qitype/anyobject.hpp>

namespace qi
{
  class LogManager;
  // Register local provider to service
  QICORE_API qi::Future<int> registerToLogger(qi::Object<LogManager> logger);

  /** Registers to a local or remote Logger service
 *  Sends local logger message to it
 *  Honors commands from it to configure local logger verbosity.
 *  @threadSafe
 */
  class QICORE_API LogProvider
  {
  public:
    virtual ~LogProvider() {};

    virtual void setLevel(qi::LogLevel level) = 0;
    virtual void addFilter(const std::string& filter,
                           qi::LogLevel level) = 0;
    virtual void setFilters(const std::vector<std::pair<std::string, qi::LogLevel> >& filters) = 0;
  };

  typedef qi::Object<LogProvider> LogProviderPtr;
} // !qi

#endif // !LOGPROVIDER_HPP_
