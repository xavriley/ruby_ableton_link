# AbletonLink
# WIP: please don't use this yet

## Todo

-[ ] include Ableton Link as submodule
-[ ] allow for compilation on platforms other than Mac
-[ ] work out something for the tempo, peers and startStop callbacks

A Ruby wrapper for the Ableton Link C++ library.

## What is Ableton Link?

Ableton Link is a new technology that synchronises beat, phase and tempo of Ableton Live and Link-enabled applications over a wireless or wired network. It lets you play devices together with the freedom of a live band. Anyone can start and stop their part while others keep playing, and anyone can adjust the tempo and the rest will follow. You can use Ableton Link with an increasing number of Link enabled applications – even without Live in your setup.

## Installation

Add this line to your application's Gemfile:

```ruby
gem 'ruby_ableton_link'
```

And then execute:

    $ bundle

Or install it yourself as:

    $ gem install ruby_ableton_link

## Usage

```
link = AbletonLink.new
puts link.status
```

See the tests for the full API

## Development

After checking out the repo, run `bin/setup` to install dependencies. Then, run `rake test` to run the tests. You can also run `bin/console` for an interactive prompt that will allow you to experiment.

To install this gem onto your local machine, run `bundle exec rake install`. To release a new version, update the version number in `version.rb`, and then run `bundle exec rake release`, which will create a git tag for the version, push git commits and tags, and push the `.gem` file to [rubygems.org](https://rubygems.org).

## Contributing

Bug reports and pull requests are welcome on GitHub at https://github.com/xavriley/ruby_ableton_link. This project is intended to be a safe, welcoming space for collaboration, and contributors are expected to adhere to the [Contributor Covenant](http://contributor-covenant.org) code of conduct.

## License

The gem is available as open source under the terms of the [MIT License](https://opensource.org/licenses/MIT).

## Code of Conduct

Everyone interacting in the Ruby::Link project’s codebases, issue trackers, chat rooms and mailing lists is expected to follow the [code of conduct](https://github.com/xavriley/ruby_link/blob/master/CODE_OF_CONDUCT.md).
