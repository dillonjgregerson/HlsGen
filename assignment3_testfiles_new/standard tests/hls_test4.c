input UInt64 a, b, c, d, zero

output UInt64 z

variable UInt64 e, f, g
variable UInt1 gEQz  

e = a / b
f = c / d
g = a % b  
gEQz = g == zero
z = gEQz ? e : f 
