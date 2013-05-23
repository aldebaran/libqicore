#include <boost/algorithm/string.hpp>

#include <qicore/behavior.hpp>


namespace qi {

  static inline std::pair<std::string, std::string> splitString2(const std::string& s, char sep)
  {
    size_t p = s.find_first_of(sep);
    if (p == s.npos)
      throw std::runtime_error("separator not found");
    return std::make_pair(s.substr(0, p), s.substr(p+1));
  }

  void BehaviorModel::clear()
  {
    nodes.clear();
    transitions.clear();
  }

  void BehaviorModel::load(std::istream& is)
  {
    // uid a.b -> c.d
    // uid model factory
    char line[1024];
    while (is.good())
    {
      line[0] = 0;
      is.getline(line, 1024);
      std::string s(line);
      if (s.empty() || s[0] == '#')
        continue;
      std::stringstream st(s);
      std::string uid, p1, p2;
      bool t = false;
      st >> uid >> p1 >> p2;
      if (p2.empty())
        continue;
      if (p2 == "->")
      {
        t = true;
        st >> p2;
      }
      if (!t)
      {
        BehaviorModel::Node& state = nodes[uid];
        state.uid = uid;
        state.interface = p1;
        state.factory = p2;

        std::string parameters;
        std::getline(st,parameters);
        size_t p = 0;
        std::string::const_iterator begin = parameters.begin();
        while (p <= parameters.size())
        {
          size_t eq = parameters.find_first_of('=', p);
          if (eq == parameters.npos)
          { // check for remaining data
            std::string remain = parameters.substr(p);
            boost::trim(remain);
            if (!remain.empty())
              throw std::runtime_error("Trailing data: " + remain);
            break;
          }
          std::string name = parameters.substr(p, eq-p);
          boost::algorithm::trim(name);
          qi::GenericValue gValue;
          size_t end = decodeJSON(begin + eq + 1, begin + parameters.size(), gValue) - begin;
          state.parameters[name] = gValue;
          p = end;
        }
      }
      else
      {
        BehaviorModel::Transition transition;
        transition.uid = uid;
        transition.src = splitString2(p1, '.');
        transition.dst = splitString2(p2, '.');
        transitions[transition.uid] = transition;
      }
    }
  }

}
