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

#include <http/CurlHttpRequest.h>
#include <http/HttpRequest.h>
#include <StringList.h>

#include <iostream>
using namespace std;

size_t curlHttpHeaderWrite( void *ptr, size_t size, size_t nmemb, void *stream);

POST_DATA CurlHttpRequest::pooh;
bool CurlHttpRequest::_verbose;
bool CurlHttpRequest::_proxyAuthenticationDetermine;
long CurlHttpRequest::_proxyAuthentication;

CurlHttpRequest::CurlHttpRequest(HttpRequest * httpRequest){
	_curl = NULL;
	_proxyAuthenticationDetermine = false;
	_verbose = true;
	_httpRequest = httpRequest;
}

void CurlHttpRequest::sendRequest(bool sslProtocol, const std::string & hostname, unsigned int hostPort,
	const std::string & path, const std::string & data, bool postMethod){

	if (!_proxyAuthenticationDetermine) {
		_proxyAuthenticationDetermine = true;
		_proxyAuthentication = getProxyAuthenticationType();
	}

	Request r = {sslProtocol, hostname, hostPort, path, data, postMethod};
	_requestList.push_back(r);

	if( !isAlive() && _requestList.size() <= 1){
		start();
	}
}

void CurlHttpRequest::run(){
	CURLcode res;
	long response;
	_curl = curl_easy_init();
	do{
		if (_curl) {
			setCurlParam();
			if(HttpRequest::useProxy()) {
				setProxyParam();
				if(HttpRequest::useProxyAuthentication()) {
					setProxyUserParam();
				}
			}
			setSslParam();
 			setUrl(_requestList.front());
			
			res = curl_easy_perform(_curl);
			if( res != CURLE_OK){
				cout << curl_easy_strerror(res) << endl;
				answerReceived(String::null, getReturnCode(res));
			}

			curl_easy_getinfo(_curl, CURLINFO_RESPONSE_CODE, &response);
			if(_verbose){
				if(!response){
					cerr << "$$ no server response code has been received" << endl;
				}
				else{
					cout << "$$ server response code : " << response << endl;
				}
			}
		}
		else{
			cerr << "$$ libCurl initialization failded" << endl;
			return;
		}
		_requestList.pop_front();
	}
	while(!_requestList.empty());
	curl_easy_cleanup(_curl);
}

void CurlHttpRequest::sendRequest(const std::string & url, const std::string & data, bool postMethod){
	_httpRequest->sendRequest(url, data, postMethod);
}

void CurlHttpRequest::answerReceived(const std::string & answer, Error error){
	_httpRequest->answerReceived(answer, error);
}

void CurlHttpRequest::setUrl(Request r){
	setUrl(r.sslProtocol, r.hostname, r.hostPort, r.path, r.data, r.postMethod);
}

void CurlHttpRequest::setUrl(bool sslProtocol, const std::string & hostname, unsigned int hostPort,
	const std::string & path, const std::string & data, bool postMethod){
	string url;

	if (sslProtocol) {
		url = HttpRequest::HTTPS_PROTOCOL;
	} else {
		url = HttpRequest::HTTP_PROTOCOL;
	}
	if(!hostname.empty()){
		url.append(hostname);
	}
	if(hostPort > 0){
		url.append(HttpRequest::HTTP_PORT_SEPARATOR);
		url.append(String::fromNumber(hostPort));
	}
	if(!path.empty()){
		url.append(path);
	}
	if (!postMethod){
		curl_easy_setopt(_curl, CURLOPT_POST , 0);
		url.append("?");
		url.append(data);
	} else {
		curl_easy_setopt(_curl, CURLOPT_POST , 1);
		if( !data.empty() ){
			setPostData(data);
		}
	}
	curl_easy_setopt(_curl, CURLOPT_URL, getstr(url));
	//cerr << "CurlHttpRequest: HTTP URL:" << url << "    "<<  data << endl;
}

