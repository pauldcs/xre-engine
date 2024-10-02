'Separator' -> r- int64_t local_0000
├── Alloc r- int64_t var_0000 # (0x0) token: '0'
├── Alloc rw int64_t var_0001 # (0x0) token: 'i'
├── Alloc r- int64_t var_0002 # (0x1) token: '1'
├── Alloc r- int64_t var_0003 # (0x7d0) token: '2000'
├── 'Assign' -> r- int64_t var_0000
│   ├── *r- int64_t var_0000
│   └── *rw int64_t var_0001
└── 'Loop' -> r- int64_t local_-2147483648
    ├── 'Less or equal' -> r- int64_t local_0000
    │   ├── 'Assign' -> r- int64_t local_0000
    │   │   ├── 'Addition' -> r- int64_t local_0000
    │   │   │   ├── *rw int64_t var_0001
    │   │   │   └── *r- int64_t var_0002
    │   │   └── *rw int64_t var_0001
    │   └── *r- int64_t var_0003
    └── 'Separator' -> r- int64_t local_0002
        ├── Alloc rw int64_t var_0004 # (0x0) token: 'f'
        ├── Alloc rw int64_t var_0005 # (0x0) token: 'j'
        ├── Alloc r- int64_t var_0006 # (0x2) token: '2'
        ├── 'Separator' -> r- int64_t local_0001
        │   ├── 'Separator' -> r- int64_t var_0002
        │   │   ├── 'Assign' -> r- int64_t var_0000
        │   │   │   ├── *r- int64_t var_0000
        │   │   │   └── *rw int64_t var_0004
        │   │   └── 'Assign' -> r- int64_t var_0002
        │   │       ├── *r- int64_t var_0002
        │   │       └── *rw int64_t var_0005
        │   └── 'Loop' -> r- int64_t local_-2147483648
        │       ├── 'Less than' -> r- int64_t local_0001
        │       │   ├── *rw int64_t var_0005
        │       │   └── *r- int64_t var_0003
        │       └── 'Separator' -> r- int64_t local_0003
        │           ├── 'Do' -> r- int64_t local_0002
        │           │   ├── 'Not' -> r- int64_t local_0002
        │           │   │   └── 'Modulus' -> r- int64_t local_0002
        │           │   │       ├── *rw int64_t var_0001
        │           │   │       └── *rw int64_t var_0005
        │           │   └── 'Assign' -> r- int64_t local_0003
        │           │       ├── 'Addition' -> r- int64_t local_0003
        │           │       │   ├── *rw int64_t var_0004
        │           │       │   └── *r- int64_t var_0002
        │           │       └── *rw int64_t var_0004
        │           └── 'Assign' -> r- int64_t local_0003
        │               ├── 'Addition' -> r- int64_t local_0003
        │               │   ├── *rw int64_t var_0005
        │               │   └── *r- int64_t var_0002
        │               └── *rw int64_t var_0005
        └── 'Do' -> r- int64_t local_0002
            ├── 'Equals' -> r- int64_t local_0002
            │   ├── *rw int64_t var_0004
            │   └── *r- int64_t var_0006
            └── 'Builtin call' `std_out` -> -- any local_0003
                └── *rw int64_t var_0001
        Drop var_0006
        Drop var_0005
        Drop var_0004
Drop var_0003
Drop var_0002
Drop var_0001
Drop var_0000
