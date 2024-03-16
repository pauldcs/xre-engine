i = 1;
n = 2000;
i <= n loop (
    f = 0;
    j = 1;
    j < n loop (
        ! i % j do (f += 1);
        j += 1
    );
    f == 2 do $i;
    i += 1
)
