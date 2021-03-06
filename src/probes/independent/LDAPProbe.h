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
#ifndef LDAPPROBE_H
#define LDAPPROBE_H

#ifdef WIN32
#include "Winldap.h"
#endif

#if defined LINUX || defined SUNOS || defined DARWIN
#include "ldap.h"
typedef int ULONG;
#endif

#include "AbsProbe.h"

/**
	This class is responsible for collecting ldap information for ldap_objects.
	This class should be platform independant.
*/

/** A map for storing attributes and their corresponding datatypes. */
typedef std::map < std::string , std::string > TypeMap;

/** A map for storing schemas and their TypeMaps that contain their attributes and corresponding datatypes. */
typedef std::map < std::string , TypeMap* > TypeMapMap;

class LDAPProbe : public AbsProbe {

	public:

		/** LDAPProbe destructor */
		virtual ~LDAPProbe();

		/** Run the LDAP probe */
		virtual ItemVector* CollectItems ( Object* object );

		/** Ensure that the LDAPProbe is a singleton. */
		static AbsProbe* Instance();

	private:

		/** LDAPProbe constructor */
		LDAPProbe();

		/** Return a new Item created for storing LDAP information */
		virtual Item* CreateItem();

		/** Open a connection to the LDAP server with the specified hostname.
		 *  @param hostnameStr A string that represents the hostname of the LDAP server.
		 *  @return A LDAP structure for accessing the LDAP server.
		 */
		LDAP* OpenConnection ( std::string hostname );

		/** Close the connection to the LDAP server.
		 *  @param ldap The LDAP structure that is used for accessing the LDAP server.
		 *  @return Void.
		 */
		void CloseConnection ( LDAP* ldap );

		/** Get the set of all suffixes on the system that match the object.
		 *  @param suffixEntity A ObjectEntity that represents the suffix entity in an Object as defined in the OVAL Definition Schema.
		 *  @return A StringSet containing all of the suffixes specified in the ObjectEntity.
		 */
		StringSet* GetSuffixes ( ObjectEntity* suffixEntity );

		/** Retrieve all of the suffixes.
		 *  @return A StringSet containing all of the matching suffixes.
		 */
		StringSet* GetAllSuffixes ( );

		/** Determine if the specified suffix exists.
		 *  @param suffixStr A string that contains the suffix of the LDAP item.
		 *  @return A boolean value indicating whether or not a LDAP item with the specified suffix exists.
		 */
		bool SuffixExists ( std::string suffixStr );

		/** Get the set of all relative distinguished names on the system that match the object.
		 *  @param suffixStr A string that contains the suffix to be used during the matching process.
		 *  @param relativeDnEntity A ObjectEntity that represents the relative distinguished name entity in an Object as defined in the OVAL Definition Schema.
		 *  @param scopeBehavior A integer value that specifies the scope from which to search for relative distinguished names.
		 *  @return A StringSet that contains all of the relative distinguished names specified in the ObjectEntity that also match the specified suffix.
		 */
		StringSet* GetRelativeDns ( std::string suffixStr, ObjectEntity* relativeDnEntity, int scopeBehavior );

		/** Deterimine if the specified relative distinguished name exists.
		 *  @param suffixStr A string that contains the suffix of the LDAP item.
		 *  @param relativeDnStr A string that contains the relative distinguished name of the LDAP item.
		 *  @return A boolean value indicating whether or not a LDAP item with the specified suffix and relative distinguished name exists.
		 */
		bool RelativeDnExists ( std::string suffixStr, std::string relativeDnStr );

		/** Get the set of all LDAP attributes on the system that match the object.
		 *  @param suffixStr A string that contains the suffix to be used during the matching process.
		 *  @param relativeDnStr A string that contains the relative distinguished name to be used during the matching process.
		 *  @param attributeEntity A ObjectEntity that represents the attribute entity in an Object as defined in the OVAL Definition Schema.
		 *  @return A StringSet that contains all of the attributes specified in the ObjectEntity that also match the specified suffix and relative distinguished name.
		 */
		StringSet* GetAttributes ( std::string suffixStr, std::string relativeDnStr, ObjectEntity* attributeEntity );

		/** Deterimine if the specified attribute exists.
		 *  @param suffixStr A string that contains the suffix of the LDAP item.
		 *  @param relativeDnStr A string that contains the relative distinguished name of the LDAP item.
		 *  @param attributeStr A string that contains the attribute of the LDAP item.
		 *  @return A boolean value indicating whether or not a LDAP item with the specified suffix, relative distinguished name, and attribute exists.
		 */
		bool AttributeExists ( std::string suffixStr, std::string relativeDnStr, std::string attributeStr );

