
#ifndef _TUTTLE_MESSAGE_H_
#define _TUTTLE_MESSAGE_H_

namespace tuttle {

    /*
    enum MessageType {
        EVALUATION_REQUEST,
        EVALUATION_REPLY,

        EXAMINATION_REQUEST,
        EXAMINATION_REPLY,

        BREAKPOINT_REQUEST,
        BREAKPOINT_REPLY,

        WATCHPOINT_REQUEST,
        WATCHPOINT_REPLY.

        TRACE_STDOUT,
        TRACE_STDERR,
        TRACE_WATCH,

        TRAP_THROW,
        TRAP_DEBUGGER,
        TRAP_BREAKPOINT,

        CONTINUE,        
    }
    */

    class Message {
    };

    class RoundtripMessage : Message {
    };

    class EvaluationRequest : RoundtripMessage {
    public:

        EvaluationRequest(char *theExpression);

        void setResult(jsval theResult);

    private:
        char  *_myExpression;
        jsval  _myResult;

    };

}

#endif /* !_TUTTLE_MESSAGE_H_ */
