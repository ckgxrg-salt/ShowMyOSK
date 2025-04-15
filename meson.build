project(
  'show-my-osk',
  'cpp',
  version: '0.1',
  license: 'BSD-2-Clause'
)

cpp_compiler = meson.get_compiler('cpp')
if cpp_compiler.has_argument('-std=c++23')
  add_global_arguments('-std=c++23', language: 'cpp')
elif cpp_compiler.has_argument('-std=c++2b')
  add_global_arguments('-std=c++2b', language: 'cpp')
else
  error('Could not configure current C++ compiler (' + cpp_compiler.get_id() + ' ' + cpp_compiler.version() + ') with required C++ standard (C++23)')
endif

hyprland = dependency(
  'hyprland',
  required: true,
)
hyprland_deps = [
  dependency('aquamarine'),
  dependency('hyprlang'),
  dependency('hyprutils'),
  dependency('hyprgraphics'),
  dependency('libdrm'),
  dependency('libinput'),
  dependency('libudev'),
  dependency('wayland-server'),
  dependency('egl'),
  dependency('pixman-1'),
  dependency('xkbcommon'),
]

subdir('src')