		/** Build a distinguished name from its suffix and relative distinguished name components.
		 *  @param suffixStr A string that contains the suffix of the LDAP item.
		 *  @param relativeDnStr A string that contains the relative distinguished name of the LDAP item.
		 *  @return A string value that represents the distinguished name of the specified suffix and relative distinguished name components.
		 */
		std::string BuildDistinguishedName ( std::string suffixStr, std::string relativeDnStr );

		/** Retrieve the relative distinguished name component of the specified distinguished name.
		 *  @param suffixStr A string that contains the suffix of the LDAP item.
		 *  @param distinguishedNameStr A string that contains the distinguished name of the LDAP item.
		 *  @return A string value that represents the relative distinguished name of the specified distinguished name.
		 */
		std::string RemoveDnBase ( std::string suffixStr, std::string distinguishedNameStr );

		/** Retrieve the object class of the specified LDAP item exists.
		 *  @param suffixStr A string that contains the suffix of the LDAP item.
		 *  @param relativeDnStr A string that contains the relative distinguished name of the LDAP item.
		 *  @return A string value representing the object class of the LDAP item with the specified suffix and relative distinguished name.
		 */
		std::string GetObjectClass ( std::string suffixStr, std::string relativeDnStr );

		/** Retrieve the location of the LDAP server. For Windows, this method retrieves the location using DsGetDcName() and for UNIX\Linux it retrieves the first entry from '/etc/ldap.conf'.
		 * @return A string value that represents the location of the LDAP server.
		 */
		std::string GetLDAPServerLocation();

		/** Retrieve all of the attributes for the specified LDAP item.
		 *  @param suffixStr A string that contains the suffix of the LDAP item.
		 *  @param relativeDnStr A string that contains the relative distinguished name of the LDAP item.
		 *  @return A StringSet that contains all of the attributes for the LDAP item with the specified suffix and relative distinguished name.
		 */
		StringSet* GetAllAttributes ( std::string suffixStr, std::string relativeDnStr );

		/** Retrieve all of the distinguished names for the specified LDAP item and scope.
		 *  @param suffixStr A string that contains the suffix of the LDAP item.
		 *  @param relativeDnStr A string that contains the relative distinguished name of the LDAP item.
		 *  @param scopeBehavior A integer value that specifies the scope from which to search for distinguished names.
		 *  @return A StringSet that contains all of the distinguished names for the LDAP item with the specified suffix, relative distinguished name, and scope.
		 */
		StringSet* GetAllDistinguishedNames ( std::string suffixStr, std::string relativeDnStr, int scopeBehavior );

		/** Determine if the specified LDAP item exists.
		 *  @param suffixStr A string that contains the suffix of the LDAP item.
		 *  @param relativeDnStr A string that contains the relative distinguished name of the LDAP item.
		 *  @return A boolean value indicating whether or not the a LDAP item with the specified suffix and relative distinguished name exist.
		 */
		bool ObjectExists ( std::string suffixStr, std::string relativeDnStr );

		/** Append the a ldaptype ItemEntity, and any value ItemEntities, to the specified Item if they exist.
		 *  @param suffixStr A string that contains the suffix of the LDAP item.
		 *  @param relativeDnStr A string that contains the relative distinguished name of the LDAP item.
		 *  @param attributeStr A string that contains the attribute of the LDAP item.
		 *  @param item A Item that will have ldaptype and value ItemEntities appended to it.
		 *  @return Void.
		 */
		void GetLdapItem ( std::string suffixStr, std::string relativeDnStr, std::string attributeStr, Item* item );

		/** Retrieve all of the attribute value types specified in the subschemasubentry attribute of the RootDSE entry.
		 * @return a TypeMapMap containing all of the individual subschemas and their corresponding attributes and LDAP datatypes.
		 */
		TypeMapMap* GetAttributeValueTypes();

		/** Convert the specified OID into its corresponding LDAP datatype.
		 * @param oid A string values that represents the OID that you would like to convert into an LDAP datatype.
		 * @return A string value that represents the LDAP datatype.
		 */
		std::string GetDataType ( std::string oid );

		/** Retrieve all of the values for the specified suffix, relative distinguished name, and attribute.
		 *  @param suffixStr A string that contains the suffix of the LDAP item.
		 *  @param relativeDnStr A string that contains the relative distinguished name of the LDAP item.
		 *  @param attributeStr A string that contains the attribute of the LDAP item.
		 * @return A StringVector containg all of the values for the specified suffix, relative distinguished name, and attribute.
		 */
		StringVector* GetValues ( std::string suffixStr, std::string relativeDnStr, std::string attributeStr, bool type = 0 );

