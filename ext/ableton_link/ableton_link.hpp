#include "ruby/ruby.h"
#include "ruby/encoding.h"

extern "C"
void Init_ableton_link();
VALUE rb_ableton_link_enable(VALUE self);

