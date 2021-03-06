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

#ifndef COLLECTEDSET_H
#define COLLECTEDSET_H

#include <string>
#include <vector>

#include "Item.h"
#include "VariableValue.h"
#include "Exception.h"
#include "OvalEnum.h"

/**
	This class represents an CollectedSet in an oval definition schema.
	CollectedSets are used by set objects to construct complex sets of items on a sytem for analysis.
*/
class CollectedSet {

public:
	/** Initialize the collected set. */
	CollectedSet() : flag(OvalEnum::FLAG_ERROR)
	{}
	~CollectedSet()
	{}

	/** Return the items field's value. */
	const ItemVector* GetItems() const {
		return &this->items;
	}
	/** Set the items field's value. */
	void SetItems(ItemVector* items) {
		this->items = (*items);
	}
	
	/** Return the variableValues field's value. */
	VariableValueVector GetVariableValues() const {
		return variableValues;
	}
	/** Set the variableValues field's value. */
	void SetVariableValues(const VariableValueVector &variableValues) {
		this->variableValues = variableValues;
	}
	
	/** Add a variable value to the end of the variable values vector. */
	void AppendVariableValue(const VariableValue &variableValue) {
		variableValues.push_back(variableValue);
	}
	/** Add a vector of variable values to the end of the variable values vector. */
	void AppendVariableValues(const VariableValueVector &variableValues);
    
	/** Return the flag field's value. */
	OvalEnum::Flag GetFlag() const {
		return this->flag;
	}
	/** Set the flag field's value. */
	void SetFlag(OvalEnum::Flag flag) {
		this->flag = flag;
	}

private:
	ItemVector items;
	VariableValueVector variableValues;
	OvalEnum::Flag flag;
};

/** 
	This class represents an Exception that occured while processing a set.
*/
class CollectedSetException : public Exception {
	public:
		CollectedSetException(std::string errMsgIn = "", int severity = ERROR_FATAL, Exception *ex = NULL)
			: Exception(errMsgIn, severity, ex)
		{}
		~CollectedSetException()
		{}
};

#endif
