
add_project_arguments('-ggdb', language : 'c')

wleafcron_srcs = files([
  'commonos.c',
  'crond.c',
  'imath.c',
  'main.c',
  'morestrings.c',
  'parser.c',
  'rules.c',
  'scanner.c',
  'symbols.c',
])

os = host_machine.system().to_lower()
if os == 'linux' or os.endswith('bsd') or os == 'cygwin'
  wleafcron_srcs += ['unix_osctrl.c']
elif os == 'windows'
  wleafcron_srcs += ['win_osctrl.c']
endif

wleafcron = executable(
  'wleafcron',
  wleafcron_srcs,
  win_subsystem : 'console',
)

# end of file
