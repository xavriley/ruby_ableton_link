#include "rice/Class.hpp"
#include "rice/String.hpp"
#include "ableton/Link.hpp"

using namespace Rice;

double beat{0.0};
double phase{0.0};
double bpm{120.0};
double quantum{4.0};

Object ableton_link_hello(Object /* self */)
{
  String str("hello, world");
  return str;
}

Object ableton_link_get_bpm(Object /* self */)
{
  return to_ruby(bpm);
}

Object ableton_link_time_now(Object /* self */)
{
  ableton::Link linkInstance(120.);
  const std::chrono::microseconds time = linkInstance.clock().micros();
  // this doesn't work currently
  // would need to split the long long into time and then fraction
  // and use Time.at(time, frac) in a custom
  return to_ruby(time.count());
}

extern "C"
void Init_ableton_link()
{
  Class rb_cAbletonLink =
    define_class("AbletonLink")
    .define_method("hello", &ableton_link_hello)
    .define_method("get_bpm", &ableton_link_get_bpm)
    .define_method("time_now", &ableton_link_time_now);
}