void CurlHttpRequest::setCurlParam(){
	curl_easy_setopt(_curl, CURLOPT_VERBOSE, _verbose);
	curl_easy_setopt(_curl, CURLOPT_READFUNCTION, curlHTTPRead);
	curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, curlHTTPWrite);
	//curl_easy_setopt(_curl, CURLOPT_HEADERFUNCTION, curlHttpHeaderWrite);
	//curl_easy_setopt(_curl, CURLOPT_WRITEHEADER, this);
	curl_easy_setopt(_curl, CURLOPT_FILE, this);
	curl_easy_setopt(_curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(_curl, CURLOPT_TIMEOUT, 15);
}

void CurlHttpRequest::setSslParam(){
	curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYHOST, 0);
	curl_easy_setopt(_curl, CURLOPT_SSLVERSION, 3);
}

void CurlHttpRequest::setPostData(std::string _data){
	char * data = getstr(_data);
	if( data ){

		//struct POST_DATA pooh;
		pooh.readptr = data;
		pooh.sizeleft = strlen(data);

		curl_easy_setopt(_curl, CURLOPT_POSTFIELDSIZE, (long)pooh.sizeleft);
		curl_easy_setopt(_curl, CURLOPT_INFILE, &pooh);
	}
}

void CurlHttpRequest::setProxyUserParam(){
	if (useProxyAuthentication() && _proxyAuthenticationDetermine){
		if ((_proxyAuthentication | CURLAUTH_BASIC) == CURLAUTH_BASIC){
			if(_verbose){
				cerr << "$$ Set proxy authentication : BASIC" << endl;
			}
			curl_easy_setopt(_curl, CURLOPT_PROXYAUTH, CURLAUTH_BASIC);
		}
		else if ((_proxyAuthentication | CURLAUTH_DIGEST) == CURLAUTH_DIGEST){
			if(_verbose){
				cerr << "$$ Set proxy authentication : DIGEST" << endl;
			}
			curl_easy_setopt(_curl, CURLOPT_PROXYAUTH, CURLAUTH_DIGEST);
		}
		else if ((_proxyAuthentication | CURLAUTH_NTLM) == CURLAUTH_NTLM){
			if(_verbose){
				cerr << "$$ Set proxy authentication : NTLM" << endl;
			}
			curl_easy_setopt(_curl, CURLOPT_PROXYAUTH, CURLAUTH_NTLM);
		}
		string proxyUserIDPassword = HttpRequest::getProxyUsername() + ":" + HttpRequest::getProxyPassword();
		curl_easy_setopt(_curl, CURLOPT_PROXYUSERPWD, getstr(proxyUserIDPassword));
	}
}

void CurlHttpRequest::setProxyParam(){
	if (useProxy() && _proxyAuthenticationDetermine){
		string proxy = HttpRequest::getProxyHost();
		proxy += ":" + String::fromNumber(HttpRequest::getProxyPort());
		curl_easy_setopt(_curl, CURLOPT_PROXY, getstr(proxy) );
		curl_easy_setopt(_curl, CURLOPT_HTTPPROXYTUNNEL, 1);
	}
}

/**
 * Find the proxy authentication method (BASIC, DIGEST, NTLM)
 *
 * Thanks to Julien Bossart
 */
long CurlHttpRequest::getProxyAuthenticationType(){
	CURL *curl_tmp;
	long AuthMask;
	string testUrl = "www.wengo.fr";

	if( useProxy() ){
		string proxy = HttpRequest::getProxyHost();
		proxy += ":" + String::fromNumber(HttpRequest::getProxyPort());

		curl_tmp = curl_easy_init();
		curl_easy_setopt(curl_tmp, CURLOPT_URL, getstr(testUrl));
		curl_easy_setopt(curl_tmp, CURLOPT_VERBOSE, _verbose);
		curl_easy_setopt(curl_tmp, CURLOPT_NOBODY, 1);
		curl_easy_setopt(curl_tmp, CURLOPT_PROXY, getstr(proxy));
		curl_easy_perform(curl_tmp);
		curl_easy_getinfo(curl_tmp, CURLINFO_PROXYAUTH_AVAIL, &AuthMask);
		curl_easy_cleanup(curl_tmp);
		if(_verbose){
			if ((AuthMask | CURLAUTH_BASIC) == CURLAUTH_BASIC){
				cerr << "$$ Proxy authentication find : BASIC" << endl;
			}
			else if ((AuthMask | CURLAUTH_DIGEST) == CURLAUTH_DIGEST){
				cerr << "$$ Proxy authentication find : DIGEST" << endl;
			}
			else if ((AuthMask | CURLAUTH_NTLM) == CURLAUTH_NTLM){
				cerr << "$$ Proxy authentication find : NTLM" << endl;
			}
		}
	}
	return AuthMask;
}

