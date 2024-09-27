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
$binop "Separator" `15:00:01:00` +0008 # r- int
 (+) alloc +0000 # @var_i (0x0) rw int
 (+) alloc +0001 # @var_0 (0x0) r- int
 (+) alloc +0002 # @var_1 (0x1) r- int
 (+) alloc +0003 # @var_50 (0x32) r- int
 (+) alloc +0004 # @var_salut (0x0) rw int
 (+) alloc +0005 # @var_bite (0x0) rw int
 (+) alloc +0006 # @var_2 (0x2) r- int
 (+) alloc +0007 # @var_pute (0x0) rw int
 (+) alloc +0008 # @var_3 (0x3) r- int
    $binop "Separator" `14:00:01:00` +0006 # r- int
        $binop "Separator" `13:00:01:00` +0002 # r- int
            $binop "Separator" `03:00:01:00` # r- ???
                $binop "Assign" `02:02:01:00` +0001 # rw int
                    ref &0000 # rw int
                    ref &0001 # r- int
                $binop "Loop" `03:20:04:00` # r- ???
                    $binop "Less or equal" `03:14:02:00` # r- int
                        $binop "Assign" `03:05:01:01` # rw int
                            ref &0000 # rw int
                            $binop "Addition" `03:09:01:01` # r- int
                                ref &0000 # rw int
                                ref &0002 # r- int
                        ref &0003 # r- int
                    $binop "Separator" `06:04:01:01` # r- ???
                     (+) alloc +0009 # @var_b (0x0) rw string
                     (+) alloc +0010 # @var_'hello' (0x602000000190) r- string
                     (+) alloc +0011 # @var_j (0x0) -w ???
                     (+) alloc +0012 # @var_5 (0x5) r- int
                     (+) alloc +0013 # @var_4 (0x4) r- int
                     (+) alloc +0014 # @var_3 (0x3) r- int
                     (+) alloc +0015 # @var_2 (0x2) r- int
                     (+) alloc +0016 # @var_@ (0x0) -w ???
                     (+) alloc +0017 # @var_a (0x0) -w ???
                        $binop "Separator" `05:04:01:01` # r- int
                            $binop "Assign" `04:08:01:01` +0010 # rw string
                                ref &0009 # rw string
                                ref &0010 # r- string
                            $binop "Assign" `05:09:01:02` # rw int
                                ref &0011 # -w ???
                                $binop "Addition" `05:13:01:02` # r- int
                                    ref &0011 # -w ???
                                    ref &0002 # r- int
                        $uniop "Builtin call" `06:06:03:01` # r- ???
                            $binop "Separator" `10:08:01:02` +0017 # r- ???
                                $binop "Builtin call" `07:28:03:02` # r- ???
                                    $binop "Sequence" `07:23:01:03` +0000 # r- ???
                                        ref &0012 # r- int
                                        $binop "Sequence" `07:19:01:04` +0000 # r- ???
                                            ref &0013 # r- int
                                            ref &0014 # r- int
                                        ref &0015 # r- int
                                        ref &0002 # r- int
                                    $binop "Closure" `08:09:02:03` # rw ???
                                        ref &0016 # -w ???
                                        $uniop "Builtin call" `08:12:03:03` # r- ???
                                            ref &0016 # -w ???
                                ref &0017 # -w ???
                     (-) drop +0017
                     (-) drop +0016
                     (-) drop +0015
                     (-) drop +0014
                     (-) drop +0013
                     (-) drop +0012
                     (-) drop +0011
                     (-) drop +0010
                     (-) drop +0009
            $binop "Assign" `13:08:01:00` +0002 # rw int
                ref &0004 # rw int
                ref &0002 # r- int
        $binop "Assign" `14:07:01:00` +0006 # rw int
            ref &0005 # rw int
            ref &0006 # r- int
    $binop "Assign" `15:07:01:00` +0008 # rw int
        ref &0007 # rw int
        ref &0008 # r- int
 (-) drop +0008
 (-) drop +0007
 (-) drop +0006
 (-) drop +0005
 (-) drop +0004
 (-) drop +0003
 (-) drop +0002
 (-) drop +0001
 (-) drop +0000

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
