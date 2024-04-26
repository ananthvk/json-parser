project(
    'jsonparser',
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
    ]
else
    extra_args = []
endif

gtest_dep = dependency('gtest')


# To build the parser library
sources = [
    'src/json_exceptions.cpp',
    'src/json_lexer.cpp',
    'src/json_parser.cpp',
    'src/json_object.cpp',
    'src/token.cpp',
]

include_dirs = ['include', 'src']

lib = library(
    'jsonparser',
    sources,
    include_directories: include_dirs,
    cpp_args: extra_args,
)

tests = ['test_json_lexer', 'test_json_parser']

foreach s : tests
    e = executable(
        s,
        sources: ['tests/' + s + '.cpp'],
        dependencies: [gtest_dep],
        include_directories: include_dirs,
        cpp_args: extra_args,
        link_with: lib,
        cpp_args: extra_args,
    )
    test(s, e)
endforeach
