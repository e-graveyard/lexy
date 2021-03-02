[![Build Status][travis-shield]][travis-url]
[![Code Quality][lgtm-shield]][lgtm-url]

[travis-shield]: https://travis-ci.org/caian-org/lexy.svg?branch=master
[travis-url]: https://travis-ci.org/caian-org/lexy

[lgtm-shield]: https://img.shields.io/lgtm/grade/cpp/g/caian-org/lexy.svg?logo=lgtm&logoWidth=18
[lgtm-url]: https://lgtm.com/projects/g/caian-org/lexy/context:cpp

# LEXY: tiny, made up lisp

This is the source code repository of `lexy`, a tiny implementation of a
Lisp-like language. It contains the interpreter, the REPL, the standard library
as well the documentation.

The original implementation has been made upon the guidance of the [Build Your
Own Lisp](http://www.buildyourownlisp.com/) book.


## Table of Contents

- [Significant improvements](#significant-improvements)
- [Getting started](#getting-started)
    - [Compiling from source](#compiling-from-source)
    - [Installing & uninstalling](#installing--uninstalling)
    - [Running in Docker](#running-in-docker)
- [Roadmap](#roadmap)


## Significant improvements

1. code modularization
1. new math-related builtin functions such as `pow`, `sqrt` and `mod`
1. constant and dynamic variables (`letc` and `let` respectively)


## Getting started

### Build requirements

`gcc` and `make` are required to compile. Additionally, the `editline` headers
are used by lexy. In Debian-based systems, the following command should install
all the necessary packages:

```sh
$ sudo apt-get install -y build-essential libedit-dev
```

### Compiling from source

Just `make`. The binary will be available at the project's root directory.

### Installing & Uninstalling

To install, just use:

```sh
sudo make install
```

To uninstall:

```sh
sudo make uninstall
```

### Running in Docker

If you'd prefer to use Docker, you can build the image locally and run
interactively.

```sh
$ docker build -t lexy .
$ docker run -it lexy
```


## Roadmap

Expected improvements until the next release (`v0.1.0`):

1. [ ] Command-line arguments (help, eval string etc)
1. [ ] REPL improvements (colors, keywords)
1. [ ] Environment symbol lookup using hash tables
1. [ ] Hashmap built-in type
1. [ ] Hashmap operations
1. [ ] Tests
