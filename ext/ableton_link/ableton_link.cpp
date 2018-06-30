#include "rice/Class.hpp"
#include "rice/String.hpp"
#include "rice/Symbol.hpp"
#include "rice/Hash.hpp"
#include "ableton/Link.hpp"

using namespace Rice;

double initial_tempo{120.0};
double quantum{4.0};
ableton::Link linkInstance(initial_tempo);

// TODO
// link.numPeersCallback
// link.setTempoCallback
// link.setStartStopCallback

void ableton_link_enable()
{
  linkInstance.enable(true);
}

void ableton_link_disable()
{
  linkInstance.enable(false);
}

Object ableton_link_enabled()
{
  return to_ruby(linkInstance.isEnabled());
}

void ableton_link_start_stop_sync_enable()
{
  linkInstance.enableStartStopSync(true);
}

void ableton_link_start_stop_sync_disable()
{
  linkInstance.enableStartStopSync(false);
}

Object ableton_link_start_stop_sync_enabled()
{
  return to_ruby(linkInstance.isStartStopSyncEnabled());
}

void ableton_link_start_playing()
{
  ableton::Link::SessionState sessionState = linkInstance.captureAppSessionState();
  sessionState.setIsPlaying(true, linkInstance.clock().micros());
  linkInstance.commitAppSessionState(sessionState);
}

void ableton_link_stop_playing()
{
  ableton::Link::SessionState sessionState = linkInstance.captureAppSessionState();
  sessionState.setIsPlaying(false, linkInstance.clock().micros());
  linkInstance.commitAppSessionState(sessionState);
}

Object ableton_link_is_playing()
{
  ableton::Link::SessionState sessionState = linkInstance.captureAppSessionState();
  return to_ruby(sessionState.isPlaying());
}

Object ableton_link_tempo()
{
  ableton::Link::SessionState sessionState = linkInstance.captureAppSessionState();
  return to_ruby(sessionState.tempo());
}

void ableton_link_set_tempo(double tempo)
{
  ableton::Link::SessionState sessionState = linkInstance.captureAppSessionState();
  sessionState.setTempo(tempo, linkInstance.clock().micros());
  linkInstance.commitAppSessionState(sessionState);
}

Object ableton_link_quantum()
{
  return to_ruby(quantum);
}

void ableton_link_set_quantum(double new_quantum)
{
  quantum = new_quantum;
}

Object ableton_link_time_until_downbeat()
{
  ableton::Link::SessionState sessionState = linkInstance.captureAppSessionState();
  double beatsLeftInBar = quantum - sessionState.phaseAtTime(linkInstance.clock().micros(), quantum);
  double currentBeat = sessionState.beatAtTime(linkInstance.clock().micros(), quantum);

  std::chrono::duration<float> time_until_downbeat = std::chrono::duration<float>(sessionState.timeAtBeat(currentBeat + beatsLeftInBar, quantum) - linkInstance.clock().micros());

  return to_ruby(time_until_downbeat.count());
}

Object ableton_link_time_until_beat_within_bar(double req_phase)
{
  ableton::Link::SessionState sessionState = linkInstance.captureAppSessionState();

  double wait;
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

  std::chrono::duration<float> time_until_beat =
    std::chrono::duration<float>(sessionState.timeAtBeat(current_beat + wait, quantum) - linkInstance.clock().micros());

  return to_ruby(time_until_beat.count());
}

Object ableton_link_time_until_subdivision_within_beat(double req_beat)
{
  ableton::Link::SessionState sessionState = linkInstance.captureAppSessionState();

  double target_beat;
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

  std::chrono::duration<float> time_until_subdivision =
    std::chrono::duration<float>(sessionState.timeAtBeat(target_beat, quantum) - linkInstance.clock().micros());

  return to_ruby(time_until_subdivision.count());
}

Object ableton_link_status()
{
  ableton::Link::SessionState sessionState = linkInstance.captureAppSessionState();

  Hash output;
  output[Symbol("beat")] = to_ruby(sessionState.beatAtTime(linkInstance.clock().micros(), quantum));
  output[Symbol("phase")] = to_ruby(sessionState.phaseAtTime(linkInstance.clock().micros(), quantum));
  output[Symbol("playing?")] = to_ruby(sessionState.isPlaying());
  output[Symbol("tempo")] = to_ruby(sessionState.tempo());
  output[Symbol("peers")] = to_ruby(linkInstance.numPeers());

  return output;
}

void ableton_link_request_beat_after(double req_beat, double offset)
{
  ableton::Link::SessionState sessionState = linkInstance.captureAppSessionState();
  std::chrono::microseconds req_time = linkInstance.clock().micros() + std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::duration<double>(offset));
  sessionState.requestBeatAtTime(req_beat, req_time, quantum);
  linkInstance.commitAppSessionState(sessionState);
}

void ableton_link_force_beat_after(double req_beat, double offset)
{
  ableton::Link::SessionState sessionState = linkInstance.captureAppSessionState();
  std::chrono::microseconds req_time = linkInstance.clock().micros() + std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::duration<double>(offset));
  sessionState.forceBeatAtTime(req_beat, req_time, quantum);
  linkInstance.commitAppSessionState(sessionState);
}

/* void ableton_link_num_peers_callback(Object block) */
/* { */
/*   linkInstance.setNumPeersCallback(protect(rb_yield, block)); */
/* } */

extern "C"
void Init_ableton_link()
{
  Class rb_cAbletonLink =
    define_class("AbletonLink")
    .define_method("enable", &ableton_link_enable)
    .define_method("disable", &ableton_link_disable)
    .define_method("enabled?", &ableton_link_enabled)
    .define_method("enable_start_stop_sync", &ableton_link_start_stop_sync_enable)
    .define_method("disable_start_stop_sync", &ableton_link_start_stop_sync_disable)
    .define_method("start_stop_sync_enabled?", &ableton_link_start_stop_sync_enabled)
    .define_method("start_playing", &ableton_link_start_playing)
    .define_method("stop_playing", &ableton_link_stop_playing)
    .define_method("is_playing?", &ableton_link_is_playing)
    .define_method("tempo", &ableton_link_tempo)
    .define_method("set_tempo", &ableton_link_set_tempo)
    .define_method("quantum", &ableton_link_quantum)
    .define_method("set_quantum", &ableton_link_set_quantum)
    .define_method("status", &ableton_link_status)
    // .define_method("num_peers_callback", &ableton_link_num_peers_callback)
    .define_method("request_beat_after", &ableton_link_request_beat_after)
    .define_method("force_beat_after!", &ableton_link_force_beat_after)
    .define_method("time_until_downbeat", &ableton_link_time_until_downbeat)
    .define_method("time_until_beat_within_bar", &ableton_link_time_until_beat_within_bar)
    .define_method("time_until_subdivision_within_beat", &ableton_link_time_until_subdivision_within_beat);
}
