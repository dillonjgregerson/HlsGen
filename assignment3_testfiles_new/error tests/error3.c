input Int16 a, b, c

output Int8 z
output Int16 x

variable Int16 d, e, f

d = a + b
e = a + c
g = d > e
z = g ? d : e
f = a * c
x = f - d  

