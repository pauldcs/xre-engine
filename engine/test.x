i = 0;
n = 2000;

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
