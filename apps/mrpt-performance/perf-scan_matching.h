/* +---------------------------------------------------------------------------+
   |          The Mobile Robot Programming Toolkit (MRPT) C++ library          |
   |                                                                           |
   |                   http://mrpt.sourceforge.net/                            |
   |                                                                           |
   |   Copyright (C) 2005-2010  University of Malaga                           |
   |                                                                           |
   |    This software was written by the Machine Perception and Intelligent    |
   |      Robotics Lab, University of Malaga (Spain).                          |
   |    Contact: Jose-Luis Blanco  <jlblanco@ctima.uma.es>                     |
   |                                                                           |
   |  This file is part of the MRPT project.                                   |
   |                                                                           |
   |     MRPT is free software: you can redistribute it and/or modify          |
   |     it under the terms of the GNU General Public License as published by  |
   |     the Free Software Foundation, either version 3 of the License, or     |
   |     (at your option) any later version.                                   |
   |                                                                           |
   |   MRPT is distributed in the hope that it will be useful,                 |
   |     but WITHOUT ANY WARRANTY; without even the implied warranty of        |
   |     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         |
   |     GNU General Public License for more details.                          |
   |                                                                           |
   |     You should have received a copy of the GNU General Public License     |
   |     along with MRPT.  If not, see <http://www.gnu.org/licenses/>.         |
   |                                                                           |
   +---------------------------------------------------------------------------+ */
#include <mrpt/slam.h>
#include <mrpt/gui.h>
#include <mrpt/scanmatching.h>

using namespace mrpt::vision;
using namespace mrpt::utils;
using namespace mrpt::poses;
using namespace mrpt::gui;
using namespace mrpt::system;
using namespace mrpt::scanmatching;
using namespace mrpt::slam;
using namespace std;


typedef std::vector< std::vector< double > > TPoints;

// ------------------------------------------------------
//				Generate both sets of points
// ------------------------------------------------------
void generate_points( TPoints &pA, TPoints &pB )
{
	const double		Dx = 0.5;
	const double		Dy = 1.5;
	const double		Dz = 0.75;

	const double		yaw = DEG2RAD(10);
	const double		pitch = DEG2RAD(20);
	const double		roll = DEG2RAD(5);

	pA.resize( 5 );		// A set of points at "A" reference system
	pB.resize( 5 );		// A set of points at "B" reference system
	
	pA[0].resize(3);	pA[0][0] = 0.0;		pA[0][1] = 0.5;		pA[0][2] = 0.4;		
	pA[1].resize(3);	pA[1][0] = 1.0;		pA[1][1] = 1.5;		pA[1][2] = -0.1;		
	pA[2].resize(3);	pA[2][0] = 1.2;		pA[2][1] = 1.1;		pA[2][2] = 0.9;		
	pA[3].resize(3);	pA[3][0] = 0.7;		pA[3][1] = 0.3;		pA[3][2] = 3.4;		
	pA[4].resize(3);	pA[4][0] = 1.9;		pA[4][1] = 2.5;		pA[4][2] = -1.7;

	CPose3DQuat qPose = CPose3D( Dx, Dy, Dz, yaw, pitch, roll ); 
	for( unsigned int i = 0; i < 5; ++i )
	{
		pB[i].resize( 3 );
		qPose.inverseComposePoint( pA[i][0], pA[i][1], pA[i][2], pB[i][0], pB[i][1], pB[i][2] );
	}

} // end generate_points

// ------------------------------------------------------
//				Generate a list of matched points
// ------------------------------------------------------
void generate_list_of_points( const TPoints &pA, const TPoints &pB, TMatchingPairList &list )
{
	TMatchingPair		pair;
	for( unsigned int i = 0; i < 5; ++i )
	{
		pair.this_idx	= pair.other_idx = i;
		pair.this_x		= pA[i][0]; 
		pair.this_y		= pA[i][1]; 
		pair.this_z		= pA[i][2]; 

		pair.other_x	= pB[i][0]; 
		pair.other_y	= pB[i][1]; 
		pair.other_z	= pB[i][2]; 

		list.push_back( pair );
	}
} // end generate_list_of_points

// ------------------------------------------------------
//				Genreate a vector of matched points
// ------------------------------------------------------
void generate_vector_of_points(  const TPoints &pA, const TPoints &pB, vector_double &inV )
{
	// The input vector: inV = [pA1x, pA1y, pA1z, pB1x, pB1y, pB1z, ... ]
	inV.resize( 30 );
	for( unsigned int i = 0; i < 5; ++i )
	{
		inV[6*i+0] = pA[i][0]; inV[6*i+1] = pA[i][1]; inV[6*i+2] = pA[i][2];
		inV[6*i+3] = pB[i][0]; inV[6*i+4] = pB[i][1]; inV[6*i+5] = pB[i][2];
	}
} // end generate_vector_of_points

// ------------------------------------------------------
//				Benchmark: using CPose3D
// ------------------------------------------------------
double scan_matching_test_1( int a1, int a2 )
{
	TPoints	pA, pB;
	generate_points( pA, pB );

	TMatchingPairList list;
	generate_list_of_points( pA, pB, list );

	CPose3D out;
	double	scale;
	
	const size_t	N = 100;
	CTicTac			tictac;

	tictac.Tic();
	for (size_t i=0;i<N;i++)
		leastSquareErrorRigidTransformation6D( list, out, scale );

	const double T = tictac.Tac()/N;
	return T;
}

// ------------------------------------------------------
//				Benchmark: using CPose3DQuat
// ------------------------------------------------------
double scan_matching_test_2( int a1, int a2 )
{
	TPoints	pA, pB;
	generate_points( pA, pB );

	TMatchingPairList list;
	generate_list_of_points( pA, pB, list );

	CPose3DQuat out;
	double		scale;

	const size_t	N = 100;
	CTicTac			tictac;

	tictac.Tic();
	for (size_t i=0;i<N;i++)
		leastSquareErrorRigidTransformation6D( list, out, scale );

	const double T = tictac.Tac()/N;
	return T;
}

// ------------------------------------------------------
//				Benchmark: using vectors
// ------------------------------------------------------
double scan_matching_test_3( int a1, int a2 )
{
	TPoints	pA, pB;
	generate_points( pA, pB );

	vector_double inV;
	generate_vector_of_points( pA, pB, inV );

	THornMethodOpts opts;
	vector_double qu;

	const size_t	N = 100;
	CTicTac			tictac;

	tictac.Tic();
	for (size_t i=0;i<N;i++)
		HornMethod( inV, qu, opts );

	const double T = tictac.Tac()/N;
	return T;
}

// ------------------------------------------------------
// register_tests_scan_matching
// ------------------------------------------------------
void register_tests_scan_matching()
{
	lstTests.push_back( TestData("scan_matching: 6D LS Rigid Trans. [CPose3D]", scan_matching_test_1 ) );
	lstTests.push_back( TestData("scan_matching: 6D LS Rigid Trans. [CPose3DQuat]", scan_matching_test_2 ) );
	lstTests.push_back( TestData("scan_matching: 6D LS Rigid Trans. [vector of points]", scan_matching_test_3 ) );
}
