  i = 0

  ; (i = i + 1) <= 2000 loop {
    j = 1
    ; j < 2000 loop {
      !(i % j) do { f = f + 1 }
      ; j = j + 1
    }

    ; (f == 2) do
      std_out i
  }

