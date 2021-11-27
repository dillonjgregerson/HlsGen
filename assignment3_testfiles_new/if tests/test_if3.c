input Int32 a, b, c, d, e, f, g, h, sa, one, two, four
output Int32 avg

variable Int32 t1, t2, t3, t4, t5, t6, t7, t7div2, t7div4 
variable Int1 csa1, csa2, csa3

t1 = a + b
t2 = t1 + c 
t3 = t2 + d 
t4 = t3 + e 
t5 = t4 + f 
t6 = t5 + g 
t7 = t6 + h 
csa1 = sa > one
csa2 = sa > two
csa3 = sa > four

if ( csa1 ) {
   t7div2 = t7 >> one
   
   if ( csa2 ) {
      t7div4 = t7div2 >> one
   }
   else {
      t7div4 = t7 >> one
      if ( csa3 ) {
         avg = t7div4 >> one
      }
   }
}