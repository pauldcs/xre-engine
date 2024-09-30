i = 0;

(i = i + 1) <= 50 loop {
    std_out ({1, 2, (3, i), 5} std_map {
		@ => std_buf @
	})
}
