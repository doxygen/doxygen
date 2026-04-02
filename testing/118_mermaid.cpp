// objective: test the \startmermaid and \endmermaid commands
// check: class_sender.xml
// check: class_receiver.xml
// check: 118__mermaid_8cpp.xml
// config: MERMAID_RENDER_MODE=CLIENT_SIDE

/** \file
 * Test inline mermaid diagram in file documentation.
 * \mermaid
 * flowchart LR
 *   A[Start] --> B[End]
 * \endmermaid
 */

/** Sender class. Can be used to send a command to the server.
 *  The receiver will acknowledge the command by calling Ack().
 *  \mermaid
 *    sequenceDiagram
 *    Sender->>Receiver: Command()
 *    Receiver-->>Sender: Ack()
 *  \endmermaid
 */
class Sender
{
  public:
    /** Acknowledgment from server */
    void Ack(bool ok);
};

/** Receiver class. Can be used to receive and execute commands.
 *  After execution of a command, the receiver will send an acknowledgment
 *  \mermaid
 *    sequenceDiagram
 *    Receiver->>Sender: Command()
 *    Sender-->>Receiver: Ack()
 *  \endmermaid
 */
class Receiver
{
  public:
    /** Executable a command on the server */
    void Command(int commandId);
};
