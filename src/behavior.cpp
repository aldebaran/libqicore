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
        while (true)
        {
          std::string p;
          st >> p;
          if (p.empty())
            break;
          size_t sep = p.find_first_of('=');
          if (sep == p.npos)
            throw std::runtime_error("No '=' found in parameter " + p);
          std::string key = p.substr(0, sep);
          std::string val = p.substr(sep + 1);
          // FIXME call parseText when someone will implement it
          qi::GenericValue gValue;
          char* lend;
          long lval = strtol(val.c_str(), &lend, 0);
          if (lend == val.c_str() + val.size())
            gValue = qi::GenericValueRef(lval);
          else
          {
            char* dend;
            double dval = strtod(val.c_str(), &dend);
            if (dend == val.c_str() + val.size())
              gValue = qi::GenericValueRef(dval);
            else
              gValue = qi::GenericValueRef(val);
          }
          state.parameters[key] = gValue;
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
