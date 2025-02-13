project(
    'mapgen',
    'cpp',
    version: '0.1',
    default_options: ['warning_level=3', 'cpp_std=c++17'],
)
if meson.get_compiler('cpp').get_id() == 'clang'
    extra_args = [
        '-Wall',
        '-Wformat=2',
        '-Wshadow',
        '-Wfloat-equal',
        '-Wshift-overflow',
        '-Wextra',
        '-Wconversion',
        '-Wsign-conversion',
        '-Wcast-align',
        '-pedantic',
        '-gdwarf-4',
        '-O',
        '-fsanitize=integer,address,undefined,integer-divide-by-zero,shift,null,return,signed-integer-overflow,float-divide-by-zero,float-cast-overflow,bounds,alignment,vptr,leak',
        '-pedantic',
        '-Wno-sign-compare',
        '-Wno-unused-parameter',
        '-Wno-sign-conversion',
        '-ftrapv',
        '-D_GLIBCXX_DEBUG',
        '-D_GLIBCXX_DEBUG_PEDANTIC',
        '-D_FORTIFY_SOURCE=2',
        '-fstack-protector',
        '-Wno-unused-parameter',
        '-Wno-implicit-int-float-conversion'
    ]
else
    extra_args = []
endif

raylib = dependency('raylib')
fmt = dependency('fmt')
srcs = [
    'main.cpp',
    'noise.cpp',
    'chunk.cpp',
    'chunk_renderer.cpp',
    'engine.cpp'
]

executable(
    'mapgen',
    sources: srcs,
    dependencies: [raylib, fmt],
    cpp_args: extra_args,
)