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

#include <maxscale/cppdefs.hh>

#include <deque>
#include <map>
#include <string>
#include <tr1/memory>
#include <cstdint>

#include <maxscale/jansson.hh>
#include <maxscale/utils.hh>

#include "http.hh"

using std::shared_ptr;
using std::string;
using std::map;
using std::deque;
using mxs::Closer;

class HttpRequest;

/** Typedef for managed pointer */
typedef std::shared_ptr<HttpRequest> SHttpRequest;

class HttpRequest
{
public:
    /**
     * @brief Parse a request
     *
     * @param request Request to parse
     *
     * @return Parsed statement or NULL if request is not valid
     */
    static HttpRequest* parse(string request);

    ~HttpRequest();

    /**
     * @brief Return request verb type
     *
     * @return One of the HTTP verb values
     */
    enum http_verb get_verb() const
    {
        return m_verb;
    }

    /**
     * @brief Check if a request contains the specified header
     *
     * @param header Header to check
     *
     * @return True if header is in the request
     */
    bool have_header(const string& header) const
    {
        return m_headers.find(header) != m_headers.end();
    }

    /**
     * @brief Get header value
     *
     * @param header Header to get
     *
     * @return Header value or empty string if the header was not found
     */
    string get_header(const string header) const
    {
        string rval;
        map<string, string>::const_iterator it = m_headers.find(header);

        if (it != m_headers.end())
        {
            rval = it->second;
        }

        return rval;
    }

    /**
     * @brief Get option value
     *
     * @param header Option to get
     *
     * @return Option value or empty string if the option was not found
     */
    string get_option(const string option) const
    {
        string rval;
        map<string, string>::const_iterator it = m_options.find(option);

        if (it != m_options.end())
        {
            rval = it->second;
        }

        return rval;
    }

    /**
     * @brief Return request body
     *
     * @return Request body or empty string if no body is defined
     */
    const string& get_json_str() const
    {
        return m_json_string;
    }

    /**
     * @brief Return raw JSON body
     *
     * @return Raw JSON body or NULL if no body is defined
     */
    const json_t* get_json() const
    {
        return m_json.get();
    }

    /**
     * @brief Get complete request URI
     *
     * @return The complete request URI
     */
    const string& get_uri() const
    {
        return m_resource;
    }

    /**
     * @brief Get URI part
     *
     * @param idx Zero indexed part number in URI
     *
     * @return The request URI part or empty string if no part was found
     */
    const string uri_part(uint32_t idx) const
    {
        return m_resource_parts.size() > idx ? m_resource_parts[idx] : "";
    }

    /**
     * @brief Return how many parts are in the URI
     *
     * @return Number of URI parts
     */
    size_t uri_part_count() const
    {
        return m_resource_parts.size();
    }

private:
    HttpRequest();
    HttpRequest(const HttpRequest&);
    HttpRequest& operator = (const HttpRequest&);

    map<string, string> m_headers;        /**< Request headers */
    map<string, string> m_options;        /**< Request options */
    Closer<json_t*>     m_json;           /**< Request body */
    string              m_json_string;    /**< String version of @c m_json */
    string              m_resource;       /**< Requested resource */
    deque<string>       m_resource_parts; /**< @c m_resource split into parts */
    enum http_verb      m_verb;           /**< Request method */
};