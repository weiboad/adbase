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
    adbase::lua::Engine::getInstance().init();
    lua_State* L = adbase::lua::Engine::getInstance().getLuaState();
	adbase::lua::BindingManager::getInstance().init(L);
    adbase::lua::Engine::getInstance().runFile("test.lua");
    return 0;
}

int sum(int a, int b) {
	return a + b;
}

typedef std::function<int(int, int)> fnSum;

ADINF_LUA_BIND_OBJECT(sample) {
	lua_State* L = adbase::lua::Engine::getInstance().getLuaState();
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
