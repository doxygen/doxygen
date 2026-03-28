// objective: test the \msc and \endmsc commands
// check: class_sender.xml
// check: class_receiver.xml
// check: 037__msc_8cpp.xml

/** \file
 * A bit more complex msc diagram, with also parallel events.
 * \msc
 * Sender_1,Receiver_1,Sender1_1,
 * Sender,Receiver,Sender1,
 * Sender_2,Receiver_2,Sender1_2;
 * 
 * Sender_1->Receiver_1 [label="Command()", URL="\ref Receiver::Command()"],
 * Sender1_1<-Receiver_1 [label="Ack()", URL="\ref Sender::Ack()", ID="1"];
 * 
 * Sender->Receiver [label="Command()", URL="\ref Receiver::Command()"];
 * Sender1<-Receiver [label="Ack()", URL="\ref Sender::Ack()", ID="1"];
 * Sender_2->Receiver_2 [label="Command()", URL="\ref Receiver::Command()"],
 * Sender1_2<-Receiver_2 [label="Ack()", URL="\ref Sender::Ack()", ID="1"];
 * \endmsc
 */

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
    /** Acknowledgment from server */
    void Ack(bool ok);
};

/** Receiver class. Can be used to receive and execute commands.
 *  After execution of a command, the receiver will send an acknowledgment
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
