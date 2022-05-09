### Operational Credentials
- Each node has a Node Operational Certificate (NOC)
- NOC (X.509 Certificate) encodes a unique Node Operational Identifier (NOI)
- If one node is part of multiple ecosystems, it has multiple NOCs (But we have only one ecosystem (?))
- NOC and NOI are valid within the scope of the own ecosystem (Matter Fabric)
- Every Fabric has a Root CA Certificate used to validate NOIs

### Commissioning:
- A commissioner needs to commission each node for network participation 
- Commissioning describes the initial configuration of the device
- The commissioner provided the node with the NOC and the Trusted Root CA Certificate 
- Fabrics have 64-bit identifier

### Access Control
- Each node has an Access Control List (ACL)
- ACL specifies permissions for NOCs
