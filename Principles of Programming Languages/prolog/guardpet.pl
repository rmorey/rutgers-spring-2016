guardpet(A):- domesticated(A), write([A, domesticated]), scary(A).
guardpet(killerbee).

domesticated(elephant).
domesticated(cat).
domesticated(goldfish).
domesticated(dog).

scary(A):- large(A), write([A, large]).

scary(A):- loud(A), write([A, loud]).

loud(dog).
loud(elephant).

large(elephant).



