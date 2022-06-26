# Sparkplug Mapping


# Flexible Design
spBv1.0/matterhub/command/MatterhubID/NodeId

example:

spBv1.0/matterhub/DBIRTH/0  : New Matterhub has been provisioned
sbBv1.0/matterhub/DBIRTH/0/333 : MatterHub discovered a new Device
spBv1.0/matterhub/DDATA/0/333 : A device changed the state
spBv1.0/matterhub/DCMD/0/333 : Sending a Command to the device
spBv1.0/matterhub/NDATA/0/333 : MatterHub sending Meta Information (logs, firmware, ...)
spBv1.0/matterhub/DCMD/0/333 : Sending a Command to the device
spBv1.0/matterhub/DDEATH/0/333 : Device has been deprovisioned
spBv1.0/matterhub/NDEATH/0 : MatterHub has been deprovisioned

## Payload
### DDATA (Updating Endpoint Data)
    {
	    "timestamp": 1234,
	    "metrics": [
	    {
		    "name": "Cluster/clusterId/Attribute/AttributeId",
		    "timestamp": 1234
		    "dataType": "Boolean" 
		    "value": true
		    "properties": ["endpointId", "kind"],
		    "values": ["1", "server"]
		}
		],
		"seq": 0
}
		    

# Matter Centered Design
A different design is imaginable: 
spBv1.0/MatterhubID/command/NodeId/ClusterId

spBv1.0/0/DBIRTH/0  : New Device (Node) has been provisioned
sbBv1.0/matterhub/DBIRTH/0/333 : New Endpoint was discovered
spBv1.0/matterhub/DDATA/0/333 :  Endpoint specific data
spBv1.0/matterhub/DCMD/0/333 : Sending a Command to an Endpoint
spBv1.0/matterhub/DDEATH/0/333 : Endpoint is no longer available (?)
spBv1.0/matterhub/NDEATH/0 : Device is no longer available

With this design a smaller and simpler payload is possible, however the NDATA and NCMD would never be used, since Matter uses endpoint 0 for nodelevel configuration. 
This also has no formal correct way to communicate with the matterHub for example to upload the current firmware level, logs, etc. 
However maybe it is much more useful to split the digital representation of the Matter network from the administrative tasks


## Payload

### DDATA (Updating Endpoint Data)
Publisher: MatterHub
Subscriber: Server

       {
            "timestamp": 1234,
            "metrics": [
            {
        	    "name": "clusterId/AttributeId",
        	    "timestamp": 1234
        	      "dataType": "Boolean"
        	    "value": true 		
        	} 		
        	],
	        "seq": 0
       }

The "dataType" is not really necessary and maybe we can go even one step further and send, as Jonathan suggested, just the raw data we receive and let the server interpret it.
### DCMD (Control Endpoints)
Publisher: Server
Subscriber: MatterHub

        {
    	    "timestamp": 1234,
    	    "metrics": [
    	    {
    		    "name": "clusterId/AttributeId/Command/Argument",
    		    "timestamp": 1234
    		    "dataType":  datatype
    		    "value": value
    		}
    		],
    		"seq": 0
    }

	
"Command":
write (to set specific parameters)
cmd (to send commands to the device)
read (for example if we went out of sync and need to know the current state)
subscribe (to un/subscribe specific endpoints)


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

