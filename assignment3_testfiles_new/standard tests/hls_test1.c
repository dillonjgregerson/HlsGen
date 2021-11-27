input UInt16 a, b, c

output UInt8 z
output UInt16 x

variable UInt8 d, e, f, g

d = a + b
e = a + c
g = d > e
z = g ? d : e
f = a * c
x = f - d  
