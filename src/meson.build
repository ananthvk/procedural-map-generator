srcs = [
    'main.cpp',
    'noise.cpp',
    'chunk.cpp',
    'chunk_renderer.cpp',
    'engine.cpp',
    'registries.cpp',
    'csscolorparser.cpp'
]

executable(
    'mapgen',
    sources: srcs,
    dependencies: [raylib, fmt],
    cpp_args: extra_args,
    include_directories: include_dirs
)