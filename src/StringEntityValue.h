//
//
//****************************************************************************************//
// Copyright (c) 2002-2014, The MITRE Corporation
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright notice, this list
//       of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright notice, this 
//       list of conditions and the following disclaimer in the documentation and/or other
//       materials provided with the distribution.
//     * Neither the name of The MITRE Corporation nor the names of its contributors may be
//       used to endorse or promote products derived from this software without specific 
//       prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
// SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
// OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
// TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//****************************************************************************************//

#ifndef STRINGENTITYVALUE_H
#define STRINGABSENTITYVALUE_H

#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMElement.hpp>

#include "AbsEntityValue.h"

/**
	This class represents a string entity value in an entity.  The datatype of an entity that has a string entity value should not be record as defined in the OVAL Language.
*/
class StringEntityValue : public AbsEntityValue{
public:

	/** Default constructor for a StringEntityValue. It initializes the value with an empty string.*/
	StringEntityValue();
	
    /** Single parameter constructor for a StringEntityValue that initializes its value with the passed in value.
     *  @param value A string that will be used to initialize the value of the entity.
     */
	StringEntityValue(std::string value);
	
    /** Destructor for a StringEntityValue.*/
	virtual ~StringEntityValue();

	/** Write this StringEntityValue as the value of the specified entity in the specified systems-characteristics file. 
	 *  @param scFile A pointer to a DOMDocument that specifies the system-characteristics file where the data should be written to.
	 *  @param entityElm A pointer to a DOMDocument that specifies the entity for which the StringEntityValue should be written to.
	 *  @return Void.
	 */
	virtual void Write(xercesc::DOMDocument* scFile, xercesc::DOMElement* entityElm);

    /** Parse the specified entity to retrieve its value.
     *  @param entityElm A pointer to a DOMElement from which the value should be retrieved.  This value should then be used to initialize the StringEntityValue. 
     *  @return Void.
     */
	virtual void Parse(xercesc::DOMElement* entityElm);
};

#endif
