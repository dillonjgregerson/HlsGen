input Int32 a, b, c, one
output Int32 z, x

variable Int32 d, e, f, g, h 
variable Int1 dLTe, dEQe, dLTEe

d = a + b
e = a + c
f = a - b  
dEQe = d == e
dLTe = d > e
dLTEe = dEQe + dLTe

if ( dLTEe ) {

	if ( dLTe ) {
		g = e + one
		h = f + one
	}
	g = d + e
	h = f + e
}
x = h << one
z = h >> one
