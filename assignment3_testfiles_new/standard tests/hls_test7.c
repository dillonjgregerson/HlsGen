input Int32 u, x, y, dx, a, three

output Int32 u1, x1, y1, c

variable Int32 t1, t2, t3, t4, t5, t6, t7, vx1

x1 = x + dx
vx1 = x + dx
t1 = three * x
t2 = u * dx
t3 = t1 * t2
t4 = u - t3
t5 = three * y
t6 = t5 * dx
u1 = t4 - t6
t7 = u * dx
y1 = y + t7
c = vx1 < a
