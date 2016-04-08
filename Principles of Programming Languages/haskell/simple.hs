module Simple where

scoreToLetter :: Integer -> Char
scoreToLetter n
   | n > 90 = A
   | n > 80 = B
   | n > 70 = C
   | otherwise = F

len  :: [a] -> Integer
len [] = 0
len (x:s) = 1 + len s

ones = 1 : ones
rpt n = n: rpt n
ct n = n : ct (n+1)

sq start lth = take lth (ct start)

front :: Integer -> [a] -> [a]
front _ [] = []
front 0 (x:s) = []
front n (x:s) = x : front (n-1) s

-- built in version of front is take
t4 = take 4 ones

--cycle
c1 = cycle ["rah","RAAAH", "ru"]
t5 = take 5 c1
t7 = take 7 ([3,4,5]++(cycle [5]))

-- zipWith
z1 = zipWith (+) [3,5,7] [3, 6, 9]

-- fibonacci
fibs = 1 : 1 : zipWith (+) fibs (tail fibs)
