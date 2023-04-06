class TCoIpModule : QObject /Abstract/ {
%TypeHeaderCode
#include <coipmanager_threaded/TCoIpModule.h>
%End
public:
	virtual void start() = 0;
	virtual void pause() = 0;
	virtual void resume() = 0;
	virtual void stop() = 0;

signals:
	void moduleFinishedSignal();
};
