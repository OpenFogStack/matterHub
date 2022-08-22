# Design decision

| Status       | <span style="color:green">Accepted</span> |
|--------------|:---------:|
| <b>Contributors</b> |     @Emirhan-Atabay @bowski23 @Ti-ger @vringar @TuCl   |
| <b>Approved</b>     |     @Emirhan-Atabay @bowski23 @Ti-ger @vringar @TuCl   |
| <b>Due date</b>     |    05.05.2022     |
| <b>Decision on</b>     |   Customize an existing example program from connectedhomeip to our needs or develop a completely new program  |

|Problem statement|
|--------------|
|We have the choice between developing a completely new program that fits the matter specification or we could use the things which are already implemented by the Connectivity Standard Alliance. We could change the example programs in a way, that fits our needs.|

|Solution hypothesis|
|--------------|
|The MatterHub can interact with the end device once the program is implemented.|

|  | Option 1 | Option 2 |
|--|--|--|
|<b>Overview</b>|using an Example File from connectedhomeip and customizing it|self contributing|
|<b>Link</b>|[connectedhomeip on GitHub](https://github.com/project-chip/connectedhomeip)|-|
|<b>Benefits and risks</b>|+ We can use the existing code and only need to change it a bit<br>– too complicated<br>– too many files to look for<br>– no Documentation so we need to try to find out|– too many dependencies to consider<br>– no API or Documentation<br>– Why reinventing the wheel when connectedhomeip gives us the basic functionality|
|<b>Criteria</b>|+ we can use the existing code <br>– no Documentation|– Why reinvent the wheel? |