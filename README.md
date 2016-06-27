##Multiple Play Simulation
###CSE 532 Lab3 "That ends this strange eventful history"

####Introduction
A client-server distributed application in which user assumes a role of "play producer" to collaborate with 
and monitor "NPC directors" on multiple computers within network to performs plays at the same time.

####Group Members
* Yu Xiao(xiao.y@wustl.edu)
* Yuhan Hao(haoyuhan@wustl.edu)
* Anqi Zhang(zhanganqi@wustl.edu)
	
####System Design Explanation

#####Director Program

- **Reactor Pattern** 
  - This pattern in ACE framework allow us integrate all the service handlers 
    that we need to send and receive messages between client and server without
    worrying too much about synchronization issue.
	
- **Interruptible Thread Pattern**	
	- Interruptible thread pattern is applied to handle the case when user want 
	  to stop the play or quit program by terminate threads' task and recycle 
	  threads elegantly.
	  
- **Message Queue Pattern**
  - This pattern stores and processes all the asynchronous coming messages without
    worrying too much about synchronization issue.

- **Finite State Machine** 
	- A generic finite state machine(fsm) to describe all the states director has and 
	  all the transitions between them. 
	- The fsm ensures that every state is clearly separated.
	
#####Producer Program
- **Command Processor Pattern**
    - This pattern used to handle the user input event which validates and parses
      the user input, generates commands that can excutes on the producer. In our
      application, the producer is also an command processor.

- **Reactor Pattern**
    - We diceded to use reactor pattern as the bridge between user interface and
      producer. Reactor acts as an active object which fires the timing event to 
      get command and excute on the producer.

- **Proactor Pattern**
    - This pattern is used to handling asynchronously communication event with
      remote directors.

- **Intercptor Pattern**
    - This pattern is semi-implemented in the application. Instead of building
      a literally *DataEndpoint*, we implemted a MessageHandler which handles the
      protocol serialization and interpreting as long as a new messag is received.

- **Model-View-Controller Pattern**
    - This pattern is in our application.
      * **View** -- *ViewRenderer* corresponds to the logic of presenting the view 
        on screen. It can request a View object to response a draw, scroll event.
        It also support a view switching function if multiple views inside this 
        container.
      * **Model** -- *TableViewCells* are the Model which store the data. They are 
         also used to performs *CRUD* operation on datastore (which resides in 
         *TableView* object). 
      * **Controller** -- Producer and inside service handler is the Controller 
         that can control the rendering behaviour, model modification behaviour.

#### Extended wrapper facade 

- **Message : public std::promise<FutureType>**
    - We designed our own message protocol. This protocol can be used in any kind of 
      message. For this lab, we provide *string* and *unsigned* type of message. 

- **Reactor Service Handler : ACE_Svc_Handler**
	  - Both send and receive in the socket service handler are needed, so we
      write an integrated service handler that can do both work.
	  - Override the handle_input method to handle receiving the message. 
	
#### Insights

This lab integrated the previous part and the ACE network part together and it is a 
really good practice of how to design the lab framework with all the design patterns 
that we have learned.


#### Run the project:

- unzip the file and open the project using vs2012

- Build it to generate debug files.

- Copy `aced.dll` into the debug file. 

- Navigate your terminal to debug file. Use the following command line:
	```
	ProducerProgram.exe
	```
	- Now you should be able to see a screen with user interface.
	  You can use `->` to see check the internal message log and `<-` to get back.
	
- Open another terminal, navigate to debug file. Use the following command line:
	```
	DirectorProgram.exe 1234 localhost <min_threads> <script_file>
	```
	- You can put several script files in the same time, that means you register 
    this director with several script files. Or you can just one script file to 
    different director(a new director needs a new terminal to run it).

#### References
[The link for lab description](http://www.cse.wustl.edu/~cdgill/courses/cse532/lab3/)
