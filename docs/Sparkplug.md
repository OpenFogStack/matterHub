# Sparkplug Mapping

## Flexible Design

spBv1.0/matterhub/\<command\>/\<matterhubID\>/\<nodeId\>

GroupId: Matterhub (maybe matterHub:Production, or MatterHub:\<ModelName\>)  
NodeId: Has to be unique.

Examples:
| Topic                          | Meaning                                                  |
| ------------------------------ | -------------------------------------------------------- |
| spBv1.0/matterhub/DBIRTH/0     | New Matterhub has been provisioned                       |
| sbBv1.0/matterhub/DBIRTH/0/333 | MatterHub discovered a new Device                        |
| spBv1.0/matterhub/DDATA/0/333  | A device changed the state                               |
| spBv1.0/matterhub/DCMD/0/333   | Sending a Command to the device                          |
| spBv1.0/matterhub/NDATA/0      | MatterHub sending Meta Information (logs, firmware, ...) |
| spBv1.0/matterhub/NCMD/0       | Sending a Command to the MatterHub                       |
| spBv1.0/matterhub/DDEATH/0/333 | Device has been deprovisioned                            |
| spBv1.0/matterhub/NDEATH/0     | MatterHub has been deprovisioned                         |

## Payload

### DBIRTH (Inform about a new Device)

#### Expanded obsolete form:

Publisher: MatterHub  
Subscriber: Server

```json
{
    "timestamp": 1234,
    "metrics": [
    {
        "name": "<endpointID>/<clusterID>/<type>/<attributeID>",
        "timestamp": 1234,
    },
    {
        "name": "<endpointID>/<clusterID>/<type>/<commandID>",
        "timestamp": 1234,
    },
    ],
    "seq": 0
}
```
type is:<br>
command<br>
attribute<br>

```json
{
    "timestamp": 1234,
    "metrics": [
    {
        "name": "1/6/attribute/0",
        "timestamp": 1234,
    },
    {
        "name": "1/6/command/2>",
        "timestamp": 1234,
    }
    ],
    "seq": 0
}
```

#### Compact form:
In this form we have a metric for each endpoint of the new node. the value of this metric 
This hacky compact form is not ideal and only somewhat sparkplug conform but enables to save a lot of memory on the esp32 and is simple enough to avoid a sparkplug deep dive. The former method could have json string sizes of several kilobytes for even the simplest device examples.

```json
{
  "timestamp": 1356129168,
  "metrics":[
  	{ "name": "<endpointID>",
  	  "timestamp": 1356129168,
  	  "dataType" : "String",
  	  "value": "[{\"id\":<clusterID>,\"attributes\":[<attributeIDs>],\"commands\":[<commandIDs>]}]"
    }
  ],
  "seq": 0
}
```

##### Example
```json
{
  "timestamp":1356129168,
  "metrics":[
  	{ "name": "1",
  	  "timestamp": 1356129168,
  	  "dataType" : "String",
  	  "value": "[{\"id\":6,\"attributes\":[0],\"commands\":[0,1,2]},{\"id\": ,\"attributes\":[0,1,2,3,4,5,6,15,16,17,18,19,20],\"commands\":[0,1,2,3,4,5,6,7]}]"
  	},
  	{ "name": "2",
  	  "timestamp": 1356129168,
  	  "dataType" : "String",
  	  "value": "[{\"id\":6,\"attributes\":[0],\"commands\":[0,1,2]}]"
  	}
  ],
  "seq": 0
}
```

### DDATA (Updating Endpoint Data)

Publisher: MatterHub  
Subscriber: Server

```json
{
    "timestamp": 1234,
    "metrics": [
    {
        "name": "<endpointID>/<clusterID>/<attributeID>",
        "timestamp": 1234,
        "dataType": "Boolean",
        "value": true,
    }
    ],
    "seq": 0
}
```

### DCMD (Control Endpoints)

Publisher: Server  
Subscriber: MatterHub

