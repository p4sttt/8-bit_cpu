project('cpu', 'cpp', version: '0.0.1')

flags = ['-g', '-Wall', '-Wextra', '-Werror', '-std=c++17', '-pedantic', '-fsanitize=address']
src = [
    'src/main.cpp',
    'src/cpu.cpp',
    'src/debug.cpp',
    'src/instruction.cpp',
    'src/logger.cpp',
    'src/options.cpp',
]
incdir = include_directories('include')

executable('cpu', src, include_directories: incdir, link_args: flags)