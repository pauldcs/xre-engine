i = 0;

(i = i + 1) <= 2000 loop {
  f = 0;
  j = 1;

  (j < 2000) loop {
    r = 100;
    !(i % j) do { f = f + 1 };
  
    j = j + 1
  };

  (f == 2) do { i }
}



[binop] Separator (depth: 00, 1:5)
  PUSH __var_00__ # i
  PUSH __var_01__ # 0
  PUSH __var_02__ # 1
  PUSH __var_03__ # 2000
    [binop] Assign (depth: 00, 1:2)
          &__var_00__
          &__var_01__
    [binop] Loop (depth: 00, 3:20)
        [binop] Less or equal (depth: 00, 3:12)
            [binop] Assign (depth: 01, 3:3)
                  &__var_00__
                [binop] Addition (depth: 01, 3:7)
                      &__var_00__
                      &__var_02__
              &__var_03__
        [binop] Separator (depth: 01, 12:3)
          PUSH __var_04__ # f
          PUSH __var_05__ # j
          PUSH __var_06__ # 2
            [binop] Separator (depth: 01, 5:7)
                [binop] Separator (depth: 01, 4:7)
                    [binop] Assign (depth: 01, 4:4)
                          &__var_04__
                          &__var_01__
                    [binop] Assign (depth: 01, 5:4)
                          &__var_05__
                          &__var_02__
                [binop] Loop (depth: 01, 7:13)
                    [binop] Less than (depth: 02, 7:5)
                          &__var_05__
                          &__var_03__
                    [binop] Separator (depth: 02, 9:29)
                      PUSH __var_07__ # r
                      PUSH __var_08__ # 100
                        [binop] Separator (depth: 02, 8:11)
                            [binop] Assign (depth: 02, 8:6)
                                  &__var_06__
                                  &__var_07__
                            [binop] Do (depth: 02, 9:13)
                                [uniop] Not (depth: 02, 9:4)
                                    [binop] Modulus (depth: 03, 9:8)
                                          &__var_00__
                                          &__var_05__
                                [binop] Assign (depth: 03, 9:20)
                                      &__var_04__
                                    [binop] Addition (depth: 03, 9:24)
                                          &__var_04__
                                          &__var_02__
                        [binop] Assign (depth: 02, 11:6)
                              &__var_05__
                            [binop] Addition (depth: 02, 11:10)
                                  &__var_05__
                                  &__var_02__
                       POP __var_07__
                       POP __var_08__
            [binop] Do (depth: 01, 14:11)
                [binop] Equals (depth: 02, 14:5)
                      &__var_04__
                      &__var_06__
                [uniop] Builtin call (depth: 02, 14:16)
                      &__var_00__
           POP __var_04__
           POP __var_05__
           POP __var_06__
   POP __var_00__
   POP __var_01__
   POP __var_02__
   POP __var_03__

[0] <Separator>
   [0] <Assign>
      [0] id: 'i'
      [0] 0
   [0] <Loop>
      [0] <Less or equal>
         [1] <Assign>
            [1] id: 'i'
            [1] <Addition>
               [1] id: 'i'
               [1] 1
         [0] 2000
      [1] <Separator>
         [1] <Separator>
            [1] <Separator>
               [1] <Assign>
                  [1] id: 'f'
                  [1] 0
               [1] <Assign>
                  [1] id: 'j'
                  [1] 1
            [1] <Loop>
               [2] <Less than>
                  [2] id: 'j'
                  [2] 2000
               [2] <Separator>
                  [2] <Separator>
                     [2] <Assign>
                        [2] id: 'r'
                        [2] 100
                     [2] <Do>
                        [2] <Not>
                           [3] <Modulus>
                              [3] id: 'i'
                              [3] id: 'j'
                        [3] <Assign>
                           [3] id: 'f'
                           [3] <Addition>
                              [3] id: 'f'
                              [3] 1
                  [2] <Assign>
                     [2] id: 'j'
                     [2] <Addition>
                        [2] id: 'j'
                        [2] 1
         [1] <Do>
            [2] <Equals>
               [2] id: 'f'
               [2] 2
            [2] id: 'i'


%> print (1, 2, (i, 1))
[uniop] Builtin call (depth: 00, 1:0)
  PUSH __var_00__ // 1
  PUSH __var_01__ // 2
  PUSH __var_02__ // i
    [binop] Sequence point (depth: 01, 1:11)
          &__var_00__
          &__var_01__
        [binop] Sequence point (depth: 02, 1:15)
              &__var_02__
              &__var_00__
   POP __var_00__
   POP __var_01__
   POP __var_02__
