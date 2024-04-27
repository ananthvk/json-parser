# jsonparser

This is an implementation of a JSON parser in C++. I built this as a learning project on parsing.

## Features
- Parses any valid JSON into a C++ tree
- Multiline strings are supported

## Differences from JSON Spec

- Leading zeroes in numbers are allowed
- Line breaks can appear within strings (multiline strings)
- Control characters (i.e. tab character) can appear within strings
- No depth limit

## TODO
- Implement unicode escapes
- Implement a parameter to limit depth
- Figure out a way for automatic type conversions
- Make it more efficient, for example by using move


## How to run

Firstly, install [meson](https://github.com/mesonbuild/meson) and [ninja](https://github.com/ninja-build/ninja)

Clone the repository

```
$ git clone https://github.com/ananthvk/json-parser
$ cd json-parser
```

Then build and run the tests

```
$ meson setup builddir
$ cd builddir
$ ninja -j8 test
```


## Notes

I got this idea from this [website](https://codingchallenges.substack.com/p/coding-challenge-2)
