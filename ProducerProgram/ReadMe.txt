========================================================================
                         CSE 532 fALL 2015 Lab3
========================================================================
1. Group Members: 
    Yuhan Hao(haoyuhan@wustl.edu)
    Anqi Zhang(zhanganqi@wustl.edu)
    Yu Xiao(xiao.y@wustl.edu)

2. Bug Issue Report:

	Debugging environment: VS2012
	
	We found a bug regarding wait_for in our program.
	When we try to use wait_for in the implementation of interruptible 
    thread, it can never set the ready flag, it is always deferred somehow.  	
	We occasionally run into this bug: An invalid parameter was passed to 
    a service or function.
	This bug is reported online. 
    http://stackoverflow.com/questions/13866088/using-a-c11-condition-variable-in-vs2012
	This is the official respond by microsoft:
	https://connect.microsoft.com/VisualStudio/feedback/details/762560
	Crash Point:
	msvcr110d.dll!Concurrency::details::DeleteAsyncTimerAndUnloadLibrary(_TP_TIMER * timer) Line 688    C++

	Consequence: 
    This bug makes our director application in unstable state, though it rarely
    happens. Our application, in order to maintain the interruptible attributes, relies heavily
    on std::future::wait_for function. We have try our best to avoid use this broken
    function in many places but cannot get rid of all of them. The version later than VS2013 
    fixed this bug. 
	
2. An overview of how your programs are designed, including all the design 
discussions requested

2.1 Director Part:

(1)	Reactor Pattern 
	We decided to use Reactor Pattern in this lab as this pattern is a 
    well-designed platform that can    concurrently integrate all the 
    service handler that we need to send and receive messages between 
    client and server.
	
(2) Interruptible Thread Pattern	
	We need to use interruptible thread pattern because we need to handle 
    the case that user want to stop the play or just quit. We need a 
    mechanism to track all the working thread and recycle it so that it 
    can be handled. 


(3) Message Queue Pattern
	We need a queue to store all the message because the messages come 
    asynchronously and we need a place to store them and handle it later 
    synchronously.

	
(4) Finite State Machine 
	We decided to use a state machine to describe all the states we 
    have and all the transitions between them. The reason being is that 
    with this mechanism, we can ensure that every state is clearly separately
	

2.2 Producer Part:
(1) Command Processor Pattern
    This pattern used to handle the user input event which validates and parses
    the user input, generates commands that can excutes on the producer. In our
    application, producer is also an command processor.

(2) Reactor Pattern
    We diceded to use reactor pattern as the bridge between user interface and
    producer. reactor acts as an active object which fires the timing event to 
    get command and excute on the producer.

(3) Proactor Pattern
    This pattern is used to handling asynchronously communication event with
    remote directors.

(4) Intercptor Pattern
    This pattern is semi-implemented in the application. Instead of building
    a literally DataEndpoint, we implemted a MessageHandler which handles the
    protocol serialization and interpreting as long as a new messag is received.

(5) Model-View-Controller Pattern
    This pattern is in our application. 
      1) View -- ViewRenderer corresponds to the logic of presenting the view 
         on screen. It can request a View object to response a draw, scroll event.
         It also support a view switching function if multiple views inside this 
         container.
      2) Model -- TableViewCells are the Model which store the data. They are 
         also used to performs CRUD operation on database (which resides in 
         TableView object). 
      3) Controller -- Producer and inside service handler is the Controller 
         that can control the rendering behaviour, model modification behaviour.

3. The wrapper facade we used or extended 

(1) Message : public std::promise<FutureType>
	Interruptible flag 
	We designed our own message protocol. This protocol can be used in any kind of 
    message. For this lab, we provide string and unsigned type of message. 
    The way we did in lab2 is that we design message as a subclass of std::promise. 
    But in this lab, we find a small bug in vs2012. The future wait_for cannot set
    ready flag, it will always be deferred.
	So to get this to work, we used a done_flag instead to manually check if the 
    message is received or not. 

(2) Reactor Service Handler : ACE_Svc_Handler
	Since we need to do both send and receive in the socket service handler, so we
     write an integrated service handler that can do both work.
	Override the handle_input method to handle receiving the message. 
	
4. Insights, observations, and questions.

This lab integrated the previous part and the ACE network part together and it is a 
really good practice of how to design the lab framework with all the design patterns 
that we have learned.


5. Run the project:

(1) unzip the file and open the project using vs2012

(2) Build it to generate debug files.

(3) Copy aced.dll into the debug file. 

(4) Navigate your terminal to debug file. Use the following command line:
	ProducerProgram.exe
	Now you should be able to see a screen with user interface.
	You can use -> to see check the internal message log and <- to get back.
	
(5)	Open another terminal, navigate to debug file. Use the following command line:
	DirectorProgram.exe 1234 localhost <min_threads> <script_file>
	You can put several script files in the same time, that means you register 
    this director with several script files. Or you can just one script file to 
    different director(a new director needs a new terminal to run it).
	

6. Test Case:

(1) User can use start/stop/quit command 

7. Extra credit
The innovative part our team did is beaufying the UI interface, which follows the
software design pattern of MVC.
User are allowed to navigate left and right to see either director information and
debugging message coming from application.
User are allowed to scroll the screen up and down to without worring about the out
of bound information.


 	
	
	










