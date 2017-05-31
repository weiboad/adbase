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

    LOG_INFO << lua_gettop(engine->getLuaState());

	adbase::lua::BindingClass<sample> clazz("sample", "adinf.test", engine->getLuaState());
	//adbase::lua::BindingNamespace cs("sample_class_state_t", clazz.getOwnerNamespace());
    //LOG_INFO << lua_gettop(engine->getLuaState());
	//adbase::lua::BindingNamespace cs1("sample_class_state_t", clazz.getOwnerNamespace());
    LOG_INFO << lua_gettop(engine->getLuaState());
	adbase::lua::BindingClass<sample> clazz1("sample1", "adinf.test", engine->getLuaState());
    LOG_INFO << lua_gettop(engine->getLuaState());
	adbase::lua::BindingClass<sample> clazz2("sample2", "adinf.test", engine->getLuaState());
    LOG_INFO << lua_gettop(engine->getLuaState());
	adbase::lua::BindingClass<sample> clazz3("sample3", "adinf.test", engine->getLuaState());
    LOG_INFO << lua_gettop(engine->getLuaState());
	adbase::lua::BindingClass<sample> clazz4("sample4", "adinf.test", engine->getLuaState());
    LOG_INFO << lua_gettop(engine->getLuaState());

    //LOG_INFO << lua_gettop(engine->getLuaState());
	//adbase::lua::BindingClass<sample> clazz1("sample1", "adinf.test", engine->getLuaState());
    //LOG_INFO << lua_gettop(engine->getLuaState());
	//cs.addConst("CREATED", sample::CREATED);
    //LOG_INFO << lua_gettop(engine->getLuaState());
	//cs.addConst("INITED", sample::INITED);
	//std::string name = "adinf const";
	//clazz.addConst("INITED", name.c_str(), name.size());
	//clazz.addMethod("sum", &sum);
	//fnSum sumFn = std::bind(sum, std::placeholders::_1, std::placeholders::_2);
	//clazz.addMethod("sum1", sumFn);
	//clazz.addMethod("sum", &sample::sum);
	//clazz.addMethod("getSum", &sample::getSum);
	LOG_INFO << "BIND";

    return 0;
}

