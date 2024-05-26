# xre-engine - Expression Evaluation Engine

```
# a program to find prime numbers from 0 -> 20
input:
i = 0;
n = 20;

(i = i + 1) <= n loop (
  f = 0;
  j = 1;

  j < n loop (
    ! i % j
      do ( f = f + 1 );
    j = j + 1
  );

  f == 2
    do print i
)

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

## Usage

see examples in `__tools__/examples`