```json
{
    "timestamp": 1234,
    "metrics": [
    {
        "name": "<endpointID>/<clusterId>/<InteractionType>/<Argument>",
        "timestamp": 1234,
        "dataType":  datatype,
        "value": value
    }
    ],
    "seq": 0
}
```

"InteractionType":  
write (to set specific parameters)  
cmd (to send commands to the device)   
read (for example if we went out of sync and need to know the current state)  
subscribe (to un/subscribe specific endpoints)  

"Argument": Depends on the InteractionType  
write: AttributeID  
read: AttributeID  
subscribe: AttributeID  
cmd: CommandID  

Example: (Turn on a light)
```json
{
    "timestamp": 1234,
    "metrics": [
    {
        "name": "333/6/cmd/1",
        "timestamp": 1234,
        "dataType":  datatype,
        "value": value
    }
    ],
    "seq": 0
}
```

### Comment

This is currently my favorite design. Following the specification it makes more sense to use the MatterHub as Node: 
> Node is any V3.1.1 compliant MQTT Client application that manages an MQTT Session and provides the physical and/or logical gateway functions [...]. The EoN node is responsible for any local protocol interface to existing legacy devices

It moves the endpointID into the name field which makes sense, since endpoint and cluster are hierarchically related to each other, additionally it turns the name field unique for a device.
To be even more flexible the whole name could be prefixed with an "m:" to indicate that it is a matter-attribute path.

The dataType is strictly speaking not required, since the combination of clusterId and attributeId already dictate the data type of the value


## Matter Centered Design

A different design is imaginable:   
spBv1.0/\<matterhubID\>/command/\<nodeID\>/\<clusterID\>

| Topic                  | Meaning                                |
| ---------------------- | -------------------------------------- |
| spBv1.0/0/DBIRTH/0     | New Device (Node) has been provisioned |
| sbBv1.0/0/DBIRTH/0/333 | New Endpoint was discovered            |
| spBv1.0/0/DDATA/0/333  | Endpoint specific data                 |
| spBv1.0/0/DCMD/0/333   | Sending a Command to an Endpoint       |
| spBv1.0/0/DDEATH/0/333 | Endpoint is no longer available (?)    |
| spBv1.0/0/NDEATH/0     | Device is no longer available          |

With this design a smaller and simpler payload is possible, however the NDATA and NCMD would never be used, since Matter uses endpoint 0 for nodelevel configuration.
This also has no formal correct way to communicate with the matterHub for example to upload the current firmware level, logs, etc.
However maybe it is much more useful to split the digital representation of the Matter network from the administrative tasks

### Payload

#### DDATA (Updating Endpoint Data)

Publisher: MatterHub  
Subscriber: Server

```json
{
    "timestamp": 1234,
    "metrics": [
    {
        "name": "clusterId/<attributeID>",
        "timestamp": 1234,
        "dataType": "Boolean",
        "value": true,
    }
    ],
    "seq": 0
}
```

The "dataType" is not really necessary and maybe we can go even one step further and send, as Jonathan suggested, just the raw data we receive and let the server interpret it.

### DCMD (Control Endpoints)

Publisher: Server  
Subscriber: MatterHub

```json
{
    "timestamp": 1234,
    "metrics": [
    {
        "name": "<clusterID>/<attributeID>/<InteractionType>/<Argument>",
        "timestamp": 1234,
        "dataType":  datatype,
        "value": value
    }
    ],
    "seq": 0
}
```

"InteractionType":
write (to set specific parameters)   
cmd (to send commands to the device)   
read (for example if we went out of sync and need to know the current state)   
subscribe (to un/subscribe specific endpoints)   

Example

```json
{
    "timestamp": 1234,
    "metrics": [
    {
        "name": "6/0/cmd/toggle",
        "timestamp": 1234,
        "dataType":  "",
        "value": ""
    }
    ],
    "seq": 0
}
```

```json
{
    "timestamp": 1234,
    "metrics": [
    {
        "name": "6/0/write/on-time",
        "timestamp": 1234,
        "dataType":  "int16u",
        "value": "10"
    }
    ],
    "seq": 0
}
```


