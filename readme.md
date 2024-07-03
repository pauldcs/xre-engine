# xre-engine - Expression Evaluation Engine

```
# finding prime numbers in range 0, 20

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
    .print i
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

## Usage

see examples in `__tools__/examples`
