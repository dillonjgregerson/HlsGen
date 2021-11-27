input Int32 a, b, c
output Int32 z, x

variable Int32 d, f, g, zrin

d = a + b
g = a < b
zrin = a + c
if ( g ) {
	zrin = a + b
}
f = a * c
x = f - d  
z = zrin + f