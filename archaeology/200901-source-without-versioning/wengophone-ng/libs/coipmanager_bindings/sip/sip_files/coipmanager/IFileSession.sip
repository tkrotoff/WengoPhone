class IFileSession /NoDefaultCtors/ {
%TypeHeaderCode
#include <coipmanager/filesessionmanager/IFileSession.h>
%End
public:

	enum IFileSessionEvent {
		/** No error. */
		IFileSessionEventNoError,

		/** Emitted when a ask for transferring a file has been sent. */
		IFileSessionEventInviteToTransfer,

		/**
		 * Emitted when the invite to transfer has been received by the peer but
		 * he has not yet accept the tranfer.
		 */
		IFileSessionEventWaitingForAnswer,

		/** Emitted when a file transfer is finished. */
		IFileSessionEventFileTransferFinished,

		/** Emitted when a file transfer failed. */
		IFileSessionEventFileTransferFailed,

		/** Emitted when a File transfer is paused by the user. */
		IFileSessionEventFileTransferPaused,

		/** Emitted when a File transfer is paused by peer. */
		IFileSessionEventFileTransferPausedByPeer,

		/** Emitted when a File transfer is resumed by the user. */
		IFileSessionEventFileTransferResumed,

		/** Emitted when a File transfer is resumed by peer. */
		IFileSessionEventFileTransferResumedByPeer,

		/** Emitted when a File transfer has been cancelled by the user. */
		IFileSessionEventFileTransferCancelled,

		/** Emitted when a File transfer has been cancelled by peer. */
		IFileSessionEventFileTransferCancelledByPeer,

		/** Emitted when the transfer of a File starts. */
		IFileSessionEventFileTransferBegan,

	};

};
