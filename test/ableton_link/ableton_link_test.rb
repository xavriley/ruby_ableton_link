require "test_helper"

class AbletonLinkTest < Minitest::Test
  def setup
    @link = AbletonLink.new
  end

  def test_that_it_has_a_version_number
    refute_nil ::AbletonLink::VERSION
  end

  def test_it_says_hello
    assert @link.hello == "hello, world"
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
    @link.enableStartStopSync
    assert @link.startStopSyncEnabled?
  end

  def test_it_disables_start_stop_sync
    @link.disableStartStopSync
    refute @link.startStopSyncEnabled?
  end

  def test_it_starts_playing
    @link.enable
    @link.start_playing
    assert @link.isPlaying?
  end

  def test_it_stops_playing
    @link.enable
    @link.stop_playing
    refute @link.isPlaying?
  end

  def test_it_sets_tempo
    assert @link.tempo == 120.0

    init_tempo = @link.tempo
    new_tempo = init_tempo + 10
    @link.set_tempo(new_tempo)
    refute @link.tempo == init_tempo
    assert @link.tempo == (init_tempo + 10)
  end

  def test_time_until_downbeat
    @link.enable
    @link.set_tempo(120)
    @link.start_playing
    sleep 1
    tud = @link.time_until_downbeat
    puts tud
    assert tud >= 0.0
  end
end
