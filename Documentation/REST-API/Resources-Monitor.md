# Monitor Resource

A monitor resource represents a monitor inside MaxScale that monitors one or
more servers.

## Resource Operations

### Get a monitor

Get a single monitor. The _:name_ in the URI must be a valid monitor name with
all whitespace replaced with hyphens. The monitor names are case-sensitive.

```
GET /monitors/:name
```

#### Response

`Status: 200 OK`

```javascript
{
    "links": {
        "self": "http://localhost:8989/v1/monitors/MySQL-Monitor"
    },
    "data": {
        "id": "MySQL-Monitor",
        "type": "monitors",
        "relationships": {
            "servers": {
                "links": {
                    "self": "http://localhost:8989/v1/servers/"
                },
                "data": [
                    {
                        "id": "server1",
                        "type": "servers"
                    },
                    {
                        "id": "server2",
                        "type": "servers"
                    }
                ]
            }
        },
        "attributes": {
            "module": "mysqlmon",
            "state": "Running",
            "parameters": {
                "user": "maxuser",
                "password": "maxpwd",
                "monitor_interval": 10000,
                "backend_connect_timeout": 3,
                "backend_read_timeout": 1,
                "backend_write_timeout": 2,
                "backend_connect_attempts": 1,
                "detect_replication_lag": false,
                "detect_stale_master": true,
                "detect_stale_slave": true,
                "mysql51_replication": false,
                "multimaster": false,
                "detect_standalone_master": false,
                "failcount": 5,
                "allow_cluster_recovery": true,
                "journal_max_age": 28800
            },
            "monitor_diagnostics": {
                "monitor_id": 0,
                "detect_stale_master": true,
                "detect_stale_slave": true,
                "detect_replication_lag": false,
                "multimaster": false,
                "detect_standalone_master": false,
                "failcount": 5,
                "allow_cluster_recovery": true,
                "mysql51_replication": false,
                "journal_max_age": 28800,
                "server_info": [
                    {
                        "name": "server1",
                        "server_id": 0,
                        "master_id": 0,
                        "read_only": false,
                        "slave_configured": false,
                        "slave_io_running": false,
                        "slave_sql_running": false,
                        "master_binlog_file": "",
                        "master_binlog_position": 0
                    },
                    {
                        "name": "server2",
                        "server_id": 0,
                        "master_id": 0,
                        "read_only": false,
                        "slave_configured": false,
                        "slave_io_running": false,
                        "slave_sql_running": false,
                        "master_binlog_file": "",
                        "master_binlog_position": 0
                    }
                ]
            }
        },
        "links": {
            "self": "http://localhost:8989/v1/monitors/MySQL-Monitor"
        }
    }
}
```

#### Supported Request Parameter

- `pretty`

### Get all monitors

Get all monitors.

```
GET /monitors
```

#### Response

`Status: 200 OK`

```javascript
{
    "links": {
        "self": "http://localhost:8989/v1/monitors/"
    },
    "data": [
        {
            "id": "MySQL-Monitor",
            "type": "monitors",
            "relationships": {
                "servers": {
                    "links": {
                        "self": "http://localhost:8989/v1/servers/"
                    },
                    "data": [
                        {
                            "id": "server1",
                            "type": "servers"
                        },
                        {
                            "id": "server2",
                            "type": "servers"
                        }
                    ]
                }
            },
            "attributes": {
                "module": "mysqlmon",
                "state": "Running",
                "parameters": {
                    "user": "maxuser",
                    "password": "maxpwd",
                    "monitor_interval": 10000,
                    "backend_connect_timeout": 3,
                    "backend_read_timeout": 1,
                    "backend_write_timeout": 2,
                    "backend_connect_attempts": 1,
                    "detect_replication_lag": false,
                    "detect_stale_master": true,
                    "detect_stale_slave": true,
                    "mysql51_replication": false,
                    "multimaster": false,
                    "detect_standalone_master": false,
                    "failcount": 5,
                    "allow_cluster_recovery": true,
                    "journal_max_age": 28800
                },
                "monitor_diagnostics": {
                    "monitor_id": 0,
                    "detect_stale_master": true,
                    "detect_stale_slave": true,
                    "detect_replication_lag": false,
                    "multimaster": false,
                    "detect_standalone_master": false,
                    "failcount": 5,
                    "allow_cluster_recovery": true,
                    "mysql51_replication": false,
                    "journal_max_age": 28800,
                    "server_info": [
                        {
                            "name": "server1",
                            "server_id": 0,
                            "master_id": 0,
                            "read_only": false,
                            "slave_configured": false,
                            "slave_io_running": false,
                            "slave_sql_running": false,
                            "master_binlog_file": "",
                            "master_binlog_position": 0
                        },
                        {
                            "name": "server2",
                            "server_id": 0,
                            "master_id": 0,
                            "read_only": false,
                            "slave_configured": false,
                            "slave_io_running": false,
                            "slave_sql_running": false,
                            "master_binlog_file": "",
                            "master_binlog_position": 0
                        }
                    ]
                }
            },
            "links": {
                "self": "http://localhost:8989/v1/monitors/MySQL-Monitor"
            }
        }
    ]
}
```

#### Supported Request Parameter

- `pretty`

### Stop a monitor

Stops a started monitor.

```
PUT /monitor/:name/stop
```

#### Response

Monitor is stopped.

`Status: 204 No Content`

### Start a monitor

Starts a stopped monitor.

```
PUT /monitor/:name/start
```

#### Response

Monitor is started.

`Status: 204 No Content`

### Update a monitor

The :name in the URI must map to a monitor name with all whitespace replaced with
hyphens. The request body must be a valid JSON document representing the modified monitor.

```
PUT /monitor/:name
```

### Modifiable Fields

The following standard server parameter can be modified.
- [user](../Monitors/Monitor-Common.md#user)
- [password](../Monitors/Monitor-Common.md#password)
- [monitor_interval](../Monitors/Monitor-Common.md#monitor_interval)
- [backend_connect_timeout](../Monitors/Monitor-Common.md#backend_connect_timeout)
- [backend_write_timeout](../Monitors/Monitor-Common.md#backend_write_timeout)
- [backend_read_timeout](../Monitors/Monitor-Common.md#backend_read_timeout)
- [backend_connect_attempts](../Monitors/Monitor-Common.md#backend_connect_attempts)

Refer to the documentation on these parameters for valid values.

In addition to these standard parameters, the monitor specific parameters can also be
modified. Refer to the monitor module documentation for details on these parameters.

#### Response

Monitor is modified.

`Status: 204 No Content`

Invalid request body.

`Status: 403 Forbidden`

