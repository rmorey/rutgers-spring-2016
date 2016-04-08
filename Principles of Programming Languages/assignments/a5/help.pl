inList(Item, [Item|Rest]).
inList(Item, [_|Rest]) :- inList(Item, Rest).

del([], Target, []).
del([H|T], Target, [H|Result]) :- not(H = Target), del(T, Target, Result). % not the Target
del([_|T], Target, Result) :- del(T, Target, Result). % removes that instance of Target from list

