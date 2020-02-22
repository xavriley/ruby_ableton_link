require "test_helper"

class AbletonLinkTest < Minitest::Test
  def setup
    @link = AbletonLink.new
  end

  def test_that_it_has_a_version_number
    refute_nil ::AbletonLink::VERSION
  end

  def test_it_enables_session
    @link.enable
    assert @link.enabled?
  end

  def test_it_disables_session
    @link.disable
    refute @link.enabled?
  end

  def test_it_enables_start_stop_sync
    @link.enable_start_stop_sync
    assert @link.start_stop_sync_enabled?
  end

  def test_it_disables_start_stop_sync
    @link.disable_start_stop_sync
    refute @link.start_stop_sync_enabled?
  end

  def test_it_starts_playing
    @link.enable
    @link.start_playing
    assert @link.is_playing?
  end

  def test_it_stops_playing
    @link.enable
    @link.stop_playing
    refute @link.is_playing?
  end

  def test_it_sets_tempo
    init_tempo = @link.tempo
    new_tempo = init_tempo + 10
    @link.set_tempo(new_tempo)
    refute @link.tempo == init_tempo
    assert @link.tempo == (init_tempo + 10)
  end

  def test_it_sets_quantum
    init_quantum = @link.quantum
    new_quantum = init_quantum + 1
    @link.set_quantum(new_quantum)
    refute @link.quantum == init_quantum
    assert @link.quantum == (init_quantum + 1)
    @link.set_quantum(init_quantum)
  end

  def test_time_until_downbeat
    @link.enable
    @link.set_tempo(120)
    @link.start_playing
    tud = @link.time_until_downbeat
    assert tud >= 0.0
    assert tud <= @link.quantum
  end

  def test_time_until_beat_within_bar
    @link.enable
    @link.set_tempo(60) # makes maths easier
    @link.force_beat_after!(3.0, 0.0) # set fourth beat to be now (zero indexed!)
    # beat not yet reached
    assert_in_delta @link.time_until_beat_within_bar(3.5).round(1), 0.5, 0.01
    # beat already passed - wait until the next bar
    assert_equal @link.time_until_beat_within_bar(2.0).ceil, 3
  end

  def test_time_until_subdivision_within_beat
    @link.enable
    @link.set_tempo(60) # makes maths easier
    @link.force_beat_after!(2.5, 0.0) # set offbeat of beat 3 to be now
    # beat not yet reached
    assert_in_delta @link.time_until_subdivision_within_beat(0.75).round(2), 0.25, 0.01
    # beat already passed - wait until the next bar
    assert_in_delta @link.time_until_subdivision_within_beat(0.25).round(2), 0.75, 0.01
  end

  def test_request_beat_after
    @link.enable
    @link.request_beat_after(0, 0.5) # set beat zero 0.5 secs into future
    assert @link.status[:beat] < 0.0 # we shouldn't have hit beat zero yet
  end

  def test_force_beat_after
    # this method is impolite and shouldn't be used really

    @link.enable
    @link.force_beat_after!(0, 0.0) # set beat zero to be now
    assert @link.status[:beat] >= 0.0 # we should be at beat zero or very close
    assert @link.status[:beat] <= 1.0 # we should be at beat zero or very close
  end
end
