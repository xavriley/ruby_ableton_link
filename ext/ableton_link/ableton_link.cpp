#include "rice/Class.hpp"
#include "rice/String.hpp"
#include "ableton/Link.hpp"

using namespace Rice;

double beat{0.0};
double phase{0.0};
double initial_tempo{120.0};
double quantum{4.0};
ableton::Link linkInstance(initial_tempo);

// Draft API
//           // link = AbletonLink.new
//           // link.enable / enabled? / disable
//           // link.enableStartStopSync / startStopSyncEnabled?
//           // link.startPlaying / isPlaying? / stopPlaying
//
// link.numPeers / numPeersCallback
// link.setTempoCallback
// link.setStartStopCallback
//
//            // link.tempo / set_tempo
//            // link.quantum / set_quantum
//            // link.timeUntilDownbeat(offset = 0)
// link.timeUntilBeat(beat) e.g. next 0.25 of a beat or next 0.5
// link.timeUntilPhase(phase) e.g. any number less than (quantum - 1), e.g. time until 2.5th beat in phase
// link.timeUntilSubdivision(subdivision) e.g. next 0.25 of a beat or next 0.5
//
// link.requestDownbeatAt(offset)
// link.forceDownbeatAt(offset)

Object ableton_link_initialize(Object self)
{
  // self.iv_set("@tempo", tempo);
  return self;
}

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

extern "C"
void Init_ableton_link()
{
  Class rb_cAbletonLink =
    define_class("AbletonLink")
    .define_method("initialize", &ableton_link_initialize)
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
    .define_method("time_until_downbeat", &ableton_link_time_until_downbeat);
}
