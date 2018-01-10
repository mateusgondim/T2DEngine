#ifndef _ENGINE_HPP
#define _ENGINE_HPP

#include "Timer.hpp"

class Engine final {
public:
	Engine() = default;
	void init();
	//void shut_down();
	~Engine() {}
private:
	Timer m_timer;
};

#endif // !_ENGINE_HPP
