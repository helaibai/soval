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

#ifndef SUBSTRINGFUNCTION_H
#define SUBSTRINGFUNCTION_H

#include "AbsFunctionComponent.h"

/**
	This class represents a SubstringFunction component in a local_variable in the oval definition schema.
*/
class SubstringFunction : public AbsFunctionComponent {
public:

	/** Create a complete SubstringFunction object. */
	SubstringFunction(int start = 0, int length = 0)
		: start(start), length(length)
	{}
	virtual ~SubstringFunction()
	{}

	/** Parse the substring element and its child component elements. */
	virtual void Parse(xercesc::DOMElement* componentElm); 

	/** Compute the desired substring and return the values. */
	virtual ComponentValue* ComputeValue();

	/** Return the variable values used to compute this function's value. */
	virtual VariableValueVector GetVariableValues();

	/** Get the start field's value. */
	int GetStart() const {
		return this->start;
	}
	/** Set the start field's value. */
	void SetStart(int start) {
		this->start = start;
	}

	/** Get the length field's value. */
	int GetLength() const {
		return this->length;
	}
	/** Set the length field's value. */
	void SetLength(int length) {
		this->length = length;
	}

private:
	int start;
	int length;
};

#endif
