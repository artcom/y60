
#include <string>

#include "TCPSocket.h"

namespace inet {

class FrameSocket {
public:
	FrameSocket(TCPSocketPtr theSocket)
	 : _mySocket(theSocket), _mySeparator() {
		_mySeparator.push_back((char)0);
	}

	virtual ~FrameSocket() {
	}

	bool receiveFrame(std::string& theResult) {
		char myBuffer[8192];
		unsigned myReceived = _mySocket->receive(&myBuffer, sizeof(myBuffer));
		if(myReceived) {
			_myBuffer.insert(_myBuffer.end(), myBuffer, myBuffer + myReceived);
		}

		std::vector<char>::iterator mySeparator =
			std::search(_myBuffer.begin(), _myBuffer.end(),
					    _mySeparator.begin(), _mySeparator.end());

		if(mySeparator != _myBuffer.end()) {
			theResult = std::string(_myBuffer.begin(), mySeparator);
			_myBuffer.erase(_myBuffer.begin(), mySeparator + _mySeparator.size());
			return true;
		}

		return false;
	}

	void sendFrame(const std::string& theFrame) {
		_mySocket->send(theFrame.c_str(), theFrame.size());
		_mySocket->send(&_mySeparator[0], _mySeparator.size());
	}

private:
	TCPSocketPtr      _mySocket;
	std::vector<char> _myBuffer;
	std::vector<char> _mySeparator;
};

}
