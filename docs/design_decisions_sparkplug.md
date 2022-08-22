# Design decision

| Status       | <span style="color:green">Accepted</span> |
|--------------|:---------:|
| <b>Contributors</b> |     @Emirhan-Atabay @bowski23 @Ti-ger @vringar @TuCl   |
| <b>Approved</b>     |     @Emirhan-Atabay @bowski23 @Ti-ger @vringar @TuCl   |
| <b>Due date</b>     |    20.06.2022     |
| <b>Decision on</b>     |    Which server specification is working better with MQTT?   |

|Problem statement|
|--------------|
|We need a mechanism to send and receive messages from the MQTT Server. Should we take an existing specification like Tahu or Sparkplug or should we implement it by ourselves.|

|Solution hypothesis|
|--------------|
|We can send and receive messages through the end device (ESP32) and we can provide the communication to the server.|

|  | Option 1 | Option 2 | Option 3 |
|--|--|--|--|
|<b>Overview</b>|Tahu (raw MQTT Client)|Sparkplug|self contributing|
|<b>Link</b>|[Tahu Documentation](https://projects.eclipse.org/projects/iot.tahu)|[Sparkplug Documentation](https://www.eclipse.org/tahu/spec/Sparkplug%20Topic%20Namespace%20and%20State%20ManagementV2.2-with%20appendix%20B%20format%20-%20Eclipse.pdf)|-|
|<b>Benefits and risks</b>|+ can be customized<br>– there are already good existing solutions like Sparkplug<br>– no Topic Namespace Elements —> We need to implement it by ourself or map the topics to the commands|+ fully auto discover tags [(source)](https://cirrus-link.com/mqtt-sparkplug-tahu/)<br>+ state aware [(source)](https://cirrus-link.com/mqtt-sparkplug-tahu/)<br>+ open standard [(source)](https://cirrus-link.com/mqtt-sparkplug-tahu/)<br>+ provides industry interoperability with open standard [(source)](https://cirrus-link.com/mqtt-sparkplug-tahu/)<br>+ has Topic Namespace Elements (—> no need for mapping => lower latency and costs)|+ independent from other programs<br>– takes too long to develop<br>– there are already existing solutions that work better with MQTT|
|<b>Criteria</b>|+ MQTT spec is only 80 pages<br>– not much time left to implement it by ourself<br>– Sparkplug is better and easier|+ simple (=> MQTT spec is 80 pages and Sparkplug adds another 60 pages) [(source)](https://cirrus-link.com/mqtt-sparkplug-tahu/)<br>+ Open Source (=> license-free to use —> better for Bosch) [(source)](https://cirrus-link.com/mqtt-sparkplug-tahu/)<br>+ Lightweight (=> minimizing the data footprint and leading to more efficient communication) [(source)](https://cirrus-link.com/mqtt-sparkplug-tahu/)<br>+ Flexible (=> subscribers don't need to know who provides the information they subscribed) [(source)](https://cirrus-link.com/mqtt-sparkplug-tahu/)<br>+ powerful tool which is easy to implement and doesn't takes much time to develop new features<br>+ works fine with HiveMQ|+ we can customize it more<br>– takes too long to develop|