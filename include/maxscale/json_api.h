#pragma once
/*
 * Copyright (c) 2016 MariaDB Corporation Ab
 *
 * Use of this software is governed by the Business Source License included
 * in the LICENSE.TXT file and at www.mariadb.com/bsl11.
 *
 * Change Date: 2019-07-01
 *
 * On the date above, in accordance with the Business Source License, use
 * of this software will be governed by version 2 or later of the General
 * Public License.
 */

/**
 * @file Helper functions for creating JSON API conforming objects
 */

#include <maxscale/cppdefs.hh>
#include <maxscale/jansson.hh>

MXS_BEGIN_DECLS

/** Resource endpoints */
#define MXS_JSON_API_SERVERS   "/servers/"
#define MXS_JSON_API_SERVICES  "/services/"
#define MXS_JSON_API_FILTERS   "/filters/"
#define MXS_JSON_API_MONITORS  "/monitors/"
#define MXS_JSON_API_SESSIONS  "/sessions/"
#define MXS_JSON_API_MAXSCALE  "/maxscale/"
#define MXS_JSON_API_THREADS   "/maxscale/threads/"
#define MXS_JSON_API_LOGS      "/maxscale/logs/"
#define MXS_JSON_API_TASKS     "/maxscale/tasks/"
#define MXS_JSON_API_MODULES   "/maxscale/modules/"

/**
 * @brief Create a JSON object
 *
 * The caller should add a `data` field to the returned object.
 *
 * @param host Hostname of this server
 * @param self Endpoint of this resource
 * @param data The JSON data, either an array or an object
 *
 * @return A valid top-level JSON API object
 */
json_t* mxs_json_resource(const char* host, const char* self, json_t* data);

/**
 * @brief Create an empty relationship object
 *
 * @param host Hostname of this server
 * @param endpoint The endpoint for the resource's collection
 *
 * @return New relationship object
 */
json_t* mxs_json_relationship(const char* host, const char* endpoint);

/**
 * @brief Add an item to a relationship object
 *
 * @param rel  Relationship created with mxs_json_relationship
 * @param id   The resource identifier
 * @param type The resource type
 */
void mxs_json_add_relation(json_t* rel, const char* id, const char* type);

/**
 * @brief Create self link object
 *
 * The self link points to the object itself.
 *
 * @param host Hostname of this server
 * @param path Base path to the resource collection
 * @param id   The identified of this resource
 *
 * @return New self link object
 */
json_t* mxs_json_self_link(const char* host, const char* path, const char* id);

/**
 * @brief Return value at provided JSON Pointer
 *
 * @param json     JSON object
 * @param json_ptr JSON Pointer to object
 * @return Pointed value or NULL if no value is found
 */
json_t* mxs_json_pointer(json_t* json, const char* json_ptr);

MXS_END_DECLS