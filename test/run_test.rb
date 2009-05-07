testdir = File.dirname(__FILE__)
require File.join(testdir, 'test_helper')

Dir.glob(File.join(testdir, 'test_*.rb')).each do |test|
  require test
end
