# xre-engine - Expression Evaluation Engine

This is a learning project for me to better understand interpreted programming
languages design

```r
i = 0;

(i = i + 1) <= 50 loop {
    out ({1, 2, (3, 4), 5} map {
		@ => buf @
	})
}
```

```r
$binop "Loop"   # 02:18:04:00
  # @var_i, raw: 0x0000000000000000, attrs: -W- undefined
 alloc +0000
  # @var_1, raw: 0x0000000000000001, attrs: R-- static_int
 alloc +0001
  # @var_50, raw: 0x0000000000000032, attrs: R-- static_int
 alloc +0002
    $binop "Less or equal"      # 02:12:02:00
        $binop "Assign"         # 02:03:01:01
            ref &0000 # attrs: ---
            $binop "Addition"   # 02:07:01:01
                ref &0000 # attrs: ---
                ref &0001 # attrs: ---
        ref &0002 # attrs: ---
    $uniop "Builtin call"       # 03:04:03:01
      # @var_5, raw: 0x0000000000000005, attrs: R-- static_int
     alloc +0003
      # @var_4, raw: 0x0000000000000004, attrs: R-- static_int
     alloc +0004
      # @var_3, raw: 0x0000000000000003, attrs: R-- static_int
     alloc +0005
      # @var_2, raw: 0x0000000000000002, attrs: R-- static_int
     alloc +0006
      # @var_@, raw: 0x0000000000000000, attrs: -W- undefined
     alloc +0007
        $binop "Builtin call"   # 03:27:03:02
            $binop "Sequence"   # 03:22:01:03
                ref &0003 # attrs: ---
                $binop "Sequence"       # 03:18:01:04
                    ref &0004 # attrs: ---
                    ref &0005 # attrs: ---
                ref &0006 # attrs: ---
                ref &0001 # attrs: ---
            $binop "Closure"    # 04:04:02:03
                ref &0007 # attrs: ---
                $uniop "Builtin call"   # 04:07:03:03
                    ref &0007 # attrs: ---
     drop +0007
     drop +0006
     drop +0005
     drop +0004
     drop +0003
 drop +0002
 drop +0001
 drop +0000

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
