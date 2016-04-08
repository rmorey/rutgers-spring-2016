% sumto(N, Sum) - Sum is the sum of numbers from 1 thru N
sumto(1, 1).
sumto(N, Sum):- N1 is N - 1, sumto(N1, S1), Sum is S1 + N.


