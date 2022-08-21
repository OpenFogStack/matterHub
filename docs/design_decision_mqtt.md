# Design decision

| Status       | <span style="color:green">Accepted</span> |
|--------------|:---------:|
| <b>Contributors</b> |     @Emirhan-Atabay @bowski23 @Ti-ger @vringar @TuCl   |
| <b>Approved</b>     |     @Emirhan-Atabay @bowski23 @Ti-ger @vringar @TuCl   |
| <b>Due date</b>     |    15.06.2022     |
| <b>Decision on</b>     |   MQTT vs self contributing  |

|Problem statement|
|--------------|
|We could use MQTT or contribute our own protocol to exchange data from the ESP32 and the server.|

|Solution hypothesis|
|--------------|
|The Server structure depends on the choice we take on the protocol.|

|  | Option 1 | Option 2 |
|--|--|--|
|<b>Overview</b>|MQTT|self contributing|
|<b>Link</b>|[MQTT](https://mqtt.org)|-|
|<b>Benefits and risks</b>|+ MQTT is the standard for IoT Messaging<br>+ is lightweigt and efficient<br>+ supports Bi-directional communication<br>+ can scale up to Millions of Things<br>+ easy to encrypt messages using TLS<br>+ Support for unreliable Networks<br>+ Reliable Message Delivery|+ customizable<br>– takes time to implement<br>– Why reinvent the wheel when there is already a standard messaging protocol for IoT?|
|<b>Criteria</b>|+ is lightweight<br>+ secure<br>+ highly scaleble<br>+ reliable|– Why reinvent the wheel?|