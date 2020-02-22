#include "ableton_link.hpp"
#include "oscpkt.hh"
#include "ableton/Link.hpp"

VALUE AbletonLink = Qnil;
VALUE rb_cAbletonLink = Qnil;

double initial_tempo{120.0};
double quantum{4.0};
ableton::Link linkInstance(initial_tempo);

// For outbound OSC comms
// Adapted from https://stackoverflow.com/a/32274589/2618015
asio::io_service io_service;
asio::ip::udp::socket sock(io_service);
asio::ip::udp::endpoint remote_endpoint;

// TODO
// detect initial tempo change
//
// link.numPeersCallback

void set_tempo_callback(double bpm)
{
  // Asynchronous callbacks are hard in Ruby extensions
  // Ideally this would be handled in a Ruby proc
  //
  // As a workaround we send an OSC message on a port
  // with the values for the tempo change. These can
  // be used by Sonic Pi directly however it should also
  // be possible to open a socket and receive these messages
  // on other Ruby applications
  //
  oscpkt::PacketWriter pkt;
  oscpkt::Message message("/link/tempo");
  message.pushDouble(bpm);
  pkt.addMessage(message);

  asio::error_code err;
  sock.send_to(asio::buffer(pkt.packetData(), pkt.packetSize()), remote_endpoint, 0, err);
}

void set_start_stop_callback(bool isPlaying)
{
  // see notes on callback above
  oscpkt::PacketWriter pkt;
  oscpkt::Message message("/link/start_stop");
  message.pushInt32(isPlaying);
  pkt.addMessage(message);

  asio::error_code err;
  sock.send_to(asio::buffer(pkt.packetData(), pkt.packetSize()), remote_endpoint, 0, err);
}

void set_num_peers_callback(std::size_t numPeers)
{
  // OSC needs a uint32
  int peerCountAsInt;
  if(numPeers > std::numeric_limits<int>::max()) {
    // stop counting if we hit the max - v. unlikely!
    peerCountAsInt = std::numeric_limits<int>::max();
  }
  else
  {
    peerCountAsInt = static_cast<int>(numPeers);
  }

  // see notes on callback above
  oscpkt::PacketWriter pkt;
  oscpkt::Message message("/link/num_peers");
  message.pushInt32(peerCountAsInt);
  pkt.addMessage(message);

  asio::error_code err;
  sock.send_to(asio::buffer(pkt.packetData(), pkt.packetSize()), remote_endpoint, 0, err);
}

VALUE ableton_link_enable(VALUE self)
{
  sock.close();
  sock.open(asio::ip::udp::v4());
  // TODO: make host and port configurable
  remote_endpoint = asio::ip::udp::endpoint(asio::ip::address::from_string("127.0.0.1"), 4559);

  linkInstance.setTempoCallback(set_tempo_callback);
  linkInstance.setStartStopCallback(set_start_stop_callback);
  linkInstance.setNumPeersCallback(set_num_peers_callback);
  linkInstance.enable(true);
  return Qtrue;
}

VALUE ableton_link_disable(VALUE self)
{
  sock.close();
  linkInstance.enable(false);
  return Qtrue;
}

VALUE ableton_link_enabled(VALUE self)
{
  return (linkInstance.isEnabled() ? Qtrue : Qfalse);
}

VALUE ableton_link_start_stop_sync_enable(VALUE self)
{
  linkInstance.enableStartStopSync(true);
  return Qtrue;
}

VALUE ableton_link_start_stop_sync_disable(VALUE self)
{
  linkInstance.enableStartStopSync(false);
  return Qtrue;
}

VALUE ableton_link_start_stop_sync_enabled(VALUE self)
{
  return (linkInstance.isStartStopSyncEnabled() ? Qtrue : Qfalse);
}

VALUE ableton_link_start_playing(VALUE self)
{
  ableton::Link::SessionState sessionState = linkInstance.captureAppSessionState();
  sessionState.setIsPlaying(true, linkInstance.clock().micros());
  linkInstance.commitAppSessionState(sessionState);
  return Qtrue;
}

VALUE ableton_link_stop_playing(VALUE self)
{
  ableton::Link::SessionState sessionState = linkInstance.captureAppSessionState();
  sessionState.setIsPlaying(false, linkInstance.clock().micros());
  linkInstance.commitAppSessionState(sessionState);
  return Qtrue;
}

VALUE ableton_link_is_playing(VALUE self)
{
  ableton::Link::SessionState sessionState = linkInstance.captureAppSessionState();
  return (sessionState.isPlaying() ? Qtrue : Qfalse);
}

