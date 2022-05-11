# Software Architecture

Here we describe and prioritize the architectural goals of our project. The major points are taken from [here](https://www.codeproject.com/Articles/20467/Software-Architecture-Review-Guidelines)
The aspects have been sorted by "Importance" and where ever it was suitable ideas and comments have been added.

Use of this Document? As a general reminder of our Software Architectural goals and a collection of ideas on how to implement them.
We may also include (at least the most important aspects) in the PR-Checklist.

### Performance, Efficiency and Scalability
Performance: The time required to respond to stimuli (events) or the number of events processed in some interval of time.
Efficiency: Deals with the use of the resources available for execution of software, and how this impacts response times, throughput and storage consumption.
Scalability: Number of user changes while maintaining other qualities. Support continuous growth to meet user demand and business complexity. It must be possible to extend the minimum hardware configuration needed for the application with additional hardware to support increased workloads.

##### Design/Architectural ideas:

+ Connection pooling:
  + towards the IoTT Cloud: Group updates
  + Use Multicast/Group Commands if a change concerns multiple devices

+ Reducing round-trips:  Reaction/Trigger-base connection to IoTT (Long Poll, Triggers, etc)
+ Lowering traffic throughput: minimal updates, minimal in-sync-check (compare only hashes)
+ Transaction Concurrency: + if possible ensure that multiple commands can happen at once

##### Unit of Measurement:
Amount of time it takes to reflect changes on the digital twin/the real world
(Maybe we want to define a threshold here?)

##### Importance: high
The prototype should show what is possible with this technology, so the reaction time should be as good as possible. (or show that this is not realistic).

##### Our level of influence: low
Since we will reuse a lot of the already provided code I am not sure if we can "increase" the performance.
As long as the performance does not suffer too much we are fine.

### Security
Measure of the system's ability to resist unauthorized attempts at usage and denial of service.

We could write a whole document about this (and maybe we should?)

##### Importance: high
Security should be considered already in the concept and then also of course in prototypes.

##### Our level of influence: low
However I think aslong as we use the concepts provided by matter (correct!) we should be fine.

### Extensibility & Modifiability
Extensibility: New feature implementation/replacement of components with improved versions and the removal of unwanted or unnecessary features or components.

Modifiability: Ability to make changes to a system quickly and cost-effectively.

##### Importance: high
Since we're building a prototype for a standard that hasn't even been released yet, we should be able to both quickly adapt to changes and expand the feature set.

### Interoperability
Interaction with other sub-subsystem, or a well defined access to externally-visible functionality and data structures or interaction with other run-time environments.

##### Importance: high
Matter offers different levels of abstractions that our implementation should use and implement.

### Usability, Ease of Deployment and Ease of Administration
Usability: How easy is it to use the program?

Ease of Deployment: How quickly can you deploy the system?

Ease of Administration: Refers to the infrastructure, tools, and staff of administrators and technicians needed to maintain the health of the application. E.g. Change physical location of service with minimal impact on the rest of the system.

##### Design/Architectural ideas:
+ Provide Scripts to setup our use-cases
+ Extend the Shell, so we can test our use-cases locally
+ Simple and tested workflows for standard use-cases: (e.g adding new devices)

##### Unit of measurement: 
How many steps does it take to set up a use-case

##### Importance: medium
We should provide the features mentioned above, however, our focus should be on reaching the project goals

### Maintainability, Debug-ability and Monitoring
Maintainability: Problem fixing, repairing a software system after errors occur.

Debug-ability: Preparing applications for easy and efficient debugging. 

Monitoring: Registration of abnormal behavior. Real-time monitoring.

##### Design/Architectural ideas:
Logging

##### Importance: medium
We should be able to quickly locate errors

### Development Productivity
Cost and time saving mechanism to aid the development of applications based on the software architecture. The developers should be able to learn the architecture concept and how to implement it easily. Extending the development team with new developers should not cost much effort in instruction, etc. A standardized way of working using templates and coding standards could help raise both the learning curve and quality.

##### Design/Architectural ideas:
Best practices Coding checklist & standards

##### Importance: medium
We are currently working on different checklists and are to defining standards.

### Testability
How easy is it to test code a unit, sub-systems, etc.

##### Design/Architectural ideas:
+ Interface based programming
+ Inversion of control/Dependency injection
+ Classes with well defined responsibilities

##### TODO: We should talk about if and how much testing we want

### Reusability
On SW arch level, the ability to reuse the SW architecture for another application.

##### Importance: low
This is a proof of concept, the code will probably never see production. We will however document our insights and concepts in the report.

### Portability
The ability of the system to run under different computing environments.

##### Importance: low
We have a very specificy target architecture. IMHO the amout of work required to support another one is far beyond the possible benefits. 

### Reliability & Availability
Reliability: The ability of the system to keep operating over time in the context of application and system errors and in situations of unexpected or incorrect usage (to perform in a predictable manner).
Availablility: The proportion of time the system is up and running.

##### Importance: low 
Of course the prototype should be able to cover the defined cases without any errors, but we don't expect it to run for multiple days/weeks or even years.
