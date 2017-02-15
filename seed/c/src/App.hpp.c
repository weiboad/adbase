#ifndef @ADINF_PROJECT_NAME|upper@_APP_HPP_
#define @ADINF_PROJECT_NAME|upper@_APP_HPP_

#include <adbase/Config.hpp>
#include "AdbaseConfig.hpp"

class App {
public:
	App(AdbaseConfig* config);
	~App();
	void reload();
	void run();
	void stop();
	//@IF @adserver
	void setAdServerContext(AdServerContext* context);
	//@ENDIF
	void setTimerContext(TimerContext* context);
	//@IF @kafkac || @kafkap
	void setAimsContext(AimsContext* context);
	//@ENDIF
	void loadConfig(adbase::IniConfig& config);
	uint64_t getSeqId();

private:
	AdbaseConfig* _configure;
	mutable std::mutex _mut;
};

#endif
