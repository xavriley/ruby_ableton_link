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
end