bool CurlHttpRequest::useProxy(){
	if(!HttpRequest::getProxyHost().empty() && HttpRequest::getProxyPort() != 0){
		return true;
	}
	return false;
}

bool CurlHttpRequest::useProxyAuthentication(){
	if( !HttpRequest::getProxyUsername().empty() && !HttpRequest::getProxyPassword().empty()){
		return true;
	}
	return false;
}

HttpRequest::Error CurlHttpRequest::getReturnCode(int curlcode){
	switch(curlcode){
		case CURLE_OK : return HttpRequest::NoError; break;
		case CURLE_COULDNT_RESOLVE_HOST : return HttpRequest::HostNotFound; break;
		case CURLE_COULDNT_CONNECT : return HttpRequest::ConnectionRefused; break;
		case CURLE_COULDNT_RESOLVE_PROXY : return HttpRequest::ProxyConnectionError; break;
		case CURLE_LOGIN_DENIED : return HttpRequest::ProxyAuthenticationError; break;
		case CURLE_OPERATION_TIMEOUTED : return HttpRequest::TimeOut; break;
		default : return HttpRequest::UnknownError;
	}
}

size_t curlHTTPWrite(void * ptr, size_t size, size_t nmemb, void * stream){

	if (stream && ptr) {
		CurlHttpRequest * instance = (CurlHttpRequest *) stream;

		//instance->answerReceived(string(buf), instance->getErrorCode((HttpRequest*)stream));
		if (true) {//sizeof(size_t) * nmemb == size ){
			instance->answerReceived(string((const char *) ptr, nmemb), HttpRequest::NoError);
		} else {
			instance->answerReceived(string((const char *) ptr, nmemb), HttpRequest::WrongContentLength);
		}
		return nmemb;
	}
	else {
		return 0;
	}
}

size_t curlHTTPRead(void *ptr, size_t size, size_t nmemb, void *userp)
{
	struct POST_DATA *pooh = (struct POST_DATA *)userp;

	if(size*nmemb < 1) {
		return 0;
	}

	if(pooh->sizeleft) {
		/* copy one single byte */
		*(char *)ptr = pooh->readptr[0];

		/* advance pointer */
		pooh->readptr++;

		/* less data left */
		pooh->sizeleft--;

		/* we return 1 byte at a time! */
		return 1;
	}
	/* no more data left to deliver */
	return -1;
}

size_t curlHttpHeaderWrite( void *ptr, size_t size, size_t nmemb, void *stream){
	if (stream && ptr) {
		CurlHttpRequest * instance = (CurlHttpRequest *) stream;

		char * buf = (char *) malloc(nmemb * sizeof(char) + 1);
		strncpy(buf, (char *) ptr, nmemb + 1);

		if( nmemb == strlen(buf) ){
			return strlen(buf);
		}
		else{
			instance->answerReceived(string(buf), HttpRequest::InvalidResponseHeader);
			//tells libCurl an error occured
			return -1;
		}
	}
	else {
		return 0;
	}
}

void CurlHttpRequest::printRequestList(){
	RequestList::iterator it;

	for(it = _requestList.begin(); it != _requestList.end(); it++){
		printRequest(*it);
	}
}

void CurlHttpRequest::printRequest(Request r){
	cout << "ssl : " << r.sslProtocol << endl << "hostname : " << r.hostname << endl << "hostport : " << r.hostPort << endl;
	cout << "path : " << r.path  << endl << "data : " << r.data << endl  << "post : " << r.postMethod << endl;
}

char * getstr(std::string s){
	//char * tmp = (char *) malloc(sizeof(char) * s.size());
	//strcpy(tmp, s.data());
	return strdup(s.c_str());
}
