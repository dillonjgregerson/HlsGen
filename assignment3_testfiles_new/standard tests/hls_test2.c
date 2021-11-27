input Int32 a, b, c

output Int32 z, x

variable Int32 d, e, f, g, h
variable UInt1 dLTe, dEQe  

d = a + b
e = a + c
f = a - b  
dEQe = d == e
dLTe = d < e
g = dLTe ? d : e 
h = dEQe ? g : f 
x = g << dLTe
z = h >> dEQe
