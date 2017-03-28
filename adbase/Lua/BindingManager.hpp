#ifndef ADBASE_LUA_BINDINGMANAGER_HPP_
#define ADBASE_LUA_BINDINGMANAGER_HPP_

#include <adbase/Utility.hpp>
#include <list>
#include <map>
#include "BindingUtils.hpp"

namespace adbase {
namespace lua {

class BindingClassManagerBase {
    protected:
        BindingClassManagerBase();

    public:
        virtual ~BindingClassManagerBase() = 0;

        virtual int proc(lua_State *L) = 0;

        virtual void addLuaState(lua_State *L) = 0;
};

template<typename TC>
class BindingClassManagerInst : public BindingClassManagerBase,
                                public adbase::Singleton<BindingClassManagerInst<TC>> {
    public:
        virtual int proc(lua_State *L) override {
            if (nullptr == L) {
                _cacheMaps.clear();
                return 0;
            }

            intptr_t index = reinterpret_cast<intptr_t>(L);
            auto iter = _cacheMaps.find(index);
            if (_cacheMaps.end() == iter) {
                return -1;
            }
            iter->second.clear();
            return 0;
        }
        
        bool addRef(lua_State *L, const std::shared_ptr<TC>& ptr) {
            if (nullptr == L) {
                return false;
            }

            intptr_t index = reinterpret_cast<intptr_t>(L);
            auto iter = _cacheMaps.find(index);
            if (_cacheMaps.end() == iter) {
                return false;
            }

            iter->second.push_back(ptr);
            return true;
        }

        virtual void addLuaState(lua_State *L) override {
            intptr_t index = reinterpret_cast<intptr_t>(L);
            _cacheMaps[index];
        }

    private:
        std::map<intptr_t, std::list<std::shared_ptr<TC>>> _cacheMaps;
};

class BindingManager : public adbase::Singleton<BindingManager> {
    public:
        typedef std::function<void()> funcType;
    protected:
        BindingManager();
        ~BindingManager();

    public:
        int init(lua_State *L);
        int proc(lua_State *L = nullptr);
        void addBind(funcType fn);

        template <typename TC>
        bool addRef(lua_State *L, const std::shared_ptr<TC> &ptr) {
            return BindingClassManagerInst<TC>::instance()->addRef(L, ptr);
        }

        inline bool isInited() const { return _inited; }

    private:
        bool _inited;
        std::list<funcType> _autoBindList;
        std::list<BindingClassManagerBase *> _luaStatesObjs;
        friend class BindingClassManagerBase;
};

class BindingWrapper {
    public:
        BindingWrapper(BindingManager::funcType);
        ~BindingWrapper();
};

#define ADINF_LUA_BIND_VAR_NAME(name) adinf_lua_LuaBindMgr_Var_##name
#define ADINF_LUA_BIND_FN_NAME(name) adinf_lua_LuaBindMgr_Fn_##name

#define ADINF_LUA_BIND_OBJECT(name)                                                  \
    static void ADINF_LUA_BIND_FN_NAME(name)();                                       \
    static adbase::lua::BindingWrapper ADINF_LUA_BIND_VAR_NAME(name)(ADINF_LUA_BIND_FN_NAME(name)); \
    void ADINF_LUA_BIND_FN_NAME(name)()

}
}

#endif
