# xre-engine - Expression Evaluation Engine

This is a learning project for me to better understand interpreted programming
languages design

```r
ii = 0;

(i = i + 1) <= 50 loop {
    std_out ({1, 2, (3, 4), 5} std_map {
		@ => std_buf @
	})
}
```

```r
"Separator" #01:06:01:00
├── # VM space: 10 object(s) 01:06:01:00
├── ALLOCATE [var_0000] as private mutable any (0x0) #ii
├── ALLOCATE [var_0001] as public constant int64_t (0x0) #0
├── ALLOCATE [var_0002] as private mutable any (0x0) #i
├── ALLOCATE [var_0003] as public constant int64_t (0x1) #1
├── ALLOCATE [var_0004] as public constant int64_t (0x32) #50
├── "Assign" #01:03:01:00
│   ├── reference var_0000 as public mutable int64_t
│   └── reference var_0001 as public constant int64_t
│   # Return offset [var_0001]
│   # as public mutable int64_t
└── "Loop" #03:18:04:00
    ├── "Less or equal" #03:12:02:00
    │   ├── "Assign" #03:03:01:01
    │   │   ├── reference var_0002 as private mutable any
    │   │   └── "Addition" #03:07:01:01
    │   │       ├── reference var_0002 as private mutable any
    │   │       └── reference var_0003 as public constant int64_t
    │   │       # as public constant int64_t
    │   │   # as public mutable int64_t
    │   └── reference var_0004 as public constant int64_t
    │   # as public constant int64_t
    └── "Builtin call" `std_out` #04:04:07:01
        ├── # VM space: 5 object(s) 04:04:07:01
        ├── ALLOCATE [var_0005] as public constant int64_t (0x5) #5
        ├── ALLOCATE [var_0006] as public constant int64_t (0x4) #4
        ├── ALLOCATE [var_0007] as public constant int64_t (0x3) #3
        ├── ALLOCATE [var_0008] as public constant int64_t (0x2) #2
        ├── ALLOCATE [var_0009] as private mutable any (0x0) #@
        └── "Builtin call" `std_map` #04:31:07:02
            ├── "Sequence" #04:26:01:03
            │   ├── reference var_0005 as public constant int64_t
            │   ├── "Sequence" #04:22:01:04
            │   │   ├── reference var_0006 as public constant int64_t
            │   │   └── reference var_0007 as public constant int64_t
            │   ├── reference var_0008 as public constant int64_t
            │   └── reference var_0003 as public constant int64_t
            │       # as public constant any
            └── "Closure" #05:04:02:03
                ├── reference var_0009 as private mutable any
                └── "Builtin call" `std_buf` #05:07:07:03
                    └── reference var_0009 as private mutable any
                    # as public mutable vector(uint8_t)
                # as public mutable vector(uint8_t)
            # as public mutable vector(object)
        DROP [var_0009]
        DROP [var_0008]
        DROP [var_0007]
        DROP [var_0006]
        DROP [var_0005]
    # as public constant any
# as public constant any
DROP [var_0004]
DROP [var_0003]
DROP [var_0002]
DROP [var_0001]
DROP [var_0000]

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
