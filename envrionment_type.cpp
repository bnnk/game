
#include "environment_type.h"
#include <cassert>

void test_environment()
{
//Conversion to string

  assert(to_str_env_type(environment_type::empty) == "empty");
  assert(to_str_env_type(environment_type::random)== "random");
  assert(to_str_env_type(environment_type::random)!= "wormhole");

  assert(1 == 2); //!OCLINT This should cause a crash. Issue #55
}
