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
    blockMesh

Description
    A multi-block mesh generator.

    Uses the block mesh description found in
      - \c system/blockMeshDict
      - \c system/\<region\>/blockMeshDict
      - \c constant/polyMesh/blockMeshDict
      - \c constant/\<region\>/polyMesh/blockMeshDict

Usage
    \b blockMesh [OPTION]

    Options:
      - \par -blockTopology
        Write the topology as a set of edges in OBJ format.

      - \par -region \<name\>
        Specify an alternative mesh region.

      - \par -dict \<filename\>
        Specify alternative dictionary for the block mesh description.

        //-wr    My question is naive: how blockMesh build the mesh that I did int he commit:
        //-wr    '2ac9f5b9'

\*---------------------------------------------------------------------------*/


#include "Time.H"
#include "IOdictionary.H"
#include "IOPtrList.H"

#include "blockMesh.H"
#include "attachPolyTopoChanger.H"
#include "polyTopoChange.H"
#include "emptyPolyPatch.H"
#include "cyclicPolyPatch.H"

#include "argList.H"
#include "OSspecific.H"
#include "OFstream.H"

#include "Pair.H"
#include "slidingInterface.H"

using namespace Foam;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{
  //-wr-question What' argList? Are they just for the extra info?
  argList::noParallel();
  argList::addBoolOption
    (
     "blockTopology",
     "write block edges and centres as .obj files"
     );
  argList::addBoolOption
    (
     "noClean",
     "keep the existing files in the polyMesh"
     );
  argList::addOption
    (
     "dict",
     "file",
     "specify alternative dictionary for the blockMesh description"
     );

  //-wr    I was shock by the ability by the ascii's power!
  //-wr      Look at this, you can even draw a complex cubic using ASCII!!!
  argList::addNote
    (
     "Block description\n"
        "\n"
        "  For a given block, the correspondence between the ordering of\n"
        "  vertex labels and face labels is shown below.\n"
        "  For vertex numbering in the sequence 0 to 7 (block, centre):\n"
        "    faces 0 (f0) and 1 are left and right, respectively;\n"
        "    faces 2 and 3 are front and back; \n"
        "    and faces 4 and 5 are bottom and top::\n"
        "\n"
        "                 7 ---- 6\n"
        "            f5   |\\     |\\   f3\n"
        "            |    | 4 ---- 5   \\\n"
        "            |    3 |--- 2 |    \\\n"
        "            |     \\|     \\|    f2\n"
        "            f4     0 ---- 1\n"
        "\n"
        "       Z         f0 ----- f1\n"
        "       |  Y\n"
        "       | /\n"
        "       O --- X\n"
    );

    #include "addRegionOption.H"
    #include "setRootCase.H"
    #include "createTime.H"

    const word dictName("blockMeshDict");

    word regionName;
    word regionPath;

    // Check if the region is specified otherwise mesh the default region
    if (args.optionReadIfPresent("region", regionName, polyMesh::defaultRegion))
    {
        Info<< nl << "Generating mesh for region " << regionName << endl;
        regionPath = regionName;
    }

    // Search for the appropriate blockMesh dictionary....

    fileName dictPath;

    // Check if the dictionary is specified on the command-line
    if (args.optionFound("dict"))
    {
        dictPath = args["dict"];

        dictPath =
        (
            isDir(dictPath)
          ? dictPath/dictName
          : dictPath
        );
    }
    // Check if dictionary is present in the constant directory
    else if
    (
        exists
        (
            runTime.path()/runTime.constant()
           /regionPath/polyMesh::meshSubDir/dictName
        )
    )
    {
        dictPath =
            runTime.constant()
           /regionPath/polyMesh::meshSubDir/dictName;
    }
    // Otherwise assume the dictionary is present in the system directory
    else
    {
        dictPath = runTime.system()/regionPath/dictName;
    }

    if (!args.optionFound("noClean"))
    {
        fileName polyMeshPath
        (
            runTime.path()/runTime.constant()/regionPath/polyMesh::meshSubDir
        );

        if (exists(polyMeshPath))
        {
            if (exists(polyMeshPath/dictName))
            {
                Info<< "Not deleting polyMesh directory " << nl
                    << "    " << polyMeshPath << nl
                    << "    because it contains " << dictName << endl;
            }
            else
            {
                Info<< "Deleting polyMesh directory" << nl
                    << "    " << polyMeshPath << endl;
                rmDir(polyMeshPath);
            }
        }
    }

    IOobject meshDictIO
    (
        dictPath,
        runTime,
        IOobject::MUST_READ,
        IOobject::NO_WRITE,
        false
    );

    if (!meshDictIO.typeHeaderOk<IOdictionary>(true))
    {
        FatalErrorInFunction
            << meshDictIO.objectPath()
            << nl
            << exit(FatalError);
    }

    Info<< "Creating block mesh from\n    "
        << meshDictIO.objectPath() << endl;

    IOdictionary meshDict(meshDictIO);
    blockMesh blocks(meshDict, regionName);


    //-wr    if (args.optionFound("blockTopology"))
    //-wr    {
    //-wr        // Write mesh as edges.
    //-wr        {
    //-wr            fileName objMeshFile("blockTopology.obj");
    //-wr    
    //-wr            OFstream str(runTime.path()/objMeshFile);
    //-wr    
    //-wr            Info<< nl << "Dumping block structure as Lightwave obj format"
    //-wr                << " to " << objMeshFile << endl;
    //-wr    
    //-wr            blocks.writeTopology(str);
    //-wr        }
    //-wr    
    //-wr        // Write centres of blocks
    //-wr        {
    //-wr            fileName objCcFile("blockCentres.obj");
    //-wr    
    //-wr            OFstream str(runTime.path()/objCcFile);
    //-wr    
    //-wr            Info<< nl << "Dumping block centres as Lightwave obj format"
    //-wr                << " to " << objCcFile << endl;
    //-wr    
    //-wr            const polyMesh& topo = blocks.topology();
    //-wr    
    //-wr            const pointField& cellCentres = topo.cellCentres();
    //-wr    
    //-wr            forAll(cellCentres, celli)
    //-wr            {
    //-wr                // point cc = b.blockShape().centre(b.points());
    //-wr                const point& cc = cellCentres[celli];
    //-wr    
    //-wr                str << "v " << cc.x() << ' ' << cc.y() << ' ' << cc.z() << nl;
    //-wr            }
    //-wr        }
    //-wr    
    //-wr        Info<< nl << "end" << endl;
    //-wr    
    //-wr        return 0;
    //-wr    }


    Info<< nl << "Creating polyMesh from blockMesh" << endl;

    word defaultFacesName = "FacesWhat";
    word defaultFacesType = emptyPolyPatch::typeName;
    polyMesh mesh
    (
        IOobject
        (
            regionName,
            runTime.constant(),
            runTime
        ),
        xferCopy(blocks.points()),
        //-wr    blocks.points(),
                                             //could we re-use space?
                                              //-wr    Let's try to see if I
                                              //-wr    can answer this
                                              //-wr    question.

        //-wr    Here's issue: why not just blocks.points()?
        
        blocks.cells(),
        blocks.patches(),
        blocks.patchNames(),
        blocks.patchDicts(),
        defaultFacesName,
        defaultFacesType
    );


    //-wr    // Read in a list of dictionaries for the merge patch pairs
    //-wr    if (meshDict.found("mergePatchPairs"))
    //-wr    {
    //-wr        List<Pair<word>> mergePatchPairs
    //-wr        (
    //-wr            meshDict.lookup("mergePatchPairs")
    //-wr        );
    //-wr    
    //-wr        #include "mergePatchPairs.H"
    //-wr    }
    //-wr    else
    //-wr    {
    //-wr        Info<< nl << "There are no merge patch pairs edges" << endl;
    //-wr    }


    // Set any cellZones (note: cell labelling unaffected by above
    // mergePatchPairs)

    //-wr    label nZones = blocks.numZonedBlocks();


    //-wr what is the zone in OF?
    //-wr    if (nZones > 0)
    //-wr    {
    //-wr        Info<< nl << "Adding cell zones" << endl;
    //-wr    
    //-wr        // Map from zoneName to cellZone index
    //-wr        HashTable<label> zoneMap(nZones);
    //-wr    
    //-wr        // Cells per zone.
    //-wr        List<DynamicList<label>> zoneCells(nZones);
    //-wr    
    //-wr        // Running cell counter
    //-wr        label celli = 0;
    //-wr    
    //-wr        // Largest zone so far
    //-wr        label freeZoneI = 0;
    //-wr    
    //-wr        forAll(blocks, blockI)
    //-wr        {
    //-wr            const block& b = blocks[blockI];
    //-wr            const List<FixedList<label, 8>> blockCells = b.cells();
    //-wr            const word& zoneName = b.zoneName();
    //-wr    
    //-wr            if (zoneName.size())
    //-wr            {
    //-wr                HashTable<label>::const_iterator iter = zoneMap.find(zoneName);
    //-wr    
    //-wr                label zoneI;
    //-wr    
    //-wr                if (iter == zoneMap.end())
    //-wr                {
    //-wr                    zoneI = freeZoneI++;
    //-wr    
    //-wr                    Info<< "    " << zoneI << '\t' << zoneName << endl;
    //-wr    
    //-wr                    zoneMap.insert(zoneName, zoneI);
    //-wr                }
    //-wr                else
    //-wr                {
    //-wr                  zoneI = iter();
    //-wr                }
    //-wr    
    //-wr                
    //-wr                forAll(blockCells, i)
    //-wr                {
    //-wr                    zoneCells[zoneI].append(celli++);
    //-wr                }
    //-wr            }
    //-wr            else
    //-wr            {
    //-wr                celli += b.cells().size();
    //-wr            }
    //-wr        }
    //-wr    
    //-wr    
    //-wr        List<cellZone*> cz(zoneMap.size());
    //-wr    
    //-wr        forAllConstIter(HashTable<label>, zoneMap, iter)
    //-wr        {
    //-wr            label zoneI = iter();
    //-wr    
    //-wr            cz[zoneI] = new cellZone
    //-wr            (
    //-wr                iter.key(),
    //-wr                zoneCells[zoneI].shrink(),
    //-wr                zoneI,
    //-wr                mesh.cellZones()
    //-wr            );
    //-wr        }
    //-wr    
    //-wr        mesh.pointZones().setSize(0);
    //-wr        mesh.faceZones().setSize(0);
    //-wr        mesh.cellZones().setSize(0);
    //-wr        mesh.addZones(List<pointZone*>(0), List<faceZone*>(0), cz);
    //-wr    }


    //-wr    // Detect any cyclic patches and force re-ordering of the faces
    //-wr    {
    //-wr        const polyPatchList& patches = mesh.boundaryMesh();
    //-wr        bool hasCyclic = false;
    //-wr        forAll(patches, patchi)
    //-wr        {
    //-wr            if (isA<cyclicPolyPatch>(patches[patchi]))
    //-wr            {
    //-wr                hasCyclic = true;
    //-wr                break;
    //-wr            }
    //-wr        }
    //-wr    
    //-wr        if (hasCyclic)
    //-wr        {
    //-wr            Info<< nl << "Detected cyclic patches; ordering boundary faces"
    //-wr                << endl;
    //-wr            const word oldInstance = mesh.instance();
    //-wr            polyTopoChange meshMod(mesh);
    //-wr            meshMod.changeMesh(mesh, false);
    //-wr            mesh.setInstance(oldInstance);
    //-wr        }
    //-wr    }


    // Set the precision of the points data to 10
    //-wr    IOstream::defaultPrecision(max(10u, IOstream::defaultPrecision()));

    Info<< nl << "Writing polyMesh" << endl;
    mesh.removeFiles();
    if (!mesh.write())
      {
        FatalErrorInFunction
          << "Failed writing polyMesh."
          << exit(FatalError);
      }

    //-wr Because that's the first part of if-else statement.
    //    If force deleted, it will not working as wished.
    // Write summary
    {
        const polyPatchList& patches = mesh.boundaryMesh();

        Info<< "----------------" << nl
            << "wr-block-Mesh-test Information" << nl
            << "----------------" << nl
            << "  " << "boundingBox: " << boundBox(mesh.points()) << nl
            << "  " << "nPoints: " << mesh.nPoints() << nl
            << "  " << "nCells: " << mesh.nCells() << nl
            << "  " << "nFaces: " << mesh.nFaces() << nl
            << "  " << "nInternalFaces: " << mesh.nInternalFaces() << nl;

        Info<< "----------------" << nl
            << "Patches" << nl
            << "----------------" << nl;

        forAll(patches, patchi)
        {
            const polyPatch& p = patches[patchi];

            Info<< "  " << "patch " << patchi
                << " (start: " << p.start()
                << " size: " << p.size()
                << ") name: " << p.name()
                << nl;
        }
    }

    Info<< "\nEnd\n" << endl;

    return 0;
}


// ************************************************************************* //
