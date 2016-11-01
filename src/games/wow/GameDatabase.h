/*
 * GameDatabase.h
 *
 *  Created on: 9 nov. 2014
 *      Author: Jerome
 */

#ifndef _GAMEDATABASE_H_
#define _GAMEDATABASE_H_

#include <map>
#include <vector>
#include "sqlite3.h"

class QDomElement;
#include <QString>

class sqlResult
{
  public:
    sqlResult() : valid(false), nbcols(0) {}
    ~sqlResult() { /* TODO :free char** */ }
    bool empty() { return values.size() == 0; }
    bool valid;
    int nbcols;
    std::vector<std::vector<QString> > values;
};

#ifdef _WIN32
#    ifdef BUILDING_WOW_DLL
#        define _GAMEDATABASE_API_ __declspec(dllexport)
#    else
#        define _GAMEDATABASE_API_ __declspec(dllimport)
#    endif
#else
#    define _GAMEDATABASE_API_
#endif

class _GAMEDATABASE_API_ GameDatabase
{
  public:
    GameDatabase();
    GameDatabase(GameDatabase &);

    bool initFromXML(const QString & file);

    sqlResult sqlQuery(const QString &query);

    void setFastMode() { m_fastMode = true; }

    ~GameDatabase();

  private:
    static int treatQuery(void *NotUsed, int nbcols, char ** values , char ** cols);
    static void logQueryTime(void* aDb, const char* aQueryStr, sqlite3_uint64 aTimeInNs);

    bool createDatabaseFromXML(const QString & file);
    bool createTableFromXML(const QDomElement &);
    bool fillTableFromGameFile(const QString & table, const QString & gamefile);

    sqlite3 *m_db;
    // std::map<TableName, [fieldID] <fieldName,fieldType> >
    // ie m_dbStruct["FileData"][0] => pair<"id","uint">
    std::map<QString, std::map<int, std::pair<QString, QString> > >  m_dbStruct;

    bool m_fastMode;
};



#endif /* _GAMEDATABASE_H_ */
