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

#ifndef ITEM_H
#define ITEM_H

#include <string>
#include <vector>
#include <map>
#include <utility>
#include <functional>

#include "ItemEntity.h"
#include "OvalMessage.h"

class Item;

/**
	A vector for storing Item objects. 
	Stores only pointers to the objects. 
*/
typedef std::vector < Item* > ItemVector;

/**	
	A pair for storing item ids and Item objects together. 
	Stores only pointers to the objects. 
*/
typedef std::pair < int, Item* > ItemPair;

/**	
	A map for storing ItemPairs. 
	Stores only pointers to the Item objects. 
*/
typedef std::map < int, Item* > ItemMap;

/**	
	A pair for storing item strings and Item objects together. 
	Stores only pointers to the objects. 
*/
typedef std::pair < std::string, Item* > StringItemPair;

/**	
	A map for storing Items keyed off of each Item's string value.
	Stores only pointers to the objects. 
*/
typedef std::map < std::string, Item* > StringKeyedItemMap;

/**	
	A pair of an StringKeyedItemMap::iterator and a bool.
*/
typedef std::pair < StringKeyedItemMap::iterator, bool > ItemCacheResult;

namespace std {
	/**
	 * Specialization of std::less to allow us to
	 * put Items into sets.
	 */
	template<> 
	struct less<const Item*> : public binary_function<const Item*, const Item*, bool> {
		bool operator()(const Item *left, const Item *right);
	};
}

/**
	This class represents an Item in a system characteristics document.

	Items are created and populated by probes during data collection. During
	data collection Items must be unique. The caching methods provided in this 
	class should be utilized to ensure uniqueness.  This class also provides 
	static methods for assigning Item ids.

	Items are also created during analysis by parsing them from a system-characteristics 
	document.
      
*/
class Item {
public:
	/** Create a complete Item object. */
	Item(int id = 0, std::string xmlns = "", std::string xmlnsAlias = "", std::string schemaLocation = "", OvalEnum::SCStatus status = OvalEnum::STATUS_EXISTS, std::string name = "");
	
    /** Item copy constructor */
    Item(const Item& item);

	/** Clean up all the elements. */
	~Item();

	/** Assign a new Item Id. */
	static int AssignId();

	/** Return all elements with the specified name.
		@return Returns a vector of elements with a matching name. If no mathes are found the vector is empty. The caller should delete the returned vector but not its contents.
	*/
	ItemEntityVector* GetElementsByName(std::string elementName);

    /** Return the ItemEntity with the specified name. If the Item contains multiple ItemEntities with the specified name, this method will indicate that an error has occurred. Note that this method should only be used with ItemEntities that can occur at most once in an Item.
        @param itemEntityNameStr A string representing the name of the ItemEntity that you would like to retrieve.
        @return Returns an ItemEntity with the specified name. If no matches are found an empty ItemEntity object is returned. If multiple ItemEntities are found an exception is thrown.  
    */
    ItemEntity* GetElementByName(std::string itemEntityNameStr);

	/** Parse the provided item element from an sc file into an Item object. */
	void Parse(xercesc::DOMElement* scItemElm);

	/** Write this item to a sc file. 
		Make sure the item has not already been written to the sc file.
	*/
	void Write(xercesc::DOMDocument* scFile, xercesc::DOMElement* itemsElm);

    /** Get the elements field's value. */
	ItemEntityVector* GetElements();
    /** Get the elements field's value. */
	const ItemEntityVector* GetElements() const { return &elements; }
	/** Get the number of entities in this item. */
	size_t GetNumElements() const { return elements.size(); }
    /** Set the elements field's value. */
	void SetElements(ItemEntityVector* elements);

    /** Get the id field's value. */
	int GetId();
    /** Set the id field's value. */
	void SetId(int id);

    /** Get the isWritten field's value. */
	bool GetIsWritten();
    /** Set the isWritten field's value. */
	void SetIsWritten(bool isWritten);
    
    /** Get the messages field's value. */
	OvalMessageVector* GetMessages();
    /** Set the messages field's value. */
	void SetMessages(OvalMessageVector* messages);

    /** Get the name field's value. */
	std::string GetName() const;
    /** Set the name field's value. */
	void SetName(std::string name);

    /** Get the schemaLocation field's value. */
	std::string GetSchemaLocation();
    /** Set the schemaLocation field's value. */
	void SetSchemaLocation(std::string schemaLocation);

    /** Get the xmlns field's value. */
	std::string GetXmlns() const;
    /** Set the xmlns field's value. */
	void SetXmlns(std::string xmlns);

    /** Get the xmlnsAlias field's value. */
	std::string GetXmlnsAlias() const;
    /** Set the xmlnsAlias field's value. */
	void SetXmlnsAlias(std::string xmlnsAlias);

    /** Get the status field's value. */
	OvalEnum::SCStatus GetStatus() const;
	/** Set the status field's value. */
    void SetStatus(OvalEnum::SCStatus status);

    /** Add an element to the end of the elements for this item. */
	void AppendElement(ItemEntity* itemEntity);

    /** Add an message to the end of the messages for this item. */
	void AppendMessage(OvalMessage* msg);

    /** Return all object elements. */
	//ItemEntityVector* GetObjectElements();

	/** Delete all items in the cache. */
	static void ClearCache();

	/** Return an item object for the specified item id.
		First the cache of Items is checked. If the item is
		not found in the cache the item is looked up in the
		system-characteristics document and parsed. Once parsed 
		the new Item object is added to the cache.

		If the item is not found an exception is thrown. 

		
		NOTE: This method is not intended to be used during data collection.
	*/
	static Item* GetItemById(std::string itemId);

private:

	/** Delete all the elements in the item entity vector. */
	void DeleteElements();

	/** Cache the specified item. */
	static void Cache(Item* item);

	/** Search the cache of Items for the specified Item.
        @param id The id of the item to retrieve from the cache.
		@return Returns the item with the specified id or NULL if not found.
	*/
	static Item* SearchCache(int id);

	ItemEntityVector elements;
	int id;
	OvalMessageVector messages;
	std::string name;
	OvalEnum::SCStatus status;
	std::string xmlns;
	std::string xmlnsAlias;
	std::string schemaLocation;
	bool isWritten;

    /** A static counter used to assign unique ids to each item. */
	static int idCounter;

	static ItemMap processedItemsMap;
};

#endif
