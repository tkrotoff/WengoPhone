/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2005  Wengo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef AUDIOCALLMANAGER_H
#define AUDIOCALLMANAGER_H

#include "AudioCall.h"

#include <observer/Observer.h>
#include <singleton/Singleton.h>

#include <qobject.h>

#include <map>

class Contact;
class SessionWindow;
class QString;

/**
 * AudioCallManager manages all AudioCall objects.
 *
 * AudioCallManager creates AudioCall
 * Destroy an AudioCall if it is in init state and more than one AudioCall object in init state exists
 *
 * Observes all the AudioCall
 *
 * Send CallProgress for the AudioCall which need it
 *
 * @author Philippe Kajmar
 * @author Mathieu Stute
 */
class AudioCallManager : public QObject, public Observer, public Singleton<AudioCallManager> {
	Q_OBJECT

	friend class Singleton<AudioCallManager>;

	friend class PhApiCallbacks;

public:

	~AudioCallManager();
	AudioCallManager(const AudioCallManager &);

	/**
	 * Callback, called each time a Subject has launched a notify().
	 *
	 * @param subject Subject that has sent the notify()
	 * @param event Event
	 */
	void update(const Subject & subject, const Event & event);

	/// create an audio call in init state if no audio call in init state exists and focus the session window
	/**
	 *  @brief create a audio call in init state if no audio call in init state exists
	 */
	AudioCall * createAudioCall(bool enableVideo);

	AudioCall * createAudioCall();

	AudioCall * createAudioCall(const SipAddress & sipAddress, Contact * contact = NULL);

	/// create an audio call in init state if no audio call in init state exists and place the call 
	/**
	 * @brief create an audio call in init state if no audio call in init state exists
	 * else focus the session window
	 */
	AudioCall * createAudioCall(const SipAddress & sipAddress, Contact * contact, bool enableVideo);

	/**
	 * Shows the SessionWindow page on the SMS tab.
	 *
	 * The user wants to send a SMS.
	 */
	void sendSms(const QString & phoneNumber, const QString & message = QString::null, Contact * contact = NULL);

	/**
	 * Shows the SessionWindow page on the Instant Messaging (Chat) tab.
	 *
	 * The user wants to start a chat conversation with a Contact.
	 */
	void startChat(Contact & contact, const QString & messageReceived = QString::null);

	/**
	 * Plays a DTMF
	 */
	void playDTMF(char dmtf) const;

	void playDTMFSoundFile(const QString & soundfile) const;

	/**
	 * Gets if a conversation is occuring.
	 *
	 * @return true if the AudioCall in used is in ConversationSipState
	 */
	bool isConversationOccuring() const;

	AudioCall * getActiveAudioCall() /*const*/;

	/**
	 * FIXME
	 * Ugly trick
	 *
	 * Deletes all the _contact from all the SessionWindow
	 * Used when we reload the ContactList since some of the SessionWindow._contact
	 * where not NULL even if ContactList::eraseAll() deletes all the Contact.
	 * Used inside the class Login.
	 * Now it does not crash anymore when we change from one profile to another.
	 *
	 * @see Login
	 */
	void reset();

	/**
	 * Defines the vector of AudioCall.
	 */
	typedef std::vector<AudioCall *> AudioCalls;

	/**
	 * Return the internal list of audio call
	 */
	AudioCalls getAudioCallList() {
		return _audioCallList;
	}
	
signals:

	/** 
	 * A call closed
	 */
	void endCall();

	/**
	 * Input Call.
	 *
	 * We are in the state InputCallSipState and the user
	 * has to accept or reject the input call.
	 */
	void inputCall();

	/**
	 * Output Call.
	 *
	 * We are in the state WaitingSipState.
	 */
	void outputCall();

private:

	AudioCallManager();

	AudioCallManager & operator=(const AudioCallManager &);

	AudioCall * getAudioCall(const QString & phoneNumber);
	AudioCall * getCallFromCallId(int callId);

	/**
	 * Callback from phapi in order to check the progress of the AudioCall.
	 *
	 * This method is typically a big switch. This method calls the method handle()
	 * from the current SipState.
	 *
	 * @param callId ID of the call
	 * @param info state info of the call
	 * @see SipState
	 */
	void callProgress(int callId, const CallStateInfo * info);

	/**
	 * Callback from phapi in order to receive chat message.
	 */
	void messageProgress(int messageId, const struct phMsgStateInfo * info);

	/*
	 * Callback from phapi for frame-ready
	 *
	 */

	void onFrameReady(int cid, struct phVideoFrameReceivedEvent *ev);

	/**
	 * List of AudioCall.
	 */
	AudioCalls _audioCallList;

	/**
	 * CallHistory.
	 *
	 * Records incoming, outgoing and missed calls.
	 */
	CallHistory * _callHistory;

	 /// audiocall in use
	int _activeAudioCall;
};

#endif	//AUDIOCALLMANAGER_H
