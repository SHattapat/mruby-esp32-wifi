MRuby::Gem::Specification.new('esp32-wifi') do |spec|
  spec.license = 'MIT'
  spec.author  = 'Sialngern hattapat'
  spec.summary = 'wifi for ESP32'
  spec.cc.include_paths << "#{build.root}/src"
end
