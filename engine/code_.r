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
