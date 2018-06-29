#include "rice/Class.hpp"
#include "rice/String.hpp"
#include "ableton/Link.hpp"

using namespace Rice;

double beat{0.0};
double phase{0.0};
double bpm{120.0};
double quantum{4.0};

Object ruby_link_hello(Object /* self */)
{
  String str("hello, world");
  return str;
}

Object ruby_link_get_bpm(Object /* self */)
{
  return to_ruby(bpm);
}

Object ruby_link_time_now(Object /* self */)
{
  ableton::Link linkInstance(120.);
  const std::chrono::microseconds time = linkInstance.clock().micros();
  // this doesn't work currently
  // would need to split the long long into time and then fraction
  // and use Time.at(time, frac) in a custom
  return to_ruby(time.count());
}

extern "C"
void Init_ruby_link()
{
  Module rb_mRubyLink =
    define_module("RubyLink")
    .define_module_function("hello", &ruby_link_hello)
    .define_module_function("get_bpm", &ruby_link_get_bpm)
    .define_module_function("time_now", &ruby_link_time_now);
}
