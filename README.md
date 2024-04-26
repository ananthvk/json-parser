# jsonparser

This is an implementation of a JSON parser in C++. I built this as a learning project on parsing.

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

## Differences from JSON Spec

- Leading zeroes in numbers are allowed
- Line breaks can appear within strings (multiline strings)
- Control characters (i.e. tab character) can appear within strings
- No depth limit

## Notes

I got this idea from this [website](https://codingchallenges.substack.com/p/coding-challenge-2)
