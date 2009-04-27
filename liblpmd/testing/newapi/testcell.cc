//
//
//

#include "cell.h"
#include "vector3.h"
#include <iostream>
#include <cmath>
#include <cassert>

void test1()
{
 Vector3 e[3];
 e[0]=e1;e[1]=e2;e[2]=e3;
 Cell a;
 for (int i=0 ; i< 3 ; ++i) {assert((a[i]-e[i]).Module() < 1E-10);}
}

void test2()
{
 Vector3 e[3];
 e[0]=RandomVector(),e[1]=RandomVector(),e[2]=RandomVector();
 Cell a(e[0],e[1],e[2]);
 for (int i=0 ; i< 3 ; ++i) {assert((a[i]-e[i]).Module() < 1E-10);}
}

void test3()
{
 double aa=1,b=1,c=1,alpha=90,beta=90,gamma=90;
 Cell a(aa,b,c,alpha,beta,gamma);
 assert((a[0]-e1).Module() < 1E-10);
 assert((a[1]-e2).Module() < 1E-10);
 assert((a[2]-e3).Module() < 1E-10);
}

void test4()
{
 Cell a("<1,2,3> <4 ,5,6> <7,8,9>");
 assert((a[0]-Vector3(1,2,3)).Module()<1E-10); 
 assert((a[1]-Vector3(4,5,6)).Module()<1E-10);
 assert((a[2]-Vector3(7,8,9)).Module()<1E-10);
}

void test5()
{
 Cell b("1 2 3 4 5 6 7 8 9");
 assert((b[0]-Vector3(1,2,3)).Module()<1E-10); 
 assert((b[1]-Vector3(4,5,6)).Module()<1E-10);
 assert((b[2]-Vector3(7,8,9)).Module()<1E-10);
}

void test6()
{
 Cell c("5 7 5 90 90 90");
 assert((c[0]-Vector3(5,0,0)).Module()<1E-10);
 assert((c[1]-Vector3(0,7,0)).Module()<1E-10);
 assert((c[2]-Vector3(0,0,5)).Module()<1E-10);
}

void test7()
{
 Cell a;
 assert(a.Periodicity(0)==true);
 assert(a.Periodicity(1)==true);
 assert(a.Periodicity(2)==true);
 a.Periodicity(0) = false;
 a.Periodicity(1) = false;
 a.Periodicity(2) = false;
 assert(a.Periodicity(0)==false);
 assert(a.Periodicity(1)==false);
 assert(a.Periodicity(2)==false);
}

void test8()
{
 Vector3 a(1,2,3);
 Vector3 b(4,5,6);
 Vector3 c(7,8,9);
 Cell CELL;
 CELL[0] = a;
 assert ((CELL[0]-a).Module()<1E-10);
 CELL[1] = b;
 assert ((CELL[1]-b).Module()<1E-10);
 CELL[2] = c;
 assert ((CELL[2]-c).Module()<1E-10);
}

int main()
{
 test1();
 test2();
 test3();
 // test4(); // FIXME: hacer pasar este test!
 test5();
 test6();
 test7();
 test8();

 return 0;
}



