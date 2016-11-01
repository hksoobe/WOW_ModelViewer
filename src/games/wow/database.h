#ifndef DATABASE_H
#define DATABASE_H

// Combined the previous 5 various "db" files into one.
// trying to cut down on excess files.
// Also instead of declaring the db objects inside various classes
// may aswell declare them as globals since pretty much most the
// different objects need to access them at one point or another.

// STL
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <set>


#include <QString>

// wmv database
class ItemDatabase;
struct NPCRecord;

#ifdef _WIN32
#    ifdef BUILDING_WOW_DLL
#        define _DATABASE_API_ __declspec(dllexport)
#    else
#        define _DATABASE_API_ __declspec(dllimport)
#    endif
#else
#    define _DATABASE_API_
#endif

class ItemDatabase;

_DATABASE_API_ extern ItemDatabase items;
_DATABASE_API_ extern std::vector<NPCRecord> npcs;

// ==============================================

// -----------------------------------
// Item Stuff
// -----------------------------------

struct _DATABASE_API_ ItemRecord {
	QString name;
	int id, itemclass, subclass, type, model, sheath, quality;

	ItemRecord(const std::vector<QString> &);
	ItemRecord():id(0), itemclass(-1), subclass(-1), type(0), model(0), sheath(0), quality(0)
	{}

	int slot();
};

class _DATABASE_API_ ItemDatabase {
public:
	ItemDatabase();

	std::vector<ItemRecord> items;
	std::map<int, int> itemLookup;

	const ItemRecord& getById(int id);
};

// ============/////////////////=================/////////////////


// ------------------------------
// NPC Stuff
// -------------------------------
struct _DATABASE_API_ NPCRecord
{
	QString name;
	int id, model, type;

	NPCRecord(QString line);
	NPCRecord(const std::vector<QString> &);
	NPCRecord(): id(0), model(0), type(0) {}
	NPCRecord(const NPCRecord &r): name(r.name), id(r.id), model(r.model), type(r.type) {}

};
#endif

