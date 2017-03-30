#include <adbase/Logging.hpp>
#include <adbase/Lua.hpp>

#include <functional>

class sample {
	private:
		int _sum;
	public:
		enum STATE_T {
			CREATED=0,
			INITED		
		};
		void sum(int a, int b) {
			_sum = a + b;	
		}
		int getSum() {
			return _sum;
		}
};

int main(void) {
    adbase::lua::Engine* engine = new  adbase::lua::Engine();
	engine->init();
    //lua_State* L = adbase::lua::EngineSingleton::getInstance().getLuaState();
	//adbase::lua::BindingManager::getInstance().init(L);
    engine->runFile("test.lua");
    //adbase::lua::EngineSingleton::getInstance().runFile("b.lua");
	//adbase::lua::EngineSingleton::getInstance().clearLoaded("test.lua");
	std::this_thread::sleep_for(std::chrono::seconds(5));
    //adbase::lua::EngineSingleton::getInstance().runFile("b.lua");
    //adbase::lua::EngineSingleton::getInstance().runFile("test.lua");
	//std::this_thread::sleep_for(std::chrono::seconds(5));
    //adbase::lua::EngineSingleton::getInstance().runFile("test.lua");
    //adbase::lua::EngineSingleton::getInstance().runFile("b.lua");
    return 0;
}

std::list<std::pair<std::string, std::string>> sum(std::list<std::string> a, int b) {
	std::list<std::pair<std::string, std::string>> ret;
	int i = 0;
	for (auto &t : a) {
		std::string offset = std::to_string(i) + "3333";
		std::string msg = std::to_string(i) + t;
		std::pair<std::string, std::string> item;
		item.first = offset;
		item.second = msg;
		ret.push_back(item);
		i++;
	}
	return ret;
}

typedef std::function<std::list<std::pair<std::string, std::string>>(std::list<std::string>, int)> fnSum;

ADINF_LUA_BIND_OBJECT(sample) {
	lua_State* L = adbase::lua::EngineSingleton::getInstance().getLuaState();
	adbase::lua::BindingClass<sample> clazz("sample", "adinf.test", L);
	
	clazz.setDefaultNew<>();
	adbase::lua::BindingNamespace cs("sample_class_state_t", clazz.getOwnerNamespace());
	cs.addConst("CREATED", sample::CREATED);
	cs.addConst("INITED", sample::INITED);
	std::string name = "adinf const";
	clazz.addConst("INITED", name.c_str(), name.size());
	clazz.addMethod("sum", &sum);
	fnSum sumFn = std::bind(sum, std::placeholders::_1, std::placeholders::_2);
	clazz.addMethod("sum1", sumFn);
	clazz.addMethod("sum", &sample::sum);
	clazz.addMethod("getSum", &sample::getSum);
	LOG_INFO << "BIND";
}
