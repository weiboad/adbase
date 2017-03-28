#include "Engine.hpp"

namespace adbase {
namespace lua {

// {{{ Engine::Engine()

Engine::Engine() {
}

// }}}
// {{{ Engine::~Engine()

Engine::~Engine() {
}

// }}}
// {{{ int Engine::addOnInit()

int Engine::addOnInit(std::function<void(lua_State *)> fn) {
    _onInited.push_back(fn);
    return 0;
}
// }}} 
// {{{ int Engine::init()

int Engine::init(lua_State *state) {
    if (nullptr != _state && _ownerState) {
        lua_close(_state);
        _ownerState = false;
    }

    if (nullptr == _state) {
        _state = luaL_newstate();
        luaL_openlibs(_state);
        _ownerState = true;
    } else {
        _state = state;
    }

    for(auto &fn : _onInited) {
        fn(_state); 
    }
    _onInited.clear();

    return 0;
}

// }}}
// {{{ void Engine::addSearchPath()

void Engine::addSearchPath(lua_State *L, const std::string &path, bool isFront) {
    lua_getglobal(L, "package");
    lua_getfield(L, -1, "path");
    const char *curPath = lua_tostring(L, -1);
    
    if (isFront) {
        lua_pushfstring(L, "%s/?.lua;%s/?.luac;%s", path.c_str(), path.c_str(), curPath);
    } else {
        lua_pushfstring(L, "%s;%s/?.lua;%s/?.luac", curPath, path.c_str(), path.c_str()); 
    }
    lua_setfield(L, -3, "path");
    lua_pop(L, 2);
}

// }}}
// {{{ void Engine::addSearchPath()

void Engine::addSearchPath(const std::string &path, bool isFront) {
    addSearchPath(_state, path, isFront);
}

// }}}
// {{{ bool Engine::runFile()

bool Engine::runFile(const char* file) {
    return runFile(_state, file);
}

// }}}
// {{{ bool Engine::runFile()

bool Engine::runFile(lua_State *L, const char* file) {
    return fn::execFile(L, file);
}

// }}}
// {{{ bool Engine::runCode()

bool Engine::runCode(const char* codes) {
    return runCode(_state, codes);
}

// }}}
// {{{ bool Engine::runCode()

bool Engine::runCode(lua_State *L, const char* codes) {
    return fn::execCode(L, codes);
}

// }}}
// {{{ lua_State* Engine::getLuaState()

lua_State* Engine::getLuaState() {
    return _state;
}

// }}}

}
}
