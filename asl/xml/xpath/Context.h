/**************************************************************************
 *                                                                        *
 *  This file is part of TaXTable, an XPath implementation for Y60.       *
 *                                                                        *
 *                                                                        *
 *  (C) 2005-2007  Tobias Anton <tobias.anton@web.de>                     *
 *                                                                        *
 *                                                                        *
 *  TaXTable is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation, either version 3 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 *  TaXTable is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *  GNU General Public License for more details.                          *
 *                                                                        *
 *  You should have received a copy of the GNU General Public License     *
 *  along with TaXTable.  If not, see <http://www.gnu.org/licenses/>.     *
 *                                                                        *
 **************************************************************************/

#ifndef XPATH_CONTEXT_H
#define XPATH_CONTEXT_H

#include "Value.h"

namespace xpath
{

  class Context
    {
    public:
	Context() { };
	Context(NodeRef n) { currentNode = n; position = 1; size=1; depth = 0;};
	Context(const Context &other) { size = other.size; position = other.position; currentNode = other.currentNode; size = other.size + 1; };

	// context position
	NodeRef currentNode;

	int position;

	int size;

	int depth;
    };

};

#endif

