/*******************************************************************************
 * Copyright (c) 2018,
 *  Folding Rain LLC
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * - Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * - Neither the name of Sony Pictures Imageworks, nor Industrial Light & Magic,
 *   nor the names of their contributors may be used to endorse or promote
 *   products derived from this software without specific prior written
 *   permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <maya/MIOStream.h>
#include <maya/MSimple.h>
#include <maya/MFnMesh.h>

#include <lifeCmd.h>


// Macro for error checking
#define checkErr(stat,msg) \
    if ( MS::kSuccess != stat ) { \
        displayError(msg); \
                return stat; \
}


void* LifeCmd::creator() { return new LifeCmd; }


MStatus LifeCmd::doIt(const MArgList&) {
	MGlobal::displayInfo("Life By Joe Howes");
	doThing();
	return MS::kSuccess;
}


MStatus initializePlugin(MObject obj) {
	MFnPlugin plugin(obj, "Folding Rain LLC", "0.1", "Any");
	MStatus status = plugin.registerCommand("life", LifeCmd::creator);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	return status;
}

 
MStatus uninitializePlugin(MObject obj) {
	MFnPlugin plugin(obj);
	MStatus status = plugin.deregisterCommand("helloWorld");
	CHECK_MSTATUS_AND_RETURN_IT(status);
	return status;
}


void LifeCmd::FILL( double x, double y, double z )
{
	MFloatPoint pnt( (float)x, (float)y, (float)z );
	iarr.append( pnt );
}


MStatus LifeCmd::doThing() {
	MStatus stat;

	// create a poly function set
	//
	MFnMesh fnPoly;

	iarr.clear();
	faceCounts.clear();
	faceConnects.clear();

	createPlane();

	// Construct the point array
	//
	pa.clear();
	int i;
	for ( i=0; i<num_verts; i++ )
		pa.append(iarr[i]);

	// If we are using polygon data then set up the face connect array
	// here. Otherwise, the create function will do it.
	//
	if ( NULL != p_gons ) {
		num_face_connects = num_faces * edges_per_face;
		num_edges = num_face_connects/2;

		for ( i=0; i<num_faces; i++ )
			faceCounts.append( edges_per_face );

		for ( i=0; i<(num_faces*edges_per_face); i++ )
			faceConnects.append( p_gons[i]-1 );
	}

	// Call the poly creation method to create the polygon
	newTransform = fnPoly.create( num_verts, num_faces, pa, 
			faceCounts, faceConnects, MObject::kNullObj, &stat );
	checkErr( stat, "Could not create MFnMesh" );

	// Primitive is created so tell shape it has changed
	//
	fnPoly.updateSurface();

	dgModifier.renameNode( newTransform, "pPrimitive1" );
	dgModifier.doIt();

	//
	// Put the polygon into a shading group
	MString cmd ("sets -e -fe initialShadingGroup ");
	cmd += fnPoly.name();
	dgModifier.commandToExecute( cmd );

	MFnDagNode fnDagNode( newTransform, &stat );
	if ( MS::kSuccess == stat )
	{
		cmd = "select ";
		cmd += fnDagNode.name();
		dgModifier.commandToExecute( cmd );
	}

	dgModifier.doIt();

	return MS::kSuccess;
}


void LifeCmd::createPlane() {
	MGlobal::displayInfo("creating plane...");

	int w = 2;
	int h = 2;
	double size = 2.0;
	double hSize, wSize;

	num_verts = 0;
	num_faces = 0;
	edges_per_face = 4;

	if ( w < 1 ) w = 1;
	if ( h < 1 ) h = 1;
	if ( size < 0.0001 ) size = 1.0;

	wSize = size / w;
	hSize = size / h;

	// create vertices
	//
	double x, z;
	for (z = -size/2.0; z <= size / 2.0; z += hSize )
	{
		for ( x = -size/2.0; x <= size/2.0; x += wSize )
		{
			FILL( x, 0, z );
			num_verts++;
		}
	}

	// create polys
	//
	int v0, v1, v2, v3;
	for (int i = 0; i < h; i++ )
	{
		for ( int j = 0; j < w; j++ )
		{
			v0 = j + (w+1) * i;
			v1 = j + 1 + (w+1) * i;
			v2 = j + 1 + (w+1) * (i+1);
			v3 = j + (w+1) * (i+1);

			faceConnects.append( v0 );
			faceConnects.append( v3 );
			faceConnects.append( v2 );
			faceConnects.append( v1 );
			num_faces++;
			faceCounts.append( edges_per_face );
		}
	}

	num_face_connects = num_faces * edges_per_face;
	num_edges = num_face_connects/2;
}

