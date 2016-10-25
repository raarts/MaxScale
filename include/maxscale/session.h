#pragma once
#ifndef _MAXSCALE_SESSION_H
#define _MAXSCALE_SESSION_H
/*
 * Copyright (c) 2016 MariaDB Corporation Ab
 *
 * Use of this software is governed by the Business Source License included
 * in the LICENSE.TXT file and at www.mariadb.com/bsl.
 *
 * Change Date: 2019-07-01
 *
 * On the date above, in accordance with the Business Source License, use
 * of this software will be governed by version 2 or later of the General
 * Public License.
 */

/**
 * @file session.h
 *
 * @verbatim
 * Revision History
 *
 * Date         Who                     Description
 * 01-06-2013   Mark Riddoch            Initial implementation
 * 14-06-2013   Massimiliano Pinto      Added void *data to session
 *                                      for session specific data
 * 01-07-2013   Massimiliano Pinto      Removed backends pointer
 *                                      from struct session
 * 02-09-2013   Massimiliano Pinto      Added session ref counter
 * 29-05-2014   Mark Riddoch            Support for filter mechanism
 *                                      added
 * 20-02-2015   Markus Mäkelä           Added session timeouts
 * 27/06/2016   Martin Brampton         Modify session struct for list manager
 *
 * @endverbatim
 */

#include <maxscale/cdefs.h>
#include <time.h>
#include <maxscale/atomic.h>
#include <maxscale/buffer.h>
#include <maxscale/listmanager.h>
#include <maxscale/spinlock.h>
#include <maxscale/resultset.h>
#include <maxscale/log_manager.h>

MXS_BEGIN_DECLS

struct dcb;
struct service;
struct filter_def;

/**
 * The session statistics structure
 */
typedef struct
{
    time_t          connect;        /**< Time when the session was started */
} SESSION_STATS;

#define SESSION_STATS_INIT {0}

typedef enum
{
    SESSION_STATE_ALLOC,            /*< for all sessions */
    SESSION_STATE_READY,            /*< for router session */
    SESSION_STATE_ROUTER_READY,     /*< for router session */
    SESSION_STATE_STOPPING,         /*< session and router are being closed */
    SESSION_STATE_LISTENER,         /*< for listener session */
    SESSION_STATE_LISTENER_STOPPED, /*< for listener session */
    SESSION_STATE_TO_BE_FREED,      /*< ready to be freed as soon as there are no references */
    SESSION_STATE_FREE,             /*< for all sessions */
    SESSION_STATE_DUMMY             /*< dummy session for consistency */
} session_state_t;

typedef enum
{
    SESSION_TRX_INACTIVE_BIT   = 1, /* 0b0001 */
    SESSION_TRX_ACTIVE_BIT     = 2, /* 0b0010 */
    SESSION_TRX_READ_ONLY_BIT  = 4, /* 0b0100 */
    SESSION_TRX_READ_WRITE_BIT = 8, /* 0b1000 */
} session_trx_state_bit_t;

typedef enum
{
    /*< There is no on-going transaction. */
    SESSION_TRX_INACTIVE    = SESSION_TRX_INACTIVE_BIT,
    /*< A transaction is active. */
    SESSION_TRX_ACTIVE      = SESSION_TRX_ACTIVE_BIT,
    /*< An explicit READ ONLY transaction is active. */
    SESSION_TRX_READ_ONLY   = (SESSION_TRX_ACTIVE_BIT | SESSION_TRX_READ_ONLY_BIT),
    /*< An explicit READ WRITE transaction is active. */
    SESSION_TRX_READ_WRITE  = (SESSION_TRX_ACTIVE_BIT | SESSION_TRX_READ_WRITE_BIT)
} session_trx_state_t;

/**
 * Convert transaction state to string representation.
 *
 * @param state A transaction state.
 * @return String representation of the state.
 */
const char* session_trx_state_to_string(session_trx_state_t state);

/**
 * The downstream element in the filter chain. This may refer to
 * another filter or to a router.
 */
typedef struct
{
    void *instance;
    void *session;
    int (*routeQuery)(void *instance, void *session, GWBUF *request);
} DOWNSTREAM;