VALUE ableton_link_tempo(VALUE self)
{
  ableton::Link::SessionState sessionState = linkInstance.captureAppSessionState();
  return DBL2NUM(sessionState.tempo());
}

VALUE ableton_link_set_tempo(VALUE self, VALUE tempo)
{
  ableton::Link::SessionState sessionState = linkInstance.captureAppSessionState();
  sessionState.setTempo(NUM2DBL(tempo), linkInstance.clock().micros());
  linkInstance.commitAppSessionState(sessionState);
  return tempo;
}

VALUE ableton_link_quantum(VALUE self)
{
  return DBL2NUM(quantum);
}

VALUE ableton_link_set_quantum(VALUE self, VALUE new_quantum)
{
  quantum = NUM2DBL(new_quantum);
  return DBL2NUM(quantum);
}

VALUE ableton_link_time_until_downbeat(VALUE self)
{
  ableton::Link::SessionState sessionState = linkInstance.captureAppSessionState();
  double beatsLeftInBar = quantum - sessionState.phaseAtTime(linkInstance.clock().micros(), quantum);
  double currentBeat = sessionState.beatAtTime(linkInstance.clock().micros(), quantum);

  std::chrono::duration<double> time_until_downbeat = std::chrono::duration<double>(sessionState.timeAtBeat(currentBeat + beatsLeftInBar, quantum) - linkInstance.clock().micros());

  return DBL2NUM(time_until_downbeat.count());
}

VALUE ableton_link_time_until_beat_within_bar(VALUE self, VALUE rb_req_phase)
{
  ableton::Link::SessionState sessionState = linkInstance.captureAppSessionState();

  double wait;
  double req_phase = NUM2DBL(rb_req_phase);
  double phase_now = sessionState.phaseAtTime(linkInstance.clock().micros(), quantum);
  double current_beat = sessionState.beatAtTime(linkInstance.clock().micros(), quantum);

  if(req_phase > phase_now) {
    // we haven't reached the target beat in the bar yet
    wait = req_phase - phase_now;
  } else {
    // we already passed the target beat
    // wait till the start of the next downbeat and then wait for target beat
    wait = (quantum - phase_now) + req_phase;
  }

  std::chrono::duration<double> time_until_beat =
    std::chrono::duration<double>(sessionState.timeAtBeat(current_beat + wait, quantum) - linkInstance.clock().micros());

  return DBL2NUM(time_until_beat.count());
}

VALUE ableton_link_time_until_subdivision_within_beat(VALUE self, VALUE rb_req_beat)
{
  ableton::Link::SessionState sessionState = linkInstance.captureAppSessionState();

  double target_beat;
  double req_beat = NUM2DBL(rb_req_beat);
  double current_beat = sessionState.beatAtTime(linkInstance.clock().micros(), quantum);

  if(req_beat > fmod(current_beat, 1.0)) {
    // we haven't reached the target beat in the bar yet
    target_beat = floor(current_beat) +req_beat;
  } else {
    // we already passed the target
    // wait till the start of the next beat and then wait for target beat
    // note the floor
    target_beat = floor(current_beat) + 1.0 + req_beat;
  }

  std::chrono::duration<double> time_until_subdivision =
    std::chrono::duration<double>(sessionState.timeAtBeat(target_beat, quantum) - linkInstance.clock().micros());

  return DBL2NUM(time_until_subdivision.count());
}

VALUE ableton_link_status(VALUE self)
{
  ableton::Link::SessionState sessionState = linkInstance.captureAppSessionState();

  VALUE output = rb_hash_new();
  rb_hash_aset(output, ID2SYM(rb_intern("beat")), DBL2NUM(sessionState.beatAtTime(linkInstance.clock().micros(), quantum)));
  rb_hash_aset(output, ID2SYM(rb_intern("phase")), DBL2NUM(sessionState.phaseAtTime(linkInstance.clock().micros(), quantum)));
  rb_hash_aset(output, ID2SYM(rb_intern("playing?")), (sessionState.isPlaying() ? Qtrue : Qfalse));
  rb_hash_aset(output, ID2SYM(rb_intern("tempo")), DBL2NUM(sessionState.tempo()));
  rb_hash_aset(output, ID2SYM(rb_intern("peers")), INT2NUM(linkInstance.numPeers()));

  // cast to double
  std::chrono::duration<double> time_now = std::chrono::duration<double>(linkInstance.clock().micros());
  std::chrono::duration<double> beat_zero = std::chrono::duration<double>(sessionState.timeAtBeat(0.0, quantum));

  rb_hash_aset(output, ID2SYM(rb_intern("now")), DBL2NUM(time_now.count()));
  rb_hash_aset(output, ID2SYM(rb_intern("beat_zero")), DBL2NUM(beat_zero.count()));

  return output;
}

VALUE ableton_link_request_beat_after(VALUE self, VALUE req_beat, VALUE offset)
{
  ableton::Link::SessionState sessionState = linkInstance.captureAppSessionState();
  std::chrono::microseconds req_time = linkInstance.clock().micros() + std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::duration<double>(NUM2DBL(offset)));
  sessionState.requestBeatAtTime(NUM2DBL(req_beat), req_time, quantum);
  linkInstance.commitAppSessionState(sessionState);
  return Qtrue;
}

VALUE ableton_link_force_beat_after(VALUE self, VALUE req_beat, VALUE offset)
{
  ableton::Link::SessionState sessionState = linkInstance.captureAppSessionState();
  std::chrono::microseconds req_time = linkInstance.clock().micros() + std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::duration<double>(NUM2DBL(offset)));
  sessionState.forceBeatAtTime(NUM2DBL(req_beat), req_time, quantum);
  linkInstance.commitAppSessionState(sessionState);
  return Qtrue;
}

void Init_ableton_link()
{
	rb_cAbletonLink = rb_define_class("AbletonLink", rb_cObject);
	// https://stackoverflow.com/a/19249603
	rb_define_method(rb_cAbletonLink, "enable", reinterpret_cast<VALUE(*)(...)>(ableton_link_enable), 0);
	rb_define_method(rb_cAbletonLink, "disable", reinterpret_cast<VALUE(*)(...)>(ableton_link_disable), 0);
	rb_define_method(rb_cAbletonLink, "enabled?", reinterpret_cast<VALUE(*)(...)>(ableton_link_enabled), 0);
	rb_define_method(rb_cAbletonLink, "enable_start_stop_sync", reinterpret_cast<VALUE(*)(...)>(ableton_link_start_stop_sync_enable), 0);
	rb_define_method(rb_cAbletonLink, "disable_start_stop_sync", reinterpret_cast<VALUE(*)(...)>(ableton_link_start_stop_sync_disable), 0);
	rb_define_method(rb_cAbletonLink, "start_stop_sync_enabled?", reinterpret_cast<VALUE(*)(...)>(ableton_link_start_stop_sync_enabled), 0);
	rb_define_method(rb_cAbletonLink, "start_playing", reinterpret_cast<VALUE(*)(...)>(ableton_link_start_playing), 0);
	rb_define_method(rb_cAbletonLink, "stop_playing", reinterpret_cast<VALUE(*)(...)>(ableton_link_stop_playing), 0);
	rb_define_method(rb_cAbletonLink, "is_playing?", reinterpret_cast<VALUE(*)(...)>(ableton_link_is_playing), 0);
	rb_define_method(rb_cAbletonLink, "tempo", reinterpret_cast<VALUE(*)(...)>(ableton_link_tempo), 0);
	rb_define_method(rb_cAbletonLink, "set_tempo", reinterpret_cast<VALUE(*)(...)>(ableton_link_set_tempo), 1);
	rb_define_method(rb_cAbletonLink, "quantum", reinterpret_cast<VALUE(*)(...)>(ableton_link_quantum), 0);
	rb_define_method(rb_cAbletonLink, "set_quantum", reinterpret_cast<VALUE(*)(...)>(ableton_link_set_quantum), 1);
	rb_define_method(rb_cAbletonLink, "status", reinterpret_cast<VALUE(*)(...)>(ableton_link_status), 0);
	/* // .define_method(rb_cAbletonLink, "num_peers_callback", ableton_link_num_peers_callback, argc); */
	rb_define_method(rb_cAbletonLink, "request_beat_after", reinterpret_cast<VALUE(*)(...)>(ableton_link_request_beat_after), 2);
	rb_define_method(rb_cAbletonLink, "force_beat_after!", reinterpret_cast<VALUE(*)(...)>(ableton_link_force_beat_after), 2);
	rb_define_method(rb_cAbletonLink, "time_until_downbeat", reinterpret_cast<VALUE(*)(...)>(ableton_link_time_until_downbeat), 0);
	rb_define_method(rb_cAbletonLink, "time_until_beat_within_bar", reinterpret_cast<VALUE(*)(...)>(ableton_link_time_until_beat_within_bar), 1);
	rb_define_method(rb_cAbletonLink, "time_until_subdivision_within_beat", reinterpret_cast<VALUE(*)(...)>(ableton_link_time_until_subdivision_within_beat), 1);
}

