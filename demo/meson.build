project('demo', ['c','cpp'],
  version: '0.2.0',
  default_options: [
    'cpp_std=c++23',
    'optimization=0',
  ],
)

linker_file = '-T' + meson.current_source_dir() + '/src/STM32F7/linker.ld'

source_files = files(
  'src/STM32F7/startup.s',
  'src/STM32F7/system.c',
  'src/STM32F7/syscalls.c'
)

executable('reference',
  sources: source_files + 'src/main_reference.cpp',
  include_directories: 'src',
  name_suffix: 'elf',
  link_args: [linker_file])

executable('demo',
  sources: source_files + 'src/main.cpp',
  include_directories: 'src',
  name_suffix: 'elf',
  link_args: [linker_file])

