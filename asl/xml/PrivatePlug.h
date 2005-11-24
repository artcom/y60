/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2003, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//   $RCSfile: PrivatePlug.h,v $
//
//   $Revision: 1.2 $
//
//   Description: 
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _xml_PrivatePlug_h_
#define _xml_PrivatePlug_h_

namespace dom {    

    /*! \addtogroup aslxml */
    /* @{ */

    template<class VALUE>
    class PrivatePlug {
        public:
            PrivatePlug() {}
            
            VALUE & getValue() {
                return _myValue;
            }
        
            const VALUE & getValue() const {
                return _myValue;
            }

            void setDirty(bool theDirtyFlag = true) {
                _myDirtyFlag = theDirtyFlag;
            }
            const bool isDirty() const {
                return _myDirtyFlag;
            }
        private:
            VALUE _myValue;
            bool  _myDirtyFlag;
    };        

    /* @} */
}

#endif
