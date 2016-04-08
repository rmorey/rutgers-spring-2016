% question 1
inDpt(joe, toys).
reportsToHead(X):-inDpt(X, _).
dptHead(sam, toys).
salaryIsLess(X, Y):-inDpt(X, D), inDpt(Y,D), reportsToHead(X, D), dptHead(Y, D).
salaryIsLess(joe, sam).

% question 2
fib(0, 0).
fib(1, 1).
fib(N, F):- N2 is N-2, fib(N2, F2), N1 is N-1, fib(N1, F1), F is  F2 + F1.

% question 3
make_double([], []).
make_double([H|T], [H, H| Result]) :- double(T, Result).
double(A, B) :- make_double(A, Double), Double = B.

% question 4
remove0([], []).
remove0([H|T], [H|Result]) :- not(H = 0), remove0(T, Result).
remove0([_|T], Result) :- remove0(T, Result).
without0(L1, L2) :- remove0(L1, W), W = L2.

dog(C).
