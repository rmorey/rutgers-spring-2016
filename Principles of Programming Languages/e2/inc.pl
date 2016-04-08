inOrder([]).
inOrder([_]).
inOrder([H, N|T]) :- H < N, inOrder([N|T]).

suffix([_|T], T).
suffix([_|T], T1) :- suffix(T, T1).

odds([], []).
odds([_], [_]).
odds([A1, _| AT], [B1|BT]) :- A1 = B1, odds(AT, BT).
