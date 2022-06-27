# Sparkplug Mapping


# Flexible Design
spBv1.0/matterhub/command/MatterhubID/NodeId

example:

spBv1.0/matterhub/DBIRTH/0  : New Matterhub has been provisioned <br>
sbBv1.0/matterhub/DBIRTH/0/333 : MatterHub discovered a new Device  <br>
spBv1.0/matterhub/DDATA/0/333 : A device changed the state <br>
spBv1.0/matterhub/DCMD/0/333 : Sending a Command to the device <br>
spBv1.0/matterhub/NDATA/0/333 : MatterHub sending Meta Information (logs, firmware, ...) <br>
spBv1.0/matterhub/DCMD/0/333 : Sending a Command to the device <br>
spBv1.0/matterhub/DDEATH/0/333 : Device has been deprovisioned <br>
spBv1.0/matterhub/NDEATH/0 : MatterHub has been deprovisioned <br>

## Payload
### DDATA (Updating Endpoint Data)
```
    {
	    "timestamp": 1234,
	    "metrics": [
	    {
		    "name": "Cluster/clusterId/Attribute/AttributeId",
		    "timestamp": 1234,
		    "dataType": "Boolean",
		    "value": true,
		    "properties": ["endpointId", "kind"],
		    "values": ["1", "server"]
		}
		],
		"seq": 0
}
```		    

# Simpler Flexible Design
spBv1.0/matterhub/command/MatterhubID/MatterNodeId:EndpointId

GroupId: Matterhub (maybe matterHub:Production, or MatterHub:<ModelName>) <br>
NodeId: Has to be unique. 
DeviceId: MatterNodeId:MatterEndpointId
example:

spBv1.0/matterhub/DBIRTH/0  : New Matterhub has been provisioned <br>
sbBv1.0/matterhub/DBIRTH/0/333:0 : MatterHub discovered a new Device  <br>
spBv1.0/matterhub/DDATA/0/333:1 : A device changed the state <br>
spBv1.0/matterhub/DCMD/0/333:1 : Sending a Command to the device <br>
spBv1.0/matterhub/NDATA/0 : MatterHub sending Meta Information (logs, firmware, ...) <br>
spBv1.0/matterhub/NCMD/0 : Sending a Command to the MatterHub <br>
spBv1.0/matterhub/DDEATH/0/333:0 : Device has been deprovisioned <br>
spBv1.0/matterhub/NDEATH/0 : MatterHub has been deprovisioned <br>

## Payload
### DDATA (Updating Endpoint Data)
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
