#include "enemy_behavior_type.h"

#include <cassert>
#include <sstream>
#include <vector>

void test_enemy_behavior_type()
{
  const enemy_behavior_type t{enemy_behavior_type::gezellig};
  const enemy_behavior_type u{enemy_behavior_type::shy};
  static_assert(t != u,"");
  {
    std::stringstream s;
    s << enemy_behavior_type::gezellig;
  }
}

std::stringstream& operator << (std::stringstream &out, const enemy_behavior_type &enemy_type)
{

        if(enemy_type == enemy_behavior_type::gezellig)
        out << "gezillig";
        else if(enemy_type == enemy_behavior_type::shy)
        out << "shy";
        else
        out << "unknown";

    return out;
}
