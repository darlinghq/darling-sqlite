/*
 *  sqlite3_private.h
 */

#ifndef _SQLITE3_PRIVATE_H
#define _SQLITE3_PRIVATE_H

#include <unistd.h>
#include <sqlite3.h>

/*
** Make sure we can call this stuff from C++.
*/
#ifdef __cplusplus
extern "C" {
#endif

#define SQLITE_TRACE_STMT       0x01
#define SQLITE_TRACE_PROFILE    0x02
#define SQLITE_TRACE_ROW        0x04
#define SQLITE_TRACE_CLOSE      0x08

extern int sqlite3_trace_v2(sqlite3*,
		unsigned uMask,
		int(*xCallback)(unsigned,void*,void*,void*),
		void *pCtx);

#define SQLITE_LOCKSTATE_OFF    0
#define SQLITE_LOCKSTATE_ON     1
#define SQLITE_LOCKSTATE_NOTADB 2
#define SQLITE_LOCKSTATE_ERROR  -1

#define SQLITE_LOCKSTATE_ANYPID -1

/* 
** Test a file path for sqlite locks held by a process ID (-1 = any PID). 
** Returns one of the following integer codes:
** 
**   SQLITE_LOCKSTATE_OFF    no active sqlite file locks match the specified pid
**   SQLITE_LOCKSTATE_ON     active sqlite file locks match the specified pid
**   SQLITE_LOCKSTATE_NOTADB path points to a file that is not an sqlite db file
**   SQLITE_LOCKSTATE_ERROR  path was not vaild or was unreadable
**
** There is no support for identifying db files encrypted via SEE encryption
** currently.  Zero byte files are tested for sqlite locks, but if no sqlite 
** locks are present then SQLITE_LOCKSTATE_NOTADB is returned.
*/
extern int _sqlite3_lockstate(const char *path, pid_t pid);

/*
** Test an open database connection for sqlite locks held by a process ID,
** if a process has an open database connection this will avoid trashing file
** locks by re-using open file descriptors for the database file and support
** files (-shm)
*/
#define SQLITE_FCNTL_LOCKSTATE_PID          103

/*
** Purges eligible purgable memory regions (holding only unpinned pages) from 
** the page cache
*/
extern void _sqlite3_purgeEligiblePagerCacheMemory(void);

/*
** Returns the system defined default sqlite3_busy_handler function
** 
*/
extern int (*_sqlite3_system_busy_handler(void))(void*,int);

/*
** Pass the SQLITE_TRUNCATE_DATABASE operation code to sqlite3_file_control() 
** to truncate a database and its associated journal file to zero length.  The 
** SQLITE_TRUNCATE_* flags represent optional flags to safely initialize an
** empty database in the place of the truncated database, the flags are passed 
** into sqlite3_file_control via the fourth argument using a pointer to an integer
** configured with the ORed flags.  If the fourth argument is NULL, the default 
** behavior is applied and the database file is truncated to zero bytes, a rollback 
** journal (if present) is unlinked, a WAL journal (if present) is truncated to zero 
** bytes and the first few bytes of the -shm file is scrambled to trigger existing
** connections to rebuild the index from the database file contents.
*/
#define SQLITE_FCNTL_TRUNCATE_DATABASE      101
#define SQLITE_TRUNCATE_DATABASE            SQLITE_FCNTL_TRUNCATE_DATABASE
#define SQLITE_TRUNCATE_INITIALIZE_HEADER_MASK    (0x7F<<0)
#define SQLITE_TRUNCATE_JOURNALMODE_WAL           (0x1<<0)
#define SQLITE_TRUNCATE_AUTOVACUUM_MASK           (0x3<<2)
#define SQLITE_TRUNCATE_AUTOVACUUM_OFF            (0x1<<2)
#define SQLITE_TRUNCATE_AUTOVACUUM_FULL           (0x2<<2)
#define SQLITE_TRUNCATE_AUTOVACUUM_INCREMENTAL    (0x3<<2)
#define SQLITE_TRUNCATE_PAGESIZE_MASK             (0x7<<4)
#define SQLITE_TRUNCATE_PAGESIZE_1024             (0x1<<4)
#define SQLITE_TRUNCATE_PAGESIZE_2048             (0x2<<4)
#define SQLITE_TRUNCATE_PAGESIZE_4096             (0x3<<4)
#define SQLITE_TRUNCATE_PAGESIZE_8192             (0x4<<4)
#define SQLITE_TRUNCATE_FORCE                     (0x1<<7)

/*
** Pass the SQLITE_REPLACE_DATABASE operation code to sqlite3_file_control()
** and a sqlite3 pointer to another open database file to safely copy the 
** contents of that database file into the receiving database.
*/
#define SQLITE_FCNTL_REPLACE_DATABASE       102
#define SQLITE_REPLACE_DATABASE             SQLITE_FCNTL_REPLACE_DATABASE

  
/*
** ****************************************************************************
** EXPERIMENTAL API moved from sqlite3.h; the following is not recommended
** for general consumption.
** ****************************************************************************
*/

/*
** CAPI3REF: The pre-update hook.
**
** ^These interfaces are only available if SQLite is compiled using the
** [SQLITE_ENABLE_PREUPDATE_HOOK] compile-time option.
**
** ^The [sqlite3_preupdate_hook()] interface registers a callback function
** that is invoked prior to each [INSERT], [UPDATE], and [DELETE] operation
** on a [rowid table].
** ^At most one preupdate hook may be registered at a time on a single
** [database connection]; each call to [sqlite3_preupdate_hook()] overrides
** the previous setting.
** ^The preupdate hook is disabled by invoking [sqlite3_preupdate_hook()]
** with a NULL pointer as the second parameter.
** ^The third parameter to [sqlite3_preupdate_hook()] is passed through as
** the first parameter to callbacks.
**
** ^The preupdate hook only fires for changes to [rowid tables]; the preupdate
** hook is not invoked for changes to [virtual tables] or [WITHOUT ROWID]
** tables.
**
** ^The second parameter to the preupdate callback is a pointer to
** the [database connection] that registered the preupdate hook.
** ^The third parameter to the preupdate callback is one of the constants
** [SQLITE_INSERT], [SQLITE_DELETE], or [SQLITE_UPDATE] to identify the
** kind of update operation that is about to occur.
** ^(The fourth parameter to the preupdate callback is the name of the
** database within the database connection that is being modified.  This
** will be "main" for the main database or "temp" for TEMP tables or 
** the name given after the AS keyword in the [ATTACH] statement for attached
** databases.)^
** ^The fifth parameter to the preupdate callback is the name of the
** table that is being modified.
** ^The sixth parameter to the preupdate callback is the initial [rowid] of the
** row being changes for SQLITE_UPDATE and SQLITE_DELETE changes and is
** undefined for SQLITE_INSERT changes.
** ^The seventh parameter to the preupdate callback is the final [rowid] of
** the row being changed for SQLITE_UPDATE and SQLITE_INSERT changes and is
** undefined for SQLITE_DELETE changes.
**
** The [sqlite3_preupdate_old()], [sqlite3_preupdate_new()],
** [sqlite3_preupdate_count()], and [sqlite3_preupdate_depth()] interfaces
** provide additional information about a preupdate event. These routines
** may only be called from within a preupdate callback.  Invoking any of
** these routines from outside of a preupdate callback or with a
** [database connection] pointer that is different from the one supplied
** to the preupdate callback results in undefined and probably undesirable
** behavior.
**
** ^The [sqlite3_preupdate_count(D)] interface returns the number of columns
** in the row that is being inserted, updated, or deleted.
**
** ^The [sqlite3_preupdate_old(D,N,P)] interface writes into P a pointer to
** a [protected sqlite3_value] that contains the value of the Nth column of
** the table row before it is updated.  The N parameter must be between 0
** and one less than the number of columns or the behavior will be
** undefined. This must only be used within SQLITE_UPDATE and SQLITE_DELETE
** preupdate callbacks; if it is used by an SQLITE_INSERT callback then the
** behavior is undefined.  The [sqlite3_value] that P points to
** will be destroyed when the preupdate callback returns.
**
** ^The [sqlite3_preupdate_new(D,N,P)] interface writes into P a pointer to
** a [protected sqlite3_value] that contains the value of the Nth column of
** the table row after it is updated.  The N parameter must be between 0
** and one less than the number of columns or the behavior will be
** undefined. This must only be used within SQLITE_INSERT and SQLITE_UPDATE
** preupdate callbacks; if it is used by an SQLITE_DELETE callback then the
** behavior is undefined.  The [sqlite3_value] that P points to
** will be destroyed when the preupdate callback returns.
**
** ^The [sqlite3_preupdate_depth(D)] interface returns 0 if the preupdate
** callback was invoked as a result of a direct insert, update, or delete
** operation; or 1 for inserts, updates, or deletes invoked by top-level 
** triggers; or 2 for changes resulting from triggers called by top-level
** triggers; and so forth.
**
** See also:  [sqlite3_update_hook()]
*/
SQLITE_EXPERIMENTAL void *sqlite3_preupdate_hook(
  sqlite3 *db,
  void(*xPreUpdate)(
    void *pCtx,                   /* Copy of third arg to preupdate_hook() */
    sqlite3 *db,                  /* Database handle */
    int op,                       /* SQLITE_UPDATE, DELETE or INSERT */
    char const *zDb,              /* Database name */
    char const *zName,            /* Table name */
    sqlite3_int64 iKey1,          /* Rowid of row about to be deleted/updated */
    sqlite3_int64 iKey2           /* New rowid value (for a rowid UPDATE) */
  ),
  void*
);
SQLITE_EXPERIMENTAL int sqlite3_preupdate_old(sqlite3 *, int, sqlite3_value **);
SQLITE_EXPERIMENTAL int sqlite3_preupdate_count(sqlite3 *);
SQLITE_EXPERIMENTAL int sqlite3_preupdate_depth(sqlite3 *);
SQLITE_EXPERIMENTAL int sqlite3_preupdate_new(sqlite3 *, int, sqlite3_value **);

/*
** CAPI3REF: Low-level system error code
**
** ^Attempt to return the underlying operating system error code or error
** number that caused the most recent I/O error or failure to open a file.
** The return value is OS-dependent.  For example, on unix systems, after
** [sqlite3_open_v2()] returns [SQLITE_CANTOPEN], this interface could be
** called to get back the underlying "errno" that caused the problem, such
** as ENOSPC, EAUTH, EISDIR, and so forth.  
*/
int sqlite3_system_errno(sqlite3*);

/*
** CAPI3REF: Database Snapshot
** KEYWORDS: {snapshot}
** EXPERIMENTAL
**
** An instance of the snapshot object records the state of a [WAL mode]
** database for some specific point in history.
**
** In [WAL mode], multiple [database connections] that are open on the
** same database file can each be reading a different historical version
** of the database file.  When a [database connection] begins a read
** transaction, that connection sees an unchanging copy of the database
** as it existed for the point in time when the transaction first started.
** Subsequent changes to the database from other connections are not seen
** by the reader until a new read transaction is started.
**
** The sqlite3_snapshot object records state information about an historical
** version of the database file so that it is possible to later open a new read
** transaction that sees that historical version of the database rather than
** the most recent version.
**
** The constructor for this object is [sqlite3_snapshot_get()].  The
** [sqlite3_snapshot_open()] method causes a fresh read transaction to refer
** to an historical snapshot (if possible).  The destructor for 
** sqlite3_snapshot objects is [sqlite3_snapshot_free()].
*/
typedef struct sqlite3_snapshot sqlite3_snapshot;

/*
** CAPI3REF: Record A Database Snapshot
** EXPERIMENTAL
**
** ^The [sqlite3_snapshot_get(D,S,P)] interface attempts to make a
** new [sqlite3_snapshot] object that records the current state of
** schema S in database connection D.  ^On success, the
** [sqlite3_snapshot_get(D,S,P)] interface writes a pointer to the newly
** created [sqlite3_snapshot] object into *P and returns SQLITE_OK.
** ^If schema S of [database connection] D is not a [WAL mode] database
** that is in a read transaction, then [sqlite3_snapshot_get(D,S,P)]
** leaves the *P value unchanged and returns an appropriate [error code].
**
** The [sqlite3_snapshot] object returned from a successful call to
** [sqlite3_snapshot_get()] must be freed using [sqlite3_snapshot_free()]
** to avoid a memory leak.
**
** The [sqlite3_snapshot_get()] interface is only available when the
** SQLITE_ENABLE_SNAPSHOT compile-time option is used.
*/
SQLITE_EXPERIMENTAL int sqlite3_snapshot_get(
  sqlite3 *db,
  const char *zSchema,
  sqlite3_snapshot **ppSnapshot
);

/*
** CAPI3REF: Start a read transaction on an historical snapshot
** EXPERIMENTAL
**
** ^The [sqlite3_snapshot_open(D,S,P)] interface starts a
** read transaction for schema S of
** [database connection] D such that the read transaction
** refers to historical [snapshot] P, rather than the most
** recent change to the database.
** ^The [sqlite3_snapshot_open()] interface returns SQLITE_OK on success
** or an appropriate [error code] if it fails.
**
** ^In order to succeed, a call to [sqlite3_snapshot_open(D,S,P)] must be
** the first operation following the [BEGIN] that takes the schema S
** out of [autocommit mode].
** ^In other words, schema S must not currently be in
** a transaction for [sqlite3_snapshot_open(D,S,P)] to work, but the
** database connection D must be out of [autocommit mode].
** ^A [snapshot] will fail to open if it has been overwritten by a
** [checkpoint].
** ^(A call to [sqlite3_snapshot_open(D,S,P)] will fail if the
** database connection D does not know that the database file for
** schema S is in [WAL mode].  A database connection might not know
** that the database file is in [WAL mode] if there has been no prior
** I/O on that database connection, or if the database entered [WAL mode] 
** after the most recent I/O on the database connection.)^
** (Hint: Run "[PRAGMA application_id]" against a newly opened
** database connection in order to make it ready to use snapshots.)
**
** The [sqlite3_snapshot_open()] interface is only available when the
** SQLITE_ENABLE_SNAPSHOT compile-time option is used.
*/
SQLITE_EXPERIMENTAL int sqlite3_snapshot_open(
  sqlite3 *db,
  const char *zSchema,
  sqlite3_snapshot *pSnapshot
);

/*
** CAPI3REF: Destroy a snapshot
** EXPERIMENTAL
**
** ^The [sqlite3_snapshot_free(P)] interface destroys [sqlite3_snapshot] P.
** The application must eventually free every [sqlite3_snapshot] object
** using this routine to avoid a memory leak.
**
** The [sqlite3_snapshot_free()] interface is only available when the
** SQLITE_ENABLE_SNAPSHOT compile-time option is used.
*/
SQLITE_EXPERIMENTAL void sqlite3_snapshot_free(sqlite3_snapshot*);

/*
** CAPI3REF: Compare the ages of two snapshot handles.
** EXPERIMENTAL
**
** The sqlite3_snapshot_cmp(P1, P2) interface is used to compare the ages
** of two valid snapshot handles. 
**
** If the two snapshot handles are not associated with the same database 
** file, the result of the comparison is undefined. 
**
** Additionally, the result of the comparison is only valid if both of the
** snapshot handles were obtained by calling sqlite3_snapshot_get() since the
** last time the wal file was deleted. The wal file is deleted when the
** database is changed back to rollback mode or when the number of database
** clients drops to zero. If either snapshot handle was obtained before the 
** wal file was last deleted, the value returned by this function 
** is undefined.
**
** Otherwise, this API returns a negative value if P1 refers to an older
** snapshot than P2, zero if the two handles refer to the same database
** snapshot, and a positive value if P1 is a newer snapshot than P2.
*/
SQLITE_EXPERIMENTAL int sqlite3_snapshot_cmp(
  sqlite3_snapshot *p1,
  sqlite3_snapshot *p2
);

/*
** ****************************************************************************
** SEE API moved from sqlite3.h; the following is for internal use only!
** ****************************************************************************
*/
  
/*
** Specify the key for an encrypted database.  This routine should be
** called right after sqlite3_open().
**
** The code to implement this API is not available in the public release
** of SQLite.
*/
SQLITE_API int SQLITE_STDCALL sqlite3_key(
  sqlite3 *db,                   /* Database to be rekeyed */
  const void *pKey, int nKey     /* The key */
);
SQLITE_API int SQLITE_STDCALL sqlite3_key_v2(
  sqlite3 *db,                   /* Database to be rekeyed */
  const char *zDbName,           /* Name of the database */
  const void *pKey, int nKey     /* The key */
);

/*
** Change the key on an open database.  If the current database is not
** encrypted, this routine will encrypt it.  If pNew==0 or nNew==0, the
** database is decrypted.
**
** The code to implement this API is not available in the public release
** of SQLite.
*/
SQLITE_API int SQLITE_STDCALL sqlite3_rekey(
  sqlite3 *db,                   /* Database to be rekeyed */
  const void *pKey, int nKey     /* The new key */
);
SQLITE_API int SQLITE_STDCALL sqlite3_rekey_v2(
  sqlite3 *db,                   /* Database to be rekeyed */
  const char *zDbName,           /* Name of the database */
  const void *pKey, int nKey     /* The new key */
);

/*
** Specify the activation key for a SEE database.  Unless 
** activated, none of the SEE routines will work.
*/
SQLITE_API void SQLITE_STDCALL sqlite3_activate_see(
  const char *zPassPhrase        /* Activation phrase */
);

  
#ifdef __cplusplus
}  /* end of the 'extern "C"' block */
#endif


