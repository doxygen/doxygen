// objective: test the \msc and \endmsc commands
// check: class_sender.xml
// check: class_receiver.xml

/** Sender class. Can be used to send a command to the server.
 *  The receiver will acknowledge the command by calling Ack().
 *  \msc
 *    Sender,Receiver;
 *    Sender->Receiver [label="Command()", URL="\ref Receiver::Command()"];
 *    Sender<-Receiver [label="Ack()", URL="\ref Ack()", ID="1"];
 *  \endmsc
 */
class Sender
{
  public:
    /** Acknowledgement from server */
    void Ack(bool ok);
};

/** Receiver class. Can be used to receive and execute commands.
 *  After execution of a command, the receiver will send an acknowledgement
 *  \msc
 *    Receiver,Sender;
 *    Receiver<-Sender [label="Command()", URL="\ref Command()"];
 *    Receiver->Sender [label="Ack()", URL="\ref Sender::Ack()", ID="1"];
 *  \endmsc
 */
class Receiver
{
  public:
    /** Executable a command on the server */
    void Command(int commandId);
};
