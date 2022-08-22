# Design decision

 | Status       | <span style="color:green">Accepted</span> |
 |--------------|:---------:|
 | <b>Contributors</b> |     @Emirhan-Atabay @bowski23 @Ti-ger @vringar @TuCl   |
 | <b>Approved</b>     |     @Emirhan-Atabay @bowski23 @Ti-ger @vringar @TuCl   |
 | <b>Due date</b>     |    23.06.2022     |
 | <b>Decision on</b>     |   Server library  |

 |Problem statement|
 |--------------|
 |Eclipse Tahu provides a Sparkplug specific library while Eclipse Paho provides generic MQTT support.|

 |Solution hypothesis|
 |--------------|
 |Our choice decides on using a Sparkplug specific library or generic MQTT support with Paho.|

 |  | Option 1 | Option 2 |
 |--|--|--|
 |<b>Overview</b>|Tahu with Sparkplug support|Paho with generic MQTT support|
 |<b>Link</b>|[Tahu](https://github.com/eclipse/tahu)|[Paho](https://www.eclipse.org/paho)|
 |<b>Benefits and risks</b>|+ supports HiveMQ<br>+ Metrics and message format brings convention and consistency|+ supports HiveMQ<br>+ generic MQTT support|
 |<b>Criteria</b>|– is not generic<br>– too complicated and not flexible enough for our use case<br>– We have no advantage from the Standard Compliance<br>– We only use 4 of 9 message types<br>– We don't use Protobuf<br>– Message format is too advanced|+ generic MQTT support<br>+ more flexible<br>– missing metrics —> we could use our own message format instead|
