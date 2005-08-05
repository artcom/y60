//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: MaterialRequirementList.h,v $
//   $Author: janbo $
//   $Revision: 1.7 $
//   $Date: 2004/10/18 16:22:59 $
//
//  Description: A simple scene class.
//
//=============================================================================

#ifndef AC_Y60_MATERIAL_REQUIREMENT_LIST_INCLUDED
#define AC_Y60_MATERIAL_REQUIREMENT_LIST_INCLUDED

#include <dom/Nodes.h>
#include <y60/DataTypes.h>
#include <asl/Exception.h>

#include <string>
#include <map>
#include <iostream>

namespace y60 {

    DEFINE_EXCEPTION(MaterialRequirementListException, asl::Exception);

    typedef std::map<std::string, VectorOfRankedFeature> RequirementMap;

    class MaterialRequirementList {
        public:
            MaterialRequirementList();
            virtual ~MaterialRequirementList();

            const RequirementMap & getRequirements() const;
            RequirementMap & getRequirements();
            void load(const dom::NodePtr theNode);
        private:

            RequirementMap _myRequirementMap;
    };
} // namespace y60

namespace asl {
    inline
    std::ostream & operator<<(std::ostream & os, const y60::MaterialRequirementList & theList) {
        const y60::RequirementMap & theMap = theList.getRequirements();
        for (y60::RequirementMap::const_iterator it = theMap.begin(); it != theMap.end(); ++it) {
            os << it->first << " " << it->second << std::endl;
        }
        return os;
    }
}

#endif // AC_Y60_MATERIAL_REQUIREMENT_LIST_INCLUDED
