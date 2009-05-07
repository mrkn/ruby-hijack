require 'fileutils'
require 'test/unit'

require 'rubygems'
require 'redgreen' rescue true
require 'shoulda'

basedir = File.dirname(File.dirname(__FILE__))
$:.unshift File.join(basedir, 'ext')
$:.unshift File.join(basedir, 'lib')
$:.unshift File.join(basedir, 'test')
