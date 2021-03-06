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

#include "FileFinder.h"
#include <AutoCloser.h>
#include <PrivilegeGuard.h>
#include <WindowsCommon.h>

#include "FileAuditedPermissionsProbe.h"

using namespace std;

//****************************************************************************************//
//                              FileAuditedPermissionsProbe Class                         //
//****************************************************************************************//
FileAuditedPermissionsProbe* FileAuditedPermissionsProbe::instance = NULL;

FileAuditedPermissionsProbe::FileAuditedPermissionsProbe() {
}

FileAuditedPermissionsProbe::~FileAuditedPermissionsProbe() {
    instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* FileAuditedPermissionsProbe::Instance() {
    // Use lazy initialization
    if ( instance == NULL )
        instance = new FileAuditedPermissionsProbe();

    return instance;
}

ItemVector* FileAuditedPermissionsProbe::CollectItems ( Object* object ) {
    // Get the path, file name, and file path.
    ObjectEntity* path = object->GetElementByName ( "path" );
    ObjectEntity* fileName = object->GetElementByName ( "filename" );
    ObjectEntity* filePath = object->GetElementByName ( "filepath" );

    if ( filePath != NULL )
        throw ProbeException ( "Error: The filepath entity is not currently supported." );

    // Get trustee name.
    ObjectEntity* trusteeName = object->GetElementByName ( "trustee_name" );

    // Check trustee datatypes - only allow string.
    if ( trusteeName->GetDatatype() != OvalEnum::DATATYPE_STRING ) {
        throw ProbeException ( "Error: Invalid data type specified on trustee_name. Found: " + OvalEnum::DatatypeToString ( trusteeName->GetDatatype() ) );
    }

    // Check trustee operation - only allow  equals, not equals and pattern match.
    if ( trusteeName->GetOperation() != OvalEnum::OPERATION_EQUALS
            && trusteeName->GetOperation() != OvalEnum::OPERATION_PATTERN_MATCH
            && trusteeName->GetOperation() != OvalEnum::OPERATION_NOT_EQUAL ) {
        throw ProbeException ( "Error: Invalid operation specified on trustee_name. Found: " + OvalEnum::OperationToString ( trusteeName->GetOperation() ) );
    }

    ItemVector *collectedItems = new ItemVector();
    // Support behaviors - init with defaults.
    bool includeGroupBehavior = true;
    bool resolveGroupBehavior = false;

    if ( object->GetBehaviors()->size() != 0 ) {
        BehaviorVector* behaviors = object->GetBehaviors();

        for ( BehaviorVector::iterator iterator = behaviors->begin(); iterator != behaviors->end(); iterator++ ) {
            Behavior* behavior = ( *iterator );

            if ( behavior->GetName().compare ( "include_group" ) == 0 )  {
                if ( behavior->GetValue().compare ( "false" ) == 0 ) {
                    includeGroupBehavior = false;
                }
				Log::Info("Deprecated behavior found when collecting " + object->GetId() + " Found behavior: " + behavior->GetName() + " = " + behavior->GetValue());
            } else if ( behavior->GetName().compare ( "resolve_group" ) == 0 ) {
                if ( behavior->GetValue().compare ( "true" ) == 0 ) {
                    resolveGroupBehavior = true;
                }

                Log::Info ( "Deprecated behavior found when collecting " + object->GetId() + ". Found behavior: " + behavior->GetName() + " = " + behavior->GetValue() );

            } else if ( behavior->GetName() == "max_depth" || behavior->GetName() == "recurse_direction" || behavior->GetName() == "windows_view" ) {
                // Skip these as they are supported in the file finder class.
            } else {
                Log::Info ( "Unsupported behavior found when collecting " + object->GetId() + ". Found behavior: " + behavior->GetName() + " = " + behavior->GetValue() );
            }
        }
    }

	FileFinder fileFinder(WindowsCommon::behavior2view(object->GetBehaviors()));
	StringPairVector *filePaths = NULL;

	{
		PrivilegeGuard pg(SE_BACKUP_NAME, false);
		filePaths = fileFinder.SearchFiles(path, fileName, object->GetBehaviors());
	}

    if ( filePaths->size() > 0 ) {
        // Loop through all file paths.
        for ( StringPairVector::iterator iterator = filePaths->begin(); iterator != filePaths->end(); iterator++ ) {
            StringPair* fp = ( *iterator );

            // If there is no file name and the fileName ObjectEntity is not set to nil.
            if ( fp->second.compare ( "" ) == 0 && !fileName->GetNil() ) {
                Item* item = NULL;
                // Check if the code should report that the filename does not exist.

                if ( fileFinder.ReportFileNameDoesNotExist ( fp->first, fileName ) ) {
                    item = this->CreateItem();
                    item->SetStatus ( OvalEnum::STATUS_DOES_NOT_EXIST );
                    item->AppendElement ( new ItemEntity ( "path", fp->first, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
                    item->AppendElement ( new ItemEntity ( "filename", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST, false ) );
					item->AppendElement(new ItemEntity("windows_view",
						(fileFinder.GetView() == BIT_32 ? "32_bit" : "64_bit")));
                    collectedItems->push_back ( item );

                } else {
                    item = this->CreateItem();
                    item->SetStatus ( OvalEnum::STATUS_EXISTS );
                    item->AppendElement ( new ItemEntity ( "path", fp->first, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
					item->AppendElement(new ItemEntity("windows_view",
						(fileFinder.GetView() == BIT_32 ? "32_bit" : "64_bit")));
                    collectedItems->push_back ( item );
                }

            } else {
                try {
					// The file exists so lets get the trustees and then examine their audited permissions.
					string filePathStr = Common::BuildFilePath(fp->first, fp->second);
					HANDLE fileHandle;

					// The SE_SECURITY_NAME privilege is needed to read the SACL
					// (for ACCESS_SYSTEM_SECURITY access, specified below.)
					{
						PrivilegeGuard pg(SE_SECURITY_NAME);

						fileHandle = fileFinder.GetFileHandle(filePathStr,
							READ_CONTROL|ACCESS_SYSTEM_SECURITY, 
							fileName && fileName->GetNil());

						pg.disable();
					}

					if (!fileHandle || fileHandle == INVALID_HANDLE_VALUE) {
						throw ProbeException("Error: unable to get trustees "
							"for file: Error opening file: " + 
							WindowsCommon::GetErrorMessage(GetLastError()));
					}

					// need the guard, cause I know at least GetTrusteesForWindowsObject()
					// throws, so I feel safer with it.
					AutoCloser<HANDLE, BOOL(WINAPI&)(HANDLE)> handleGuard(fileHandle, 
						CloseHandle, "file "+filePathStr);

					StringSet trusteeNames = GetTrusteesForWindowsObject(
						SE_FILE_OBJECT, fileHandle, trusteeName, false, 
						resolveGroupBehavior, includeGroupBehavior);

                    if ( !trusteeNames.empty() ) {
                        for ( StringSet::iterator iterator = trusteeNames.begin(); iterator != trusteeNames.end(); iterator++ ) {
                            try {
                                Item* item = this->GetAuditedPermissions ( fileHandle, fp->first, fp->second, ( *iterator ) );

                                if ( item != NULL ) {
									if (fileName->GetNil()) {
										auto_ptr<ItemEntityVector> fileNameVector(item->GetElementsByName("filename"));
										if (fileNameVector->size() > 0) {
											fileNameVector->at(0)->SetNil(true);
											fileNameVector->at(0)->SetStatus(OvalEnum::STATUS_NOT_COLLECTED);
										}
									}
									item->AppendElement(new ItemEntity("windows_view",
										(fileFinder.GetView() == BIT_32 ? "32_bit" : "64_bit")));
                                    collectedItems->push_back ( item );
                                }

                            } catch ( ProbeException ex ) {
                                Log::Message ( "ProbeException caught when collecting: " + object->GetId() + " " +  ex.GetErrorMessage() );

                            } catch ( Exception ex ) {
                                Log::Message ( "Exception caught when collecting: " + object->GetId() + " " +  ex.GetErrorMessage() );
                            }
                        }
                    } else {
                        Log::Debug ( "No matching trustees found when getting audited permissions for object: " + object->GetId() );

                        if ( this->ReportTrusteeDoesNotExist ( trusteeName, false ) ) {
                            Item* item = this->CreateItem();
                            item->SetStatus ( OvalEnum::STATUS_DOES_NOT_EXIST );
                            item->AppendElement ( new ItemEntity ( "path", fp->first, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
                            item->AppendElement (new ItemEntity ("filename", fp->second, OvalEnum::DATATYPE_STRING, ((fileName->GetNil())?OvalEnum::STATUS_NOT_COLLECTED : OvalEnum::STATUS_EXISTS), fileName->GetNil() ) );
                            item->AppendElement ( new ItemEntity ( "trustee_name", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST ) );
							item->AppendElement(new ItemEntity("windows_view",
								(fileFinder.GetView() == BIT_32 ? "32_bit" : "64_bit")));
                            collectedItems->push_back ( item );
                        }
                    }

                } catch ( ProbeException ex ) {
                    Log::Message ( "ProbeException caught when collecting: " + object->GetId() + " " +  ex.GetErrorMessage() );

                } catch ( Exception ex ) {
                    Log::Message ( "Exception caught when collecting: " + object->GetId() + " " +  ex.GetErrorMessage() );

                } catch ( ... ) {
                    Log::Message ( "Unknown error when collecting " + object->GetId() );
                }
            }

            delete fp;
            fp = NULL;
        }

    } else {
        // If there are no file paths check to see if the code should report that the path does not exist.
        if ( fileFinder.ReportPathDoesNotExist ( path ) ) {
            Item* item = NULL;

            item = this->CreateItem();
            item->SetStatus ( OvalEnum::STATUS_DOES_NOT_EXIST );
            item->AppendElement ( new ItemEntity ( "path", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST ) );
			item->AppendElement(new ItemEntity("windows_view",
				(fileFinder.GetView() == BIT_32 ? "32_bit" : "64_bit")));
            collectedItems->push_back ( item );
        }
    }

    filePaths->clear();
    delete filePaths;
    filePaths = NULL;
    return collectedItems;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
Item* FileAuditedPermissionsProbe::CreateItem() {
    Item* item = new Item ( 0,
                            "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows",
                            "win-sc",
                            "http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows windows-system-characteristics-schema.xsd",
                            OvalEnum::STATUS_ERROR,
                            "fileauditedpermissions_item" );
    return item;
}

Item* FileAuditedPermissionsProbe::GetAuditedPermissions ( HANDLE fileHandle, string path, string fileName, string trusteeName ) {
    Item* item = NULL;
    PSID pSid = NULL;
    PACCESS_MASK pSuccessfulAuditedPermissions = NULL;
    PACCESS_MASK pFailedAuditPermissions = NULL;
    // Build the path.
    string filePath = Common::BuildFilePath ( path, fileName );
    string baseErrMsg = "Error: Unable to get audited permissions for trustee: " + trusteeName + " from dacl for file: " + filePath;

    try {

        // Get the sid for the trustee name.
        pSid = WindowsCommon::GetSIDForTrusteeName ( trusteeName );
        // The file exists and trustee name seems good so we can create the new item now.
        item = this->CreateItem();
        item->SetStatus ( OvalEnum::STATUS_EXISTS );
        item->AppendElement ( new ItemEntity ( "path", path, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "filename", fileName, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "trustee_name", trusteeName, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
        // Build a structure to hold the successful audited rights.
        pSuccessfulAuditedPermissions = reinterpret_cast<PACCESS_MASK> ( ::LocalAlloc ( LPTR, sizeof ( PACCESS_MASK ) + sizeof ( ACCESS_MASK ) ) );

        if ( pSuccessfulAuditedPermissions == NULL ) {
            if ( pSid != NULL ) {
                free ( pSid );
                pSid = NULL;
            }

            throw ProbeException ( baseErrMsg + ". Out of memory! Unable to allocate memory for successful audited rights." );
        }

        // Build a structure to hold the failed audit rights.
        pFailedAuditPermissions = reinterpret_cast<PACCESS_MASK> ( ::LocalAlloc ( LPTR, sizeof ( PACCESS_MASK ) + sizeof ( ACCESS_MASK ) ) );

        if ( pFailedAuditPermissions == NULL ) {
            if ( pSid != NULL ) {
                free ( pSid );
                pSid = NULL;
            }

            if ( pSuccessfulAuditedPermissions != NULL ) {
                LocalFree ( pSuccessfulAuditedPermissions );
                pSuccessfulAuditedPermissions = NULL;
            }

            throw ProbeException ( baseErrMsg + ". Out of memory! Unable to allocate memory for failed audit rights." );
        }

        // Get the audited rights.
        Log::Debug ( "Getting audited permissions masks for file: " + path + " filename: " + fileName + " trustee_name: " + trusteeName );
        WindowsCommon::GetAuditedPermissionsForWindowsObject ( SE_FILE_OBJECT, pSid, fileHandle, pSuccessfulAuditedPermissions, pFailedAuditPermissions );
        item->AppendElement ( new ItemEntity ( "standard_delete", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedPermissions ) & DELETE ), ( ( *pFailedAuditPermissions ) & DELETE ) ), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "standard_read_control", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedPermissions ) & READ_CONTROL ), ( ( *pFailedAuditPermissions ) & READ_CONTROL ) ), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "standard_write_dac", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedPermissions ) & WRITE_DAC ), ( ( *pFailedAuditPermissions ) & WRITE_DAC ) ), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "standard_write_owner", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedPermissions ) & WRITE_OWNER ), ( ( *pFailedAuditPermissions ) & WRITE_OWNER ) ), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "standard_synchronize", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedPermissions ) & SYNCHRONIZE ), ( ( *pFailedAuditPermissions ) & SYNCHRONIZE ) ), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "access_system_security", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedPermissions ) & ACCESS_SYSTEM_SECURITY ), ( ( *pFailedAuditPermissions ) & ACCESS_SYSTEM_SECURITY ) ), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "generic_read", ConvertPermissionsToStringValue ( ( ( ( *pSuccessfulAuditedPermissions | SYNCHRONIZE ) & FILE_GENERIC_READ ) == FILE_GENERIC_READ ), ( ( ( *pFailedAuditPermissions | SYNCHRONIZE ) & FILE_GENERIC_READ ) == FILE_GENERIC_READ ) ), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "generic_write", ConvertPermissionsToStringValue ( ( ( ( *pSuccessfulAuditedPermissions ) & FILE_GENERIC_WRITE ) == FILE_GENERIC_WRITE ), ( ( ( *pFailedAuditPermissions ) & FILE_GENERIC_WRITE ) == FILE_GENERIC_WRITE ) ), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "generic_execute", ConvertPermissionsToStringValue ( ( ( ( *pSuccessfulAuditedPermissions ) & FILE_GENERIC_EXECUTE ) == FILE_GENERIC_EXECUTE ), ( ( ( *pFailedAuditPermissions ) & FILE_GENERIC_EXECUTE ) == FILE_GENERIC_EXECUTE ) ), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "generic_all", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedPermissions ) & FILE_READ_DATA ), ( ( *pFailedAuditPermissions ) & FILE_READ_DATA ) ), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "file_read_data", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedPermissions ) & FILE_READ_DATA ), ( ( *pFailedAuditPermissions ) & FILE_READ_DATA ) ), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "file_write_data", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedPermissions ) & FILE_WRITE_DATA ), ( ( *pFailedAuditPermissions ) & FILE_WRITE_DATA ) ), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "file_append_data", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedPermissions ) & FILE_APPEND_DATA ), ( ( *pFailedAuditPermissions ) & FILE_APPEND_DATA ) ), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "file_read_ea", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedPermissions ) & FILE_READ_EA ), ( ( *pFailedAuditPermissions ) & FILE_READ_EA ) ), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "file_write_ea", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedPermissions ) & FILE_WRITE_EA ), ( ( *pFailedAuditPermissions ) & FILE_WRITE_EA ) ), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "file_execute", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedPermissions ) & FILE_EXECUTE ), ( ( *pFailedAuditPermissions ) & FILE_EXECUTE ) ), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "file_delete_child", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedPermissions ) & FILE_DELETE_CHILD ), ( ( *pFailedAuditPermissions ) & FILE_DELETE_CHILD ) ), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "file_read_attributes", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedPermissions ) & FILE_READ_ATTRIBUTES ), ( ( *pFailedAuditPermissions ) & FILE_READ_ATTRIBUTES ) ), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );
        item->AppendElement ( new ItemEntity ( "file_write_attributes", ConvertPermissionsToStringValue ( ( ( *pSuccessfulAuditedPermissions ) & FILE_WRITE_ATTRIBUTES ), ( ( *pFailedAuditPermissions ) & FILE_WRITE_ATTRIBUTES ) ), OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS ) );

    } catch ( Exception ex ) {
        if ( item != NULL ) {
            item->SetStatus ( OvalEnum::STATUS_ERROR );
            item->AppendMessage ( new OvalMessage ( ex.GetErrorMessage(), OvalEnum::LEVEL_ERROR ) );

        } else {
            if ( pSuccessfulAuditedPermissions != NULL ) {
                LocalFree ( pSuccessfulAuditedPermissions );
                pSuccessfulAuditedPermissions = NULL;
            }

            if ( pFailedAuditPermissions != NULL ) {
                LocalFree ( pFailedAuditPermissions );
                pFailedAuditPermissions = NULL;
            }

            if ( pSid != NULL ) {
                free ( pSid );
                pSid = NULL;
            }

            throw ex;
        }
    }

    if ( pSuccessfulAuditedPermissions != NULL ) {
        LocalFree ( pSuccessfulAuditedPermissions );
        pSuccessfulAuditedPermissions = NULL;
    }

    if ( pFailedAuditPermissions != NULL ) {
        LocalFree ( pFailedAuditPermissions );
        pFailedAuditPermissions = NULL;
    }

    if ( pSid != NULL ) {
        free ( pSid );
        pSid = NULL;
    }

    return item;
}

string FileAuditedPermissionsProbe::ConvertPermissionsToStringValue ( ACCESS_MASK success , ACCESS_MASK failure ) {
    if ( success && failure ) return "AUDIT_SUCCESS_FAILURE";

    else if ( success && !failure ) return "AUDIT_SUCCESS";

    else if ( !success && failure ) return "AUDIT_FAILURE";

    else if ( !success && !failure ) return "AUDIT_NONE";

    else return "";
}