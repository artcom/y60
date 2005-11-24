/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSnumeric: test_linearAlgebra.tst.cpp,v $
//
//   $Revision: 1.2 $
//
// Description: unit test for linear algebra
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
#ifndef INCL_LINALGEBRAUTILS
#define INCL_LINALGEBRAUTILS

#include "linearAlgebra.h"
#include "numeric_functions.h"
#include <asl/UnitTest.h>

#include <iostream>


using namespace std;
using namespace asl;


class linearAlgebraTestBase  : public UnitTest {
	public:
		linearAlgebraTestBase(const char * myName) :  UnitTest(myName) {}

		template<class T, class Number> void testFixedVectorBase(T & theProband, Number theTypeHint, 
                                                                 int theSize, const string & theProbandName) {
			// size test
			ENSURE_MSG(theProband.size() == theSize, string(theProbandName + "'s size.").c_str() ); 
		    
          
			// operator [] and == tests
			for (int i = 0; i < theSize; i++ ) {
				Number myValue = Number(5);
				theProband[i] = myValue;
				ENSURE_MSG(theProband[i] == myValue, string(theProbandName + "'s [] access and equal operator.").c_str() );                  
			}
		    
			// iterator interface test
			typename TripleOf<Number>::iterator theStart = theProband.begin();
			typename TripleOf<Number>::iterator theEnd   = theProband.end();
			int theCounter = 0; 
			for (; theStart != theEnd; theStart++) {
				ENSURE_MSG(*theStart == theProband[theCounter++], string(theProbandName + "'s iterators.").c_str() );                  
			}
		}

		template<class T> void runTripleTest(const T & myT, const string & theProbandName) {
						
			T myFirstTriple(1.0, 2.0, 3.0);
			T mySecondTriple(-1.f, -2.f, -3.f);

            bool equal = (myFirstTriple == T(1.0, 2.0, 3.0)); 
			ENSURE_MSG(myFirstTriple == T(1.0, 2.0, 3.0), string(theProbandName + "'s equal operator.").c_str() );
			ENSURE_MSG((myFirstTriple[0] == 1.0) && (myFirstTriple[1] == 2.0) && (myFirstTriple[2] == 3.0) &&
					(myFirstTriple == T(1.0f, 2.0f, 3.0f)), string(theProbandName + "'s equal operator.").c_str() );
			
			T myResultTriple(myFirstTriple); // copy ctor
			ENSURE_MSG(myResultTriple == myFirstTriple, string(theProbandName + "'s copy ctor.").c_str() );                  
			
			myResultTriple.add( mySecondTriple ); // add TripleOf
			ENSURE_MSG( myResultTriple == T(0.f, 0.f, 0.f), 
						string(theProbandName + "'s add a " + theProbandName + ".").c_str()  );                  

			myResultTriple.add( 1.5f ); // add number
			ENSURE_MSG(myResultTriple == T(1.5, 1.5, 1.5), string(theProbandName + "'s add a number.").c_str()  );                  
			
			myResultTriple.sub( 1.3f ); // sub number
			ENSURE_MSG( almostEqual(myResultTriple, T(0.2f, 0.2f, 0.2f)), 
						string(theProbandName + "'s almostEqual.").c_str()  );
			ENSURE_MSG( !almostEqual(myResultTriple, T(0.2f, 0.2f, 0.2f), 1E-15), 
						string(theProbandName + "'s almostEqual.").c_str()  ); 
		    
			myResultTriple.sub( T(0.3f, 0.4f, 0.5f )); // sub TripleOf
			ENSURE_MSG( almostEqual(myResultTriple, T(-0.1f, -0.2f, -0.3f)), 
						string(theProbandName + "'s sutract a " + theProbandName + ".").c_str() );                  
		                        
			myResultTriple.mult( -10.5f ); // mult number
			ENSURE_MSG( almostEqual(myResultTriple, T(1.05f, 2.1f, 3.15f)), 
						string(theProbandName + "'s divide by a " + theProbandName + ".").c_str()  );
			
			myResultTriple.div( 1.05f ); // div number
			ENSURE_MSG(almostEqual(myResultTriple, TripleOf<double>(1, 2, 3)), 
                string(theProbandName + "'s divide by a number.").c_str()  );
		    
			myResultTriple.negate();
			ENSURE_MSG(almostEqual(myResultTriple, mySecondTriple), string(theProbandName + "'s negate.").c_str() ); 

		}
		bool floatIsEqual(const float f1, const float f2, double thePrecision = 1E-6) {
			return (fabs(f1 - f2)) <= (f1 * thePrecision);
		}      
		bool floatIsEqual(const double f1, const double f2, double thePrecision = 1E-6) {
			return (fabs(f1 - f2)) <= (f1 * thePrecision);
		}      

};
#endif

