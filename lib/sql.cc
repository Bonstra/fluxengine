#include "globals.h"
#include "sql.h"
#include "fluxmap.h"

void sqlCheck(sqlite3* db, int i)
{
    if (i != SQLITE_OK)
        Error() << "database error: " << sqlite3_errmsg(db);
}

sqlite3* sqlOpen(const std::string filename, int flags)
{
    sqlite3* db;
    int i = sqlite3_open_v2(filename.c_str(), &db, flags, NULL);
    if (i != SQLITE_OK)
        Error() << "failed to open output file: " << sqlite3_errstr(i);

    return db;
}

void sqlClose(sqlite3* db)
{
	sqlite3_close(db);
}

void sqlStmt(sqlite3* db, const char* sql)
{
    char* errmsg;
    int i = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
    if (i != SQLITE_OK)
        Error() << "database error: %s" << errmsg;
}

void sql_bind_blob(sqlite3* db, sqlite3_stmt* stmt, const char* name,
    const void* ptr, size_t bytes)
{
    sqlCheck(db, sqlite3_bind_blob(stmt,
        sqlite3_bind_parameter_index(stmt, name),
        ptr, bytes, SQLITE_TRANSIENT));
}

void sql_bind_int(sqlite3* db, sqlite3_stmt* stmt, const char* name, int value)
{
    sqlCheck(db, sqlite3_bind_int(stmt,
        sqlite3_bind_parameter_index(stmt, name),
        value));
}

void sqlPrepareFlux(sqlite3* db)
{
    sqlStmt(db, "PRAGMA synchronous = OFF;");
    sqlStmt(db, "CREATE TABLE IF NOT EXISTS rawdata ("
                 "  track INTEGER,"
                 "  side INTEGER,"
                 "  data BLOB,"
                 "  PRIMARY KEY(track, side)"
                 ");");
}

void sqlWriteFlux(sqlite3* db, int track, int side, const Fluxmap& fluxmap)
{
    sqlite3_stmt* stmt;
    sqlCheck(db, sqlite3_prepare_v2(db,
        "INSERT OR REPLACE INTO rawdata (track, side, data) VALUES (:track, :side, :data)",
        -1, &stmt, NULL));
    sql_bind_int(db, stmt, ":track", track);
    sql_bind_int(db, stmt, ":side", side);
    sql_bind_blob(db, stmt, ":data", fluxmap.ptr(), fluxmap.bytes());

    if (sqlite3_step(stmt) != SQLITE_DONE)
        Error() << "failed to write to database: " << sqlite3_errmsg(db);
    sqlCheck(db, sqlite3_finalize(stmt));
}

std::unique_ptr<Fluxmap> sqlReadFlux(sqlite3* db, int track, int side)
{
    sqlite3_stmt* stmt;
    sqlCheck(db, sqlite3_prepare_v2(db,
        "SELECT data FROM rawdata WHERE track=:track AND side=:side",
        -1, &stmt, NULL));
    sql_bind_int(db, stmt, ":track", track);
    sql_bind_int(db, stmt, ":side", side);

    auto fluxmap = std::unique_ptr<Fluxmap>(new Fluxmap());

    int i = sqlite3_step(stmt);
    if (i != SQLITE_DONE)
    {
        if (i != SQLITE_ROW)
            Error() << "failed to read from database: " << sqlite3_errmsg(db);

        const uint8_t* blobptr = (const uint8_t*) sqlite3_column_blob(stmt, 0);
        size_t bloblen = sqlite3_column_bytes(stmt, 0);

        fluxmap->appendBytes(blobptr, bloblen);
    }
    sqlCheck(db, sqlite3_finalize(stmt));
    return fluxmap;
}

#if 0
void sql_for_all_flux_data(sqlite3* db,
    void (*cb)(int track, int side, const struct fluxmap* fluxmap))
{
    sqlite3_stmt* stmt;
    sql_check(db, sqlite3_prepare_v2(db,
        "SELECT track, side, data FROM rawdata ORDER BY track, side ASC",
        -1, &stmt, NULL));

    struct fluxmap* fluxmap = create_fluxmap();
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        int track = sqlite3_column_int(stmt, 0);
        int side = sqlite3_column_int(stmt, 1);
        const void* ptr = sqlite3_column_blob(stmt, 2);
        size_t len = sqlite3_column_bytes(stmt, 2);
        fluxmap_append_intervals(fluxmap, ptr, len);
        cb(track, side, fluxmap);
        fluxmap_clear(fluxmap);
    }
    free_fluxmap(fluxmap);

    sql_check(db, sqlite3_finalize(stmt));
}

void sql_prepare_record(sqlite3* db)
{
    sql_stmt(db, "PRAGMA synchronous = OFF;");
    sql_stmt(db, "CREATE TABLE IF NOT EXISTS records ("
                 "  track INTEGER,"
                 "  side INTEGER,"
                 "  record INTEGER,"
                 "  data BLOB,"
                 "  PRIMARY KEY(track, side, record)"
                 ");");
}

void sql_write_record(sqlite3* db, int track, int side, int record, const uint8_t* ptr, size_t len)
{
    sqlite3_stmt* stmt;
    sql_check(db, sqlite3_prepare_v2(db,
        "INSERT OR REPLACE INTO records (track, side, record, data) VALUES (:track, :side, :record, :data)",
        -1, &stmt, NULL));
    sql_bind_int(db, stmt, ":track", track);
    sql_bind_int(db, stmt, ":side", side);
    sql_bind_int(db, stmt, ":record", record);
    sql_bind_blob(db, stmt, ":data", ptr, len);

    if (sqlite3_step(stmt) != SQLITE_DONE)
        error("failed to write to database: %s", sqlite3_errmsg(db));
    sql_check(db, sqlite3_finalize(stmt));
}

void sql_for_all_record_data(sqlite3* db,
    void (*cb)(int track, int side, int record, const uint8_t* data, size_t len))
{
    sqlite3_stmt* stmt;
    sql_check(db, sqlite3_prepare_v2(db,
        "SELECT track, side, record, data FROM records ORDER BY track, side, record ASC",
        -1, &stmt, NULL));

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        int track = sqlite3_column_int(stmt, 0);
        int side = sqlite3_column_int(stmt, 1);
        int record = sqlite3_column_int(stmt, 2);
        const void* ptr = sqlite3_column_blob(stmt, 3);
        size_t len = sqlite3_column_bytes(stmt, 3);
        cb(track, side, record, ptr, len);
    }

    sql_check(db, sqlite3_finalize(stmt));
}


#endif
