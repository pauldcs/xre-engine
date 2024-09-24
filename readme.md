# xre-engine - Expression Evaluation Engine

This is a learning project for me to better understand interpreted programming
languages design

```r
i = 0;

(i = i + 1) <= 50 loop {
    print ({1, 2, (3, 4), 5} map {
		@ => buf @
	})
}
```

```r
$binop "Separator" 	# 01:05:01:00
 reserve +0000 	# __local: 'i'
 reserve +0001 	# __static: 0
 reserve +0002 	# __static: 1
 reserve +0003 	# __static: 50
    $binop "Assign" 	# 01:02:01:00
        ref &0000
        ref &0001
    $binop "Loop" 	# 03:18:04:00
        $binop "Less or equal" 	# 03:12:02:00
            $binop "Assign" 	# 03:03:01:01
                ref &0000
                $binop "Addition" 	# 03:07:01:01
                    ref &0000
                    ref &0002
            ref &0003
        $uniop "Builtin call" 	# 04:04:05:01
         reserve +0004 	# __static: 5
         reserve +0005 	# __static: 4
         reserve +0006 	# __static: 3
         reserve +0007 	# __static: 2
         reserve +0008 	# __local: '@'
            $binop "Builtin call" 	# 04:29:03:02
                $binop "Sequence" 	# 04:24:01:03
                    ref &0004
                    $binop "Sequence" 	# 04:20:01:04
                        ref &0005
                        ref &0006
                    ref &0007
                    ref &0006
                $binop "Closure" 	# 05:04:02:03
                    ref &0008
                    $uniop "Builtin call" 	# 05:07:03:03
                        ref &0017
         drop +0008
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
