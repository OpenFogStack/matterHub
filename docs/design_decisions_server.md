# Design decision

| Status       | <span style="color:green">Accepted</span> |
|--------------|:---------:|
| <b>Contributors</b> |     @Emirhan-Atabay @bowski23 @Ti-ger @vringar @TuCl   |
| <b>Approved</b>     |     @Emirhan-Atabay @bowski23 @Ti-ger @vringar @TuCl   |
| <b>Due date</b>     |    17.06.2022     |
| <b>Decision on</b>     |   Server structure  |

|Problem statement|
|--------------|
|We need a structure for our server, so the ESP32 (our Hub) and M5stack (our end device) can interact through the server with each other. We could directly send it to IOTT from the end device. We could implement an MQTT Broker as an intermediate as well which gets the topics by publishing and subscribing, transform them into commands and then send it to IOTT. Transforming in HiveMQ is difficult to implement so we could use a JAVA Rest Client as an intermediate as well.|

|Solution hypothesis|
|--------------|
|The MatterHub can interact with the Enddevice once the program is implemented.|


|  | Option 1 | Option 2 | Option 3 |
|--|--|--|--|
|<b>Overview</b>|<b>HiveMQ</b> + IOTT|HiveMQ + <b>Rest Client</b> + IOTT|sending <b>directly to IOTT</b>|
|<b>Link</b>|[HiveMQ](https://www.hivemq.com)|-|[Bosch IoT Things](https://docs.bosch-iot-suite.com/things/)|
|<b>Benefits and risks</b>|+ HiveMQ is a good MQTT Broker that the industry is using<br>– transforming received messages into commands is not efficient with HiveMQ|+ HiveMQ is a good MQTT Broker that the industry is using<br>– The Server must be set up too —> takes much time|+ We don't have an intermediate server —> The more Server we need, the more it would cost<br>– The Clients (ESP32) must have good CPU and RAM|
|<b>Criteria</b>|+ works fine with Sparkplug<br>– transforming commands not efficient with MQTT|+ transforming MQTT messages into commands is more efficient than using HiveMQ for the transformation<br>+ works fine with Sparkplug<br>– takes much time to set up the server|– The ESP32 is not strong enough|
