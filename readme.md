# xre-engine - Expression Evaluation Engine

```C
// finding prime numbers in range 0, 20

i = 0;

{i = i + 1} <= 20 loop {
  f = 0;
  j = 1;

  j < 20 loop {
    !{i % j} do {
      f = f + 1
    };

    j = j + 1
  };

  f == 2 do
    print i
}

output:
2
3
5
7
11
13
17
19
```

## Installation

install xre is via Git using the install script:

```bash
$ git clone https://github.com/pauldcs/xre-engine
$ ./xre-engine/install.sh
 ...
$ xre -h
```

## Developement

The makefile in the root directory creates a docker
container that shares the sources (`engine`) between
your host machine and the container.
This makes it possible and easy to develop on any host while
always compiling on a standard linux distro.

The container is created by running:

```bash
$ make
# Ctrl-D to stop the container
```

Once the container is created, you get a bash shell to it by
opening a new terminal and running:

```bash
$ make shell
root @xre-dev $ cd engine
root @xre-dev $ make
root @xre-dev $ ./xre -v
...
```
