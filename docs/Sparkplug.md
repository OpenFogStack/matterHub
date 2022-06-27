# Sparkplug Mapping




# Flexible Design
spBv1.0/matterhub/command/MatterhubID/MatterNodeId

GroupId: Matterhub (maybe matterHub:Production, or MatterHub:<ModelName>) <br>
NodeId: Has to be unique. 

example:

spBv1.0/matterhub/DBIRTH/0  : New Matterhub has been provisioned <br>
sbBv1.0/matterhub/DBIRTH/0/333 : MatterHub discovered a new Device  <br>
spBv1.0/matterhub/DDATA/0/333 : A device changed the state <br>
spBv1.0/matterhub/DCMD/0/333 : Sending a Command to the device <br>
spBv1.0/matterhub/NDATA/0 : MatterHub sending Meta Information (logs, firmware, ...) <br>
spBv1.0/matterhub/NCMD/0 : Sending a Command to the MatterHub <br>
spBv1.0/matterhub/DDEATH/0/333 : Device has been deprovisioned <br>
spBv1.0/matterhub/NDEATH/0 : MatterHub has been deprovisioned <br>

## Payload
### DBIRTH (Inform about a new Device)
Publisher: MatterHub<br>
Subscriber: Server
```
{
	"timestamp": 1234,
	"metrics": [
	{
		"name": "endpointId/clusterId/AttributeId",
		"timestamp": 1234,
		"dataType": "Boolean",
		"value": true,
	},
	{
		"name": "endpointId/clusterId/AttributeId",
		"timestamp": 1234,
		"dataType": "String",
		"value": "someText",
	},
	{
		"name": "endpointId/clusterId/side",
		"timestamp": 1234,
		"dataType": "String",
		"value": "server",
	}
	],
	"seq": 0
}
```	

### DDATA (Updating Endpoint Data)
Publisher: MatterHub<br>
Subscriber: Server
```
{
	"timestamp": 1234,
	"metrics": [
	{
		"name": "endpointId/clusterId/AttributeId",
		"timestamp": 1234,
		"dataType": "Boolean",
		"value": true,
	}
	],
	"seq": 0
}
```	
### DCMD (Control Endpoints)
Publisher: Server<br>
Subscriber: MatterHub
```
{
	"timestamp": 1234,
	"metrics": [
	{
		"name": "endpointId/clusterId/AttributeId/Command/Argument",
		"timestamp": 1234,
		"dataType":  datatype,
		"value": value
	}
	],
	"seq": 0
}
```
	
"Command":
write (to set specific parameters) <br>
cmd (to send commands to the device) <br>
read (for example if we went out of sync and need to know the current state) <br>
subscribe (to un/subscribe specific endpoints) <br>

### Comment
This is currently my favorite design. Following the specification it makes more sense to use the MatterHub as Node: 
> Node is any V3.1.1 compliant MQTT Client application that manages an MQTT Session and provides the physical and/or logical gateway functions [...]. The EoN node is responsible for any local protocol interface to existing legacy devices

It moves the endpointId into the name field which makes sense, since endpoint and cluster are hierarchically related to each other, additinally it turns the name field unique for a device.
To be even more flexible the whole name could be prefixed with an "m:" to indicate that it is a matter-attribute path.

The dataType is strictly speaking not required, since the combination of clusterId and attributeId alrea


# Matter Centered Design
A different design is imaginable: <br>
spBv1.0/MatterhubID/command/NodeId/ClusterId

spBv1.0/0/DBIRTH/0  : New Device (Node) has been provisioned <br>
sbBv1.0/matterhub/DBIRTH/0/333 : New Endpoint was discovered <br>
spBv1.0/matterhub/DDATA/0/333 :  Endpoint specific data <br>
spBv1.0/matterhub/DCMD/0/333 : Sending a Command to an Endpoint <br>
spBv1.0/matterhub/DDEATH/0/333 : Endpoint is no longer available (?)  <br>
spBv1.0/matterhub/NDEATH/0 : Device is no longer available <br>

With this design a smaller and simpler payload is possible, however the NDATA and NCMD would never be used, since Matter uses endpoint 0 for nodelevel configuration. 
This also has no formal correct way to communicate with the matterHub for example to upload the current firmware level, logs, etc. 
However maybe it is much more useful to split the digital representation of the Matter network from the administrative tasks


## Payload

### DDATA (Updating Endpoint Data)
Publisher: MatterHub<br>
Subscriber: Server<br>
```
{
	"timestamp": 1234,
	"metrics": [
	{
		"name": "clusterId/AttributeId",
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
Publisher: Server<br>
Subscriber: MatterHub
```
{
	"timestamp": 1234,
	"metrics": [
	{
		"name": "clusterId/AttributeId/Command/Argument",
		"timestamp": 1234,
		"dataType":  datatype,
		"value": value
	}
	],
	"seq": 0
}
```
	
"Command":
write (to set specific parameters) <br>
cmd (to send commands to the device) <br>
read (for example if we went out of sync and need to know the current state) <br>
subscribe (to un/subscribe specific endpoints) <br>

```
Example 
{
	"timestamp": 1234,
	"metrics": [
	{
		"name": "6/0/cmd/toggle",
		"timestamp": 1234
		"dataType":  ""
		"value": ""
	}
	],
	"seq": 0
}
```

```
{
	"timestamp": 1234,
	"metrics": [
	{
		"name": "6/0/write/on-time",
		"timestamp": 1234
		"dataType":  "int16u"
		"value": "10"
	}
	],
	"seq": 0
}
```