#define DOWNSTREAM_INIT {0}

/**
 * The upstream element in the filter chain. This may refer to
 * another filter or to the protocol implementation.
 */
typedef struct
{
    void *instance;
    void *session;
    int (*clientReply)(void *instance, void *session, GWBUF *response);
    int (*error)(void *instance, void *session, void *);
} UPSTREAM;

#define UPSTREAM_INIT {0}

/**
 * Structure used to track the filter instances and sessions of the filters
 * that are in use within a session.
 */
typedef struct
{
    struct filter_def *filter;
    void *instance;
    void *session;
} SESSION_FILTER;

#define SESSION_FILTER_INIT {0}

/**
 * Filter type for the sessionGetList call
 */
typedef enum
{
    SESSION_LIST_ALL,
    SESSION_LIST_CONNECTION
} SESSIONLISTFILTER;

/**
 * The session status block
 *
 * A session status block is created for each user (client) connection
 * to the database, it links the descriptors, routing implementation
 * and originating service together for the client session.
 * 
 * Note that the first few fields (up to and including "entry_is_ready") must
 * precisely match the LIST_ENTRY structure defined in the list manager.
 */
typedef struct session
{
    LIST_ENTRY_FIELDS
    skygw_chk_t     ses_chk_top;
    SPINLOCK        ses_lock;
    session_state_t state;            /*< Current descriptor state */
    size_t          ses_id;           /*< Unique session identifier */
    int             enabled_log_priorities; /*< Bitfield of enabled syslog priorities */
    struct dcb      *client_dcb;      /*< The client connection */
    void            *router_session;  /*< The router instance data */
    SESSION_STATS   stats;            /*< Session statistics */
    struct service  *service;         /*< The service this session is using */
    int             n_filters;        /*< Number of filter sessions */
    SESSION_FILTER  *filters;         /*< The filters in use within this session */
    DOWNSTREAM      head;             /*< Head of the filter chain */
    UPSTREAM        tail;             /*< The tail of the filter chain */
    int             refcount;         /*< Reference count on the session */
    bool            ses_is_child;     /*< this is a child session */
    session_trx_state_t trx_state;    /*< The current transaction state. */
    bool            autocommit;       /*< Whether autocommit is on. */
    skygw_chk_t     ses_chk_tail;
} SESSION;

#define SESSION_INIT {.ses_chk_top = CHK_NUM_SESSION, .ses_lock = SPINLOCK_INIT, \
    .stats = SESSION_STATS_INIT, .head = DOWNSTREAM_INIT, .tail = UPSTREAM_INIT, \
    .state = SESSION_STATE_ALLOC, .ses_chk_tail = CHK_NUM_SESSION}

/** Whether to do session timeout checks */
extern bool check_timeouts;

/** When the next timeout check is done. This is compared to hkheartbeat in
 * hk_heartbeat.h */
extern long next_timeout_check;

#define SESSION_PROTOCOL(x, type)       DCB_PROTOCOL((x)->client_dcb, type)

/**
 * A convenience macro that can be used by the protocol modules to route
 * the incoming data to the first element in the pipeline of filters and
 * routers.
 */
#define SESSION_ROUTE_QUERY(sess, buf)                          \
    ((sess)->head.routeQuery)((sess)->head.instance,            \
                              (sess)->head.session, (buf))
/**
 * A convenience macro that can be used by the router modules to route
 * the replies to the first element in the pipeline of filters and
 * the protocol.
 */
#define SESSION_ROUTE_REPLY(sess, buf)                          \
    ((sess)->tail.clientReply)((sess)->tail.instance,           \
                               (sess)->tail.session, (buf))

SESSION *session_alloc(struct service *, struct dcb *);
bool session_pre_alloc(int number);
SESSION *session_set_dummy(struct dcb *);
bool session_free(SESSION *);
int session_isvalid(SESSION *);
int session_reply(void *inst, void *session, GWBUF *data);
char *session_get_remote(SESSION *);
char *session_getUser(SESSION *);
void printAllSessions();
void printSession(SESSION *);
void dprintSessionList(DCB *pdcb);
void dprintAllSessions(struct dcb *);
void dprintSession(struct dcb *, SESSION *);
void dListSessions(struct dcb *);
char *session_state(session_state_t);
bool session_link_dcb(SESSION *, struct dcb *);
SESSION* get_session_by_router_ses(void* rses);
void session_enable_log_priority(SESSION* ses, int priority);
void session_disable_log_priority(SESSION* ses, int priority);
RESULTSET *sessionGetList(SESSIONLISTFILTER);
void process_idle_sessions();
void enable_session_timeouts();