/*
** ****************************************************************************
** Note for future SQLite merges: These declarations are intentionally copied
** from test_intarray.h in order to expose intarray as SPI.
** ****************************************************************************
*/
#ifndef _INTARRAY_H_
#define _INTARRAY_H_

/*
** Make sure we can call this stuff from C++.
*/
#ifdef __cplusplus
extern "C" {
#endif

/*
** An sqlite3_intarray is an abstract type to stores an instance of
** an integer array.
*/
typedef struct sqlite3_intarray sqlite3_intarray;

/*
** Invoke this routine to create a specific instance of an intarray object.
** The new intarray object is returned by the 3rd parameter.
**
** Each intarray object corresponds to a virtual table in the TEMP table
** with a name of zName.
**
** Destroy the intarray object by dropping the virtual table.  If not done
** explicitly by the application, the virtual table will be dropped implicitly
** by the system when the database connection is closed.
*/
int sqlite3_intarray_create(
  sqlite3 *db,
  const char *zName,
  sqlite3_intarray **ppReturn
);

/*
** Bind a new array array of integers to a specific intarray object.
**
** The array of integers bound must be unchanged for the duration of
** any query against the corresponding virtual table.  If the integer
** array does change or is deallocated undefined behavior will result.
*/
int sqlite3_intarray_bind(
  sqlite3_intarray *pIntArray,   /* The intarray object to bind to */
  int nElements,                 /* Number of elements in the intarray */
  sqlite3_int64 *aElements,      /* Content of the intarray */
  void (*xFree)(void*)           /* How to dispose of the intarray when done */
);

#ifdef __cplusplus
}  /* End of the 'extern "C"' block */
#endif
#endif /* _INTARRAY_H_ */

#endif

