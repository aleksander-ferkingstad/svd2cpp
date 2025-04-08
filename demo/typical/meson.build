project('demo', ['cpp'],
  version: '1.0',
  default_options: [
    'cpp_std=c++20'
  ])

# Compiler flags
args = [
  '-mcpu=cortex-m7',
  '-mthumb',
  '-mfpu=fpv5-d16',
  '-mfloat-abi=hard',
  '-ffunction-sections',
  '-fdata-sections',
  '-fno-exceptions',
  '-fno-rtti',
  '-fno-use-cxa-atexit',
  '-Wno-volatile',
  '-specs=nosys.specs',
]

link_args = [
  '-nostartfiles',
  '-Wl,--gc-sections',
  '-Wl,-Map=app.map',
  '-specs=nosys.specs',
]

linker_file = '-T' + meson.current_source_dir() + '/linker/stm32f767zi.ld'

source_files = files(
    'src/main.cpp',
    'src/startup.cpp'
)
executable('demo',
  sources: source_files,
  include_directories: 'src',
  name_suffix: 'elf',
  link_args: link_args + [linker_file],
  cpp_args: args)