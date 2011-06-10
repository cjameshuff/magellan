# -*- ruby -*-

# http://tenderlovemaking.com/2009/12/18/writing-ruby-c-extensions-part-1/

require 'rubygems'
require 'hoe'
require "rake/extensiontask"

# Hoe.plugin :compiler
# Hoe.plugin :gem_prelude_sucks
# Hoe.plugin :inline
# Hoe.plugin :racc
# Hoe.plugin :rubyforge

Hoe.spec 'magellan' do
    developer('Christopher James Huff', 'cjameshuff@gmail.com')
    self.readme_file   = 'README.rdoc'
    self.history_file  = 'History.txt'
    self.extra_rdoc_files  = FileList['*.rdoc']
    self.extra_dev_deps << ['rake-compiler', '>= 0']
    self.spec_extras = { :extensions => ["ext/magellan/extconf.rb"] }

    Rake::ExtensionTask.new('magellan', spec) do |ext|
        ext.lib_dir = File.join('lib', 'magellan')
    end
    
  # self.rubyforge_name = 'magellanx' # if different than 'magellan'
end

Rake::Task[:gem].prerequisites << :compile
Rake::Task[:test].prerequisites << :compile

# vim: syntax=ruby
