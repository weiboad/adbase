#include "BindingManager.hpp"
#include <adbase/Logging.hpp>

namespace adbase {
namespace lua {

// {{{ BindingClassManagerBase::BindingClassManagerBase()

BindingClassManagerBase::BindingClassManagerBase() {
    BindingManager::instance()->_luaStatesObjs.push_back(this);
}

// }}}
// {{{ BindingClassManagerBase::~BindingClassManagerBase()

BindingClassManagerBase::~BindingClassManagerBase() {
}

// }}}
// {{{ BindingManager::BindingManager()

BindingManager::BindingManager() : _inited(false) {
}

// }}}
// {{{ BindingManager::~BindingManager()

BindingManager::~BindingManager() {
}

// }}}
// {{{ int BindingManager::init()

int BindingManager::init(lua_State *L) {
    if (nullptr == L) {
        return -1;
    }

    if (!_inited) {
        for (funcType &fn : _autoBindList) {
            LuaAutoBlock block(L);
            fn();
        }
    }

    for (auto &cmgr : _luaStatesObjs) {
        cmgr->addLuaState(L);
    }

    _inited = true;
    return 0;
}

// }}}
// {{{ int BindingManager::proc()

int BindingManager::proc(lua_State *L) {
    for (auto &mgr : _luaStatesObjs) {
        mgr->proc(L);
    }

    return 0;
}

// }}}
// {{{ void BindingManager::addBind()

void BindingManager::addBind(funcType fn) {
    _autoBindList.push_back(fn);
}

// }}}
// {{{ BindingWrapper::BindingWrapper()

BindingWrapper::BindingWrapper(BindingManager::funcType fn) {
    BindingManager::instance()->addBind(fn);
}

// }}}
// {{{ BindingWrapper::~BindingWrapper()

BindingWrapper::~BindingWrapper() {

}

// }}}

}
}
