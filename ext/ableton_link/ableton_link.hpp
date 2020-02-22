#include "ruby/ruby.h"
#include "ruby/encoding.h"

extern "C"
void Init_ableton_link();
VALUE ableton_link_enable(VALUE self);
VALUE ableton_link_disable(VALUE self);
VALUE ableton_link_enabled(VALUE self);
VALUE ableton_link_start_stop_sync_enable(VALUE self);
VALUE ableton_link_start_stop_sync_disable(VALUE self);
VALUE ableton_link_start_stop_sync_enabled(VALUE self);
VALUE ableton_link_start_playing(VALUE self);
VALUE ableton_link_stop_playing(VALUE self);
VALUE ableton_link_is_playing(VALUE self);
VALUE ableton_link_tempo(VALUE self);
VALUE ableton_link_set_tempo(VALUE self, VALUE tempo);
VALUE ableton_link_quantum(VALUE self);
VALUE ableton_link_set_quantum(VALUE self, VALUE quantum);
VALUE ableton_link_time_until_downbeat(VALUE self);
VALUE ableton_link_time_until_beat_within_bar(VALUE self, VALUE rb_req_phase);
VALUE ableton_link_time_until_subdivision_within_beat(VALUE self, VALUE rb_req_beat);
VALUE ableton_link_request_beat_after(VALUE self, VALUE req_beat, VALUE offset);
VALUE ableton_link_force_beat_after(VALUE self, VALUE req_beat, VALUE offset);
VALUE ableton_link_status(VALUE self);

