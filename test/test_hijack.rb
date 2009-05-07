require File.join(File.dirname(__FILE__), 'test_helper')

require 'hijack'

class TestHijack < Test::Unit::TestCase
  should "be able to hijack method definition" do
    f = hijack do
      def test_method
        return 1 + 2
      end
    end
    assert f.kind_of? Proc
    assert_equal 3, f.call
  end
end
