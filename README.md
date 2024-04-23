# jsonparser

This is an implementation of a JSON parser in C++. I built this as a learning experience on parsing.

## How to run

Firstly, install [meson](https://github.com/mesonbuild/meson) and [ninja](https://github.com/ninja-build/ninja)

Clone the repository

```
$ git clone https://github.com/ananthvk/jsonparser
$ cd jsonparser
```

Then build and run the tests

```
$ meson setup builddir
$ cd builddir
$ ninja -j8 test
```

## Notes

I got this idea from this [website](https://codingchallenges.substack.com/p/coding-challenge-2)