		/** Converts a sequence of characters that represents binary data into a string of hex-encoded data.  Please see the binary datatype in DatatypeEnumeration in the oval-common-schema for more information.
		 *	@param data The binary data represented as a sequence of characters.
		 *	@param length The length of the data.
		 *	@return The string value of the data in binary form as described in the documentation for the binary datatype in DatatypeEnumeration in the oval-common-schema. 
		 */
		std::string ConvertToBinary(char* data, unsigned long length);

		/** Remove quotes from the specified string.
		 * @param str A string value that you want to remove the quotes from.
		 * @return A string value of the specified string with any beginning and ending quotes removed.
		 */
		std::string RemoveQuotes ( std::string str );

		/** Retrieve the error message for the specified error code.
		 * @param error A ULONG error value that you want the error message for.
		 * @return A string representation of the specified error code.
		 */
		std::string GetErrorMessage ( ULONG error );

		/** Determine if there are any suffixes that do not exist that need to be reported.
		 *  @param suffixEntity A ObjectEntity that represents the suffix entity in an Object as defined in the OVAL Definition Schema.
		 *  @param suffixesDne A StringVector that will be used to contain all of the suffixes specified in the ObjectEntity that do not exist on the system.
		 *  @return A boolean value that indicates whether or not there are relative distinguished names that do not exist that need to be reported.
		 */
		bool ReportSuffixDoesNotExist ( ObjectEntity *suffixEntity, StringVector* suffixesDne );

		/** Determine if there are any relative distinguished names that do not exist for the specified suffix that need to be reported.
		 *  @param suffixStr A string that contains the suffix of the LDAP item.
		 *  @param relativeDnEntity A ObjectEntity that represents the relative distinguished name entity in an Object as defined in the OVAL Definition Schema.
		 *  @param relativednsDne A StringVector that will be used to contain all of the relative distinguished names specified in the ObjectEntity that do not exist on the system.
		 *  @return A boolean value that indicates whether or not there are relative distinguished names that do not exist that need to be reported.
		 */
		bool ReportRelativeDnDoesNotExist ( std::string suffixStr, ObjectEntity* relativeDnEntity, StringVector* relativeDnsDne );

		/** Determine if there are any attributes that do not exist for the specified suffix and relative distinguished name that need to be reported.
		 *  @param suffixStr A string that contains the suffix of the LDAP item.
		 *  @param relativeDnStr A string that contains the relative distinguished name of the LDAP item.
		 *  @param attributeEntity A ObjectEntity that represents the attribute entity in an Object as defined in the OVAL Definition Schema.
		 *  @param attributesDne A StringVector that will be used to contain all of the attributes specified in the ObjectEntity that do not exist on the system.
		 *  @return A boolean value that indicates whether or not there are attributes that do not exist that need to be reported.
		 */
		bool ReportAttributeDoesNotExist ( std::string suffixStr, std::string relativeDnStr, ObjectEntity *attributeEntity, StringVector *attributesDne );

		/** Delete all of the stored types.
		 *  @return Void.
		 */
		void DeleteAllTypes();

		/** The static instance of the LDAPProbe.
		 *  All Probes are singletons. The ProbeFactory is responsible for managing instances of Probes.
		 */
		static LDAPProbe *instance;

		/** A string that represents the BASE scope. */
		static const std::string SCOPE_BASE;

		/** A string that represents the ONE scope. */
		static const std::string SCOPE_ONE;

		/** A string that represents the SUBTREE scope. */
		static const std::string SCOPE_SUBTREE;

		/** A string that represents the namingcontexts attribute. */
		static const std::string NAMING_CONTEXTS_ATTRIBUTE;

		/** A string that represents the subschemasubentry attribute. */
		static const std::string SUBSCHEMA_SUBENTRY_ATTRIBUTE;

		/** A string that represents the attributetypes attribute. */
		static const std::string ATTRIBUTE_TYPES_ATTRIBUTE;

		/** A string that represents the objectclass attribute. */
		static const std::string OBJECT_CLASS_ATTRIBUTE;

		/** A map of maps for storing multiple schemas and there associated attribute types and attribute type datatypes. */
		TypeMapMap* types;

		/** A LDAP structure for accessing the LDAP server. */
		LDAP* ldap;

};

#endif
