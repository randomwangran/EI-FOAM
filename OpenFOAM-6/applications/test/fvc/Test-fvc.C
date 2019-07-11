/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2011-2018 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Application
    test

Description
    Finite volume method test code.

\*---------------------------------------------------------------------------*/

#include "fvCFD.H"
//-wr    where is this fvCFD.H?
//       /OpenFOAM-6/src/finiteVolume/cfdTools/general/include/fvCFD.H
//-wr    #ifndef fvCFD_H
//-wr    #define fvCFD_H
//-wr    
//-wr    #include "parRun.H"
//-wr    
//-wr    #include "Time.H"
//-wr    #include "fvMesh.H"
//-wr    #include "fvc.H"
//-wr    #include "fvMatrices.H"
//-wr    #include "fvm.H"
//-wr    #include "linear.H"
//-wr    #include "uniformDimensionedFields.H"
//-wr    #include "calculatedFvPatchFields.H"
//-wr    #include "extrapolatedCalculatedFvPatchFields.H"
//-wr    #include "fixedValueFvPatchFields.H"
//-wr    #include "zeroGradientFvPatchFields.H"
//-wr    #include "fixedFluxPressureFvPatchScalarField.H"
//-wr    #include "constrainHbyA.H"
//-wr    #include "constrainPressure.H"
//-wr    #include "adjustPhi.H"
//-wr    #include "findRefCell.H"
//-wr    #include "IOMRFZoneList.H"
//-wr    #include "constants.H"
//-wr    
//-wr    #include "OSspecific.H"
//-wr    #include "argList.H"
//-wr    #include "timeSelector.H"
//-wr    
//-wr    #ifndef namespaceFoam
//-wr    #define namespaceFoam
//-wr        using namespace Foam;
//-wr    #endif
//-wr    
//-wr    #endif

//-wr    In case tags not working... do this under src
//-wr    find ./* -name setRootCase.H
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{
    #include "setRootCase.H"
    //-wr    do checking
    //-wr    Foam::argList args(argc, argv);
    //-wr    if (!args.checkRootCase())
    //-wr    {
    //-wr        Foam::FatalError.exit();
    //-wr    }

  
    #include "createTime.H"

    //-wr    Foam::Info<< "Create time\n" << Foam::endl;
    //-wr    
    //-wr    Foam::Time runTime(Foam::Time::controlDictName, args);

    #include "createMesh.H"

    //-wr    Foam::Info
    //-wr    << "Create mesh for time = "
    //-wr    << runTime.timeName() << Foam::nl << Foam::endl;
    //-wr    
    //-wr    Foam::fvMesh mesh
    //-wr    (
    //-wr        Foam::IOobject
    //-wr        (
    //-wr            Foam::fvMesh::defaultRegion,
    //-wr            runTime.timeName(),
    //-wr            runTime,
    //-wr            Foam::IOobject::MUST_READ
    //-wr        )
    //-wr    );

    volScalarField fx(pow(mesh.C().component(vector::X), 1));
    fx.write();
    volScalarField gradx4(fvc::grad(fx)().component(vector::X));
    gradx4.write();

    volVectorField curlC(fvc::curl(1.0*mesh.C()));
    curlC.write();

    surfaceScalarField xf(mesh.Cf().component(vector::X));
    surfaceScalarField xf4(pow(xf, 4));

    for (int i=1; i<xf4.size()-1; i++)
    {
        scalar gradx4a = (xf4[i] - xf4[i-1])/(xf[i] - xf[i-1]);
        Info<< (gradx4a - gradx4[i])/gradx4a << endl;
    }

    Info<< "end" << endl;
}


// ************************************************************************* //
