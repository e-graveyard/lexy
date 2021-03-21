[![Build Status @ Travis-CI][travis-shield]][travis-url]
[![Build Status @ Azure Pipelines][azure-shield]][azure-url]
[![Code Quality][lgtm-shield]][lgtm-url]

[travis-shield]: https://img.shields.io/travis/caian-org/lexy?logo=travis-ci&logoColor=FFF&style=for-the-badge
[travis-url]: https://travis-ci.org/caian-org/lexy

[azure-shield]: https://img.shields.io/azure-devops/build/caian-org/97c8ebe2-b395-407e-8b0f-8db0a1771869/9/master?logo=azure-pipelines&style=for-the-badge
[azure-url]: https://dev.azure.com/caian-org/lexy

[lgtm-shield]: https://img.shields.io/lgtm/grade/cpp/github/caian-org/lexy?logo=lgtm&style=for-the-badge
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

1. [ ] Proper modules support
1. [ ] Hash table powered symbol lookup
1. [ ] Hashmap built-in type
1. [ ] Hashmap operations
1. [ ] Expand standard library
1. [x] Command-line arguments (help, eval string etc)
1. [ ] Receive arguments from the shell
1. [ ] REPL improvements (colors, keywords)
1. [ ] Tests
