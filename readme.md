# xre-engine - Expression Evaluation Engine

This is a learning project for me to better understand programming
languages design.

```r
i = 0;

(i = i + 1) <= 50 loop {
    std_out ({1, 2, (3, i), 5} std_map {
		@ => std_buf @
	})
}
```

```r
'Separator' [local_0000] # as r- any
├── Alloc [var_0000] # as r- int64_t (0x0) token: '0'
├── Alloc [var_0001] # as rw int64_t (0x0) token: 'i'
├── Alloc [var_0002] # as r- int64_t (0x1) token: '1'
├── Alloc [var_0003] # as r- int64_t (0x32) token: '50'
├── 'Assign' [var_0000] # as r- int64_t
│   ├── *var_0000 # as r- int64_t
│   └── *var_0001 # as rw int64_t
└── 'Loop' [local_-2147483648] # as r- any
    ├── 'Less or equal' [local_0000] # as r- int64_t
    │   ├── 'Assign' [local_0000] # as r- int64_t
    │   │   ├── 'Addition' [local_0000] # as r- int64_t
    │   │   │   ├── *var_0001 # as rw int64_t
    │   │   │   └── *var_0002 # as r- int64_t
    │   │   └── *var_0001 # as rw int64_t
    │   └── *var_0003 # as r- int64_t
    └── 'Builtin call' `std_out` [local_0002] # as -- any
        ├── Alloc [var_0004] # as r- int64_t (0x5) token: '5'
        ├── Alloc [var_0005] # as r- int64_t (0x3) token: '3'
        ├── Alloc [var_0006] # as r- int64_t (0x2) token: '2'
        ├── Alloc [var_0007] # as -w any (0x0) token: '@'
        └── 'Builtin call' `std_map` [local_0002] # as rw vector(object)
            ├── 'Sequence' [local_0002] # as -w any
            │   ├── *var_0004 # as r- int64_t
            │   ├── 'Sequence' [local_0001] # as -w any
            │   │   ├── *var_0001 # as rw int64_t
            │   │   └── *var_0005 # as r- int64_t
            │   ├── *var_0006 # as r- int64_t
            │   └── *var_0002 # as r- int64_t
            └── 'Closure' [local_0003] # as rw vector(uint8_t)
                ├── *var_0007 # as -w any
                └── 'Builtin call' `std_buf` [local_0003] # as rw vector(uint8_t)
                    └── *var_0007 # as -w any
        Drop [var_0007]
        Drop [var_0006]
        Drop [var_0005]
        Drop [var_0004]
Drop [var_0003]
Drop [var_0002]
Drop [var_0001]
Drop [var_0000]
```

## Installation

install xre via Git using the install script:

```bash
$ git clone https://github.com/pauldcs/xre-engine
$ cd xre-engine
$ ./install.sh
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
