[![Build Status][travis-shield]][travis-url]
[![Code Quality][lgtm-shield]][lgtm-url]

[travis-shield]: https://travis-ci.org/caian-org/tmul.svg?branch=master
[travis-url]: https://travis-ci.org/caian-org/tmul

[lgtm-shield]: https://img.shields.io/lgtm/grade/cpp/g/caian-org/tmul.svg?logo=lgtm&logoWidth=18
[lgtm-url]: https://lgtm.com/projects/g/caian-org/tmul/context:cpp

# TMUL: tiny, made up lisp


This is the source code repository of `tmul`, a tiny implementation of a
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
are used by Tmul. In Debian-based systems, the following command should install
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
$ docker build -t tmul .
$ docker run -it tmul
```


## Roadmap

*TODO*
