
project(
  'wleafcron',
  'c',
  version : '0.0.3',
  license : 'MIT',
  default_options : [
    'c_std=c99',
    'warning_level=3',
    #'b_sanitize=address,undefined',
  ],
  meson_version : '>= 0.56',
)

subdir('resources')
subdir('src')

if get_option('tests')
  subdir('tests')
endif

# end of file
