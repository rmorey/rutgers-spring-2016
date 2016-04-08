eats(wolf, lamb).
eats(lamb, grass).
eats(lion, X):-eats(X, Food),plant(Food).
plant(grass).
