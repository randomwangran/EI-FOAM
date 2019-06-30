// just do a simple vector inner product using OpenFOAM.
// a[1 2 3] and b[4 5 6]
// calculate a dot b =?


#include "Time.H"
#include "IOdictionary.H"
#include "IOPtrList.H"

#include "blockMesh.H"
#include "polyTopoChange.H"
#include "emptyPolyPatch.H"
#include "cyclicPolyPatch.H"

#include "argList.H"
#include "OSspecific.H"
#include "OFstream.H"

#include "Vector.H"

using namespace Foam;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{

  const Vector<int32_t>& a = (1 2 3);
  const Vector<int32_t>& b = (4 5 6);

  Info<< "Hi foo!" << endl;
  return 0;
}
