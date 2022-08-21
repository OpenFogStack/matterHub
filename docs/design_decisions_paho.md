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
|Our choice decides on using a Sparkplug specific library with JAVA or a generic MQTT support with Paho using Python.|

|  | Option 1 | Option 2 |
|--|--|--|
|<b>Overview</b>|Tahu with Sparkplug support|Paho with generic MQTT support|
|<b>Link</b>|[Tahu](https://github.com/eclipse/tahu)|[Paho](https://www.eclipse.org/paho)|
|<b>Benefits and risks</b>|+ JAVA|+ Python<br>+ generic MQTT support|
|<b>Criteria</b>|+ JAVA is more convenient<br>+ is sufficient for our application area<br>– is not generic|+ generic MQTT support if Sparkplug is not good enough|