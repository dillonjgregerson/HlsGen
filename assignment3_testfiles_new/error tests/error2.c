input Int16 a, b, c

output Int8 z

variable Int16 d, e, f, g

d = a + b
e = a + c
g = d > e
z = g ? d : e
f = a * c
x = f - d  

