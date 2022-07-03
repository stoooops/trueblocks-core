/*-------------------------------------------------------------------------------------------
 * qblocks - fast, easily-accessible, fully-decentralized data from blockchains
 * copyright (c) 2016, 2021 TrueBlocks, LLC (http://trueblocks.io)
 *
 * This program is free software: you may redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version. This program is
 * distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details. You should have received a copy of the GNU General
 * Public License along with this program. If not, see http://www.gnu.org/licenses/.
 *-------------------------------------------------------------------------------------------*/
/*
 * Parts of this file were generated with makeClass --run. Edit only those parts of
 * the code inside of 'EXISTING_CODE' tags.
 */
#include "manifest.h"

namespace qblocks {

//---------------------------------------------------------------------------
IMPLEMENT_NODE(CManifest, CBaseNode);

//---------------------------------------------------------------------------
extern string_q nextManifestChunk(const string_q& fieldIn, const void* dataPtr);
static string_q nextManifestChunk_custom(const string_q& fieldIn, const void* dataPtr);

//---------------------------------------------------------------------------
void CManifest::Format(ostream& ctx, const string_q& fmtIn, void* dataPtr) const {
    if (!m_showing)
        return;

    // EXISTING_CODE
    // EXISTING_CODE

    string_q fmt = (fmtIn.empty() ? expContext().fmtMap["manifest_fmt"] : fmtIn);
    if (fmt.empty()) {
        if (expContext().exportFmt == YAML1) {
            toYaml(ctx);
        } else {
            toJson(ctx);
        }
        return;
    }

    // EXISTING_CODE
    // EXISTING_CODE

    while (!fmt.empty())
        ctx << getNextChunk(fmt, nextManifestChunk, this);
}

//---------------------------------------------------------------------------
string_q nextManifestChunk(const string_q& fieldIn, const void* dataPtr) {
    if (dataPtr)
        return reinterpret_cast<const CManifest*>(dataPtr)->getValueByName(fieldIn);

    // EXISTING_CODE
    // EXISTING_CODE

    return fldNotFound(fieldIn);
}

//---------------------------------------------------------------------------
string_q CManifest::getValueByName(const string_q& fieldName) const {
    // Give customized code a chance to override first
    string_q ret = nextManifestChunk_custom(fieldName, this);
    if (!ret.empty())
        return ret;

    // EXISTING_CODE
    // EXISTING_CODE

    // Return field values
    switch (tolower(fieldName[0])) {
        case 'c':
            if (fieldName % "chain") {
                return chain;
            }
            if (fieldName % "chunks" || fieldName % "chunksCnt") {
                size_t cnt = chunks.size();
                if (endsWith(toLower(fieldName), "cnt"))
                    return uint_2_Str(cnt);
                if (!cnt)
                    return "";
                string_q retS;
                for (size_t i = 0; i < cnt; i++) {
                    retS += chunks[i].Format();
                    retS += ((i < cnt - 1) ? ",\n" : "\n");
                }
                return retS;
            }
            break;
        case 'd':
            if (fieldName % "databases") {
                return databases;
            }
            break;
        case 's':
            if (fieldName % "schemas") {
                return schemas;
            }
            break;
        case 'v':
            if (fieldName % "version") {
                return version;
            }
            break;
        default:
            break;
    }

    // EXISTING_CODE
    // EXISTING_CODE

    // Finally, give the parent class a chance
    return CBaseNode::getValueByName(fieldName);
}

//---------------------------------------------------------------------------------------------------
bool CManifest::setValueByName(const string_q& fieldNameIn, const string_q& fieldValueIn) {
    string_q fieldName = fieldNameIn;
    string_q fieldValue = fieldValueIn;

    // EXISTING_CODE
    // EXISTING_CODE

    switch (tolower(fieldName[0])) {
        case 'c':
            if (fieldName % "chain") {
                chain = fieldValue;
                return true;
            }
            if (fieldName % "chunks") {
                CPinnedChunk obj;
                string_q str = fieldValue;
                while (obj.parseJson3(str)) {
                    chunks.push_back(obj);
                    obj = CPinnedChunk();  // reset
                }
                return true;
            }
            break;
        case 'd':
            if (fieldName % "databases") {
                databases = fieldValue;
                return true;
            }
            break;
        case 's':
            if (fieldName % "schemas") {
                schemas = fieldValue;
                return true;
            }
            break;
        case 'v':
            if (fieldName % "version") {
                version = fieldValue;
                return true;
            }
            break;
        default:
            break;
    }
    return false;
}

//---------------------------------------------------------------------------------------------------
void CManifest::finishParse() {
    // EXISTING_CODE
    // EXISTING_CODE
}

//---------------------------------------------------------------------------------------------------
bool CManifest::Serialize(CArchive& archive) {
    if (archive.isWriting())
        return SerializeC(archive);

    // Always read the base class (it will handle its own backLevels if any, then
    // read this object's back level (if any) or the current version.
    CBaseNode::Serialize(archive);
    if (readBackLevel(archive))
        return true;

    // EXISTING_CODE
    // EXISTING_CODE
    archive >> version;
    archive >> chain;
    archive >> schemas;
    archive >> databases;
    archive >> chunks;
    // EXISTING_CODE
    // EXISTING_CODE
    finishParse();
    return true;
}

//---------------------------------------------------------------------------------------------------
bool CManifest::SerializeC(CArchive& archive) const {
    // Writing always writes the latest version of the data
    CBaseNode::SerializeC(archive);

    // EXISTING_CODE
    // EXISTING_CODE
    archive << version;
    archive << chain;
    archive << schemas;
    archive << databases;
    archive << chunks;
    // EXISTING_CODE
    // EXISTING_CODE
    return true;
}

//---------------------------------------------------------------------------------------------------
bool CManifest::Migrate(CArchive& archiveIn, CArchive& archiveOut) const {
    ASSERT(archiveIn.isReading());
    ASSERT(archiveOut.isWriting());
    CManifest copy;
    // EXISTING_CODE
    // EXISTING_CODE
    copy.Serialize(archiveIn);
    copy.SerializeC(archiveOut);
    return true;
}

//---------------------------------------------------------------------------
CArchive& operator>>(CArchive& archive, CManifestArray& array) {
    uint64_t count;
    archive >> count;
    array.resize(count);
    for (size_t i = 0; i < count; i++) {
        ASSERT(i < array.capacity());
        array.at(i).Serialize(archive);
    }
    return archive;
}

//---------------------------------------------------------------------------
CArchive& operator<<(CArchive& archive, const CManifestArray& array) {
    uint64_t count = array.size();
    archive << count;
    for (size_t i = 0; i < array.size(); i++)
        array[i].SerializeC(archive);
    return archive;
}

//---------------------------------------------------------------------------
void CManifest::registerClass(void) {
    // only do this once
    if (HAS_FIELD(CManifest, "schema"))
        return;

    size_t fieldNum = 1000;
    ADD_FIELD(CManifest, "schema", T_NUMBER, ++fieldNum);
    ADD_FIELD(CManifest, "deleted", T_BOOL, ++fieldNum);
    ADD_FIELD(CManifest, "showing", T_BOOL, ++fieldNum);
    ADD_FIELD(CManifest, "cname", T_TEXT, ++fieldNum);
    ADD_FIELD(CManifest, "version", T_TEXT | TS_OMITEMPTY, ++fieldNum);
    ADD_FIELD(CManifest, "chain", T_TEXT | TS_OMITEMPTY, ++fieldNum);
    ADD_FIELD(CManifest, "schemas", T_IPFSHASH | TS_OMITEMPTY, ++fieldNum);
    ADD_FIELD(CManifest, "databases", T_IPFSHASH | TS_OMITEMPTY, ++fieldNum);
    ADD_FIELD(CManifest, "chunks", T_OBJECT | TS_ARRAY | TS_OMITEMPTY, ++fieldNum);

    // Hide our internal fields, user can turn them on if they like
    HIDE_FIELD(CManifest, "schema");
    HIDE_FIELD(CManifest, "deleted");
    HIDE_FIELD(CManifest, "showing");
    HIDE_FIELD(CManifest, "cname");

    builtIns.push_back(_biCManifest);

    // EXISTING_CODE
    // EXISTING_CODE
}

//---------------------------------------------------------------------------
string_q nextManifestChunk_custom(const string_q& fieldIn, const void* dataPtr) {
    const CManifest* man = reinterpret_cast<const CManifest*>(dataPtr);
    if (man) {
        switch (tolower(fieldIn[0])) {
            // EXISTING_CODE
            // EXISTING_CODE
            case 'p':
                // Display only the fields of this node, not it's parent type
                if (fieldIn % "parsed")
                    return nextBasenodeChunk(fieldIn, man);
                // EXISTING_CODE
                // EXISTING_CODE
                break;

            default:
                break;
        }
    }

    return "";
}

// EXISTING_CODE
// EXISTING_CODE

//---------------------------------------------------------------------------
bool CManifest::readBackLevel(CArchive& archive) {
    bool done = false;
    // EXISTING_CODE
    // EXISTING_CODE
    return done;
}

//---------------------------------------------------------------------------
CArchive& operator<<(CArchive& archive, const CManifest& man) {
    man.SerializeC(archive);
    return archive;
}

//---------------------------------------------------------------------------
CArchive& operator>>(CArchive& archive, CManifest& man) {
    man.Serialize(archive);
    return archive;
}

//-------------------------------------------------------------------------
ostream& operator<<(ostream& os, const CManifest& it) {
    // EXISTING_CODE
    // EXISTING_CODE

    it.Format(os, "", nullptr);
    os << "\n";
    return os;
}

//---------------------------------------------------------------------------
const CBaseNode* CManifest::getObjectAt(const string_q& fieldName, size_t index) const {
    // EXISTING_CODE
    // EXISTING_CODE
    if (fieldName % "chunks") {
        if (index == NOPOS) {
            CPinnedChunk empty;
            ((CManifest*)this)->chunks.push_back(empty);  // NOLINT
            index = chunks.size() - 1;
        }
        if (index < chunks.size())
            return &chunks[index];
    }
    // EXISTING_CODE
    // EXISTING_CODE

    return NULL;
}

//---------------------------------------------------------------------------
const char* STR_DISPLAY_MANIFEST = "";

//---------------------------------------------------------------------------
// EXISTING_CODE
// EXISTING_CODE
}  // namespace qblocks