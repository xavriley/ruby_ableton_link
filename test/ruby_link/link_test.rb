require "test_helper"

class RubyLinkTest < Minitest::Test
  def test_that_it_has_a_version_number
    refute_nil ::RubyLink::VERSION
  end

  def test_it_says_hello
    $stderr.puts (RubyLink.methods.sort - Object.methods).inspect
    #assert Ruby::Link.hello == "hello, world"
  end
end
