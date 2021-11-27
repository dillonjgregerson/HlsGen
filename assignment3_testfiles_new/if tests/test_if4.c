input Int32 a, b, c, zero, one
input Int1 t
output Int32 z, x

variable Int32 e, g, d, f

d = a + b

if ( t ) {
   d = a - one
   f = a + c
   x = f - d
}
else {
	e = a + c
	g = d > e
	z = g ? d : e
	f = a * c
	x = f - d  
}
