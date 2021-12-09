<h1> SocketProgramming Project </h1>

This is a simple client-server based chat room, where clients take turn to broadcast a question. Other clients (sorted by their id number and one at a time) each have 60 seconds to 
answer the question. If a client doesn't answer in 60 seconds, we will skip his/her turn.
Finally, the original client will choose the best answer out of all the given answers.
We have four different majors (computer engineering, electrical engineering, civil engineering and mechanical engineering), so each client can choose the proper catagory
and be assigned the corresponding room. Each room will start operating when it reaches exactly three members. It will then be out of reach to other clients. Other potential
clients will be assigned different rooms if needed.
When the mentioned round is repeated three times (in each round, a different client will ask the starting question), the room will be closed and the chat history (along with the
chosen answers) will be written into a text file.

<h2> To start </h2>
First compile both files and run the server output file in with the following format:
./server.out PORT_NUMBER

where PORT_NUMBER can be a random integer like 8080.
Then you can add as many clients as you wish (each client needs a new terminal window) by the following commmand:
./client.out PORT_NUMBER\
For each client, you will be prompted to choose a subject you wish to ask a question about.
If the number of people in that subject reaches three, you will be redirected to a room, in which you will be able to ask your questions and answer others' questions!
You can also read the notifications sent to each client to be aware of your turn in queue. The program will automatically terminate once everyone has asked their questions.

