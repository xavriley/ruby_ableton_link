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
end
