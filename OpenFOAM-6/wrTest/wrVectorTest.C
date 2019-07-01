// just do a simple vector inner product using OpenFOAM.
// a[1 2 3] and b[4 5 6]
// calculate a dot b =?

//-wr    #include "pointField.H"
//-wr    #include "vector.H"
//-wr    #include "point.H"
#include "vector.H"
//-wr    #include "VectorI.H"
//-wr    #include "VectorSpace.H"
//-wr    #include "primitiveFields.H"
//-wr    #include "pointFieldFwd.H"

using namespace Foam;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{

  //-wr    const Vector<int32_t>& a = (1 2 3);
  //-wr    const Vector<int32_t>& b = (4 5 6);

  //-wr    vector a = (1,2,3);
  //-wr    vector a = (1,2,3);
  //-wr    point a = (1, 2, 3);
  vector a;
  a.x()=1;
  a.y()=2;
  a.z()=3;

  vector b;
  b.x()=4;
  b.y()=5;
  b.z()=6;

  //-wr    vector c;  
  //-wr    const vector& a = (1 2 3);
  //-wr    const vector& b = (4 5 6);
  //-wr    c = a&b;

  scalar c;
  c = a & b;

  vector d(1,2,3);
  vector e(4,5,6);

  scalar g;
  g = d & e;
  //-wr    a[1 2 3] dot  b[4 5 6]
  //-wr    
  //-wr      = 1*4 +2*5 +3*6
  //-wr    
  //-wr      = 4+10+18
  //-wr    
  //-wr      = 32
  
  Info<< "Hi foo!" << endl;
  Info<<  c << endl;
  Info<<  g <<endl;
  return 0;
}
