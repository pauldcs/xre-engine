i = 0;

{ i = i + 1 } <= 100 loop {
       !{ i % 15 } do out "FizzBuzz"
  else !{ i %  3 } do out "Fizz"
  else !{ i %  5 } do out "Buzz"
  else                out i
}