/**
 * Get the transaction state of the session.
 *
 * Note that this tells only the state of @e explicitly started transactions.
 * That is, if @e autocommit is OFF, which means that there is always an
 * active transaction that is ended with an explicit COMMIT or ROLLBACK,
 * at which point a new transaction is started, this function will still
 * return SESSION_TRX_INACTIVE, unless a transaction has explicitly been
 * started with START TRANSACTION.
 *
 * Likewise, if @e autocommit is ON, which means that every statement is
 * executed in a transaction of its own, this will return false, unless a
 * transaction has explicitly been started with START TRANSACTION.
 *
 * @note The return value is valid only if either a router or a filter
 *       has declared that it needs RCAP_TYPE_TRANSACTION_TRACKING.
 *
 * @param ses The SESSION object.
 * @return The transaction state.
 */
session_trx_state_t session_get_trx_state(const SESSION* ses);

/**
 * Set the transaction state of the session.
 *
 * NOTE: Only the protocol object may call this.
 *
 * @param ses       The SESSION object.
 * @param new_state The new transaction state.
 *
 * @return The previous transaction state.
 */
session_trx_state_t session_set_trx_state(SESSION* ses, session_trx_state_t new_state);

/**
 * Tells whether an explicit transaction is active.
 *
 * @see session_get_trx_state
 *
 * @note The return value is valid only if either a router or a filter
 *       has declared that it needs RCAP_TYPE_TRANSACTION_TRACKING.
 *
 * @return True if a transaction is active, false otherwise.
 */
static inline bool session_trx_is_active(const SESSION* ses)
{
    return ses->trx_state & SESSION_TRX_ACTIVE_BIT;
}

/**
 * Tells whether an explicit READ ONLY transaction is active.
 *
 * @see session_get_trx_state
 *
 * @note The return value is valid only if either a router or a filter
 *       has declared that it needs RCAP_TYPE_TRANSACTION_TRACKING.
 *
 * @return True if an explicit READ ONLY transaction is active,
 *         false otherwise.
 */
static inline bool session_trx_is_read_only(const SESSION* ses)
{
    return ses->trx_state == SESSION_TRX_READ_ONLY;
}

/**
 * Tells whether an explicit READ WRITE transaction is active.
 *
 * @see session_get_trx_state
 *
 * @note The return value is valid only if either a router or a filter
 *       has declared that it needs RCAP_TYPE_TRANSACTION_TRACKING.
 *
 * @return True if an explicit READ WRITE  transaction is active,
 *         false otherwise.
 */
static inline bool session_trx_is_read_write(const SESSION* ses)
{
    return ses->trx_state == SESSION_TRX_READ_WRITE;
}

/**
 * Tells whether autocommit is ON or not.
 *
 * Note that the returned value effectively only tells the last value
 * of the statement "set autocommit=...".
 *
 * That is, if the statement "set autocommit=1" has been executed, then
 * even if a transaction has been started, which implicitly will cause
 * autocommit to be set to 0 for the duration of the transaction, this
 * function will still return true.
 *
 * Note also that by default autocommit is ON.
 *
 * @see session_get_trx_state
 *
 * @return True if autocommit has been set ON, false otherwise.
 */
static inline bool session_is_autocommit(const SESSION* ses)
{
    return ses->autocommit;
}

/**
 * Sets the autocommit state of the session.
 *
 * NOTE: Only the protocol object may call this.
 *
 * @param enable True if autocommit is enabled, false otherwise.
 * @return The previous state.
 */
static inline bool session_set_autocommit(SESSION* ses, bool autocommit)
{
    bool prev_autocommit = ses->autocommit;
    ses->autocommit = autocommit;
    return prev_autocommit;
}

MXS_END_DECLS

#endif