#include "SDLSystem.h"
SDL_version SDLSystem::_compiled;
SDL_version SDLSystem::_linked;
SDLSystem::SDLSystem(Uint32 flags) {
	SDL_ResetAssertionReport();
	SDL_VERSION(&_compiled);
	SDL_GetVersion(&_linked);
	if (SDL_Init(flags) != 0) {
		throw SDLInitError();
	}	
	_basePath.reset(SDL_GetBasePath());
	_prefPath.reset(SDL_GetPrefPath("ulfben", "raycastdemo"));
	if (!_prefPath || !_basePath) {
		throw SDLInitError();
	}
	_flags = flags;	
}
SDLSystem::~SDLSystem() {
	printAssertionReport();
	SDL_Quit();	
}
void SDLSystem::initSubSystem(Uint32 flags) const {
	if (SDL_InitSubSystem(flags) != 0) {
		throw SDLInitError();
	}	
}
void SDLSystem::quitSubSystem(Uint32 flags) const noexcept {
	SDL_QuitSubSystem(flags);	
}
bool SDLSystem::isUp(Uint32 flags) const noexcept {
	if (!flags) { flags = _flags; }
	return SDL_WasInit(flags) != 0;
}
const SDL_version& SDLSystem::getCompiledVersion() const noexcept {
	return _compiled;	
}
const SDL_version& SDLSystem::getLinkedVersion() const noexcept {
	return _linked;
}
bool SDLSystem::setHint(const std::string_view name, const std::string_view value) const noexcept {
	return SDLex::toBool(SDL_SetHint(name.data(), value.data()));
}
std::string_view SDLSystem::getHint(std::string_view name) const noexcept {
	return SDL_GetHint(name.data());
}
bool SDLSystem::setHintWithPriority(const std::string_view name, const std::string_view value, SDL_HintPriority priority) const noexcept {
	return SDLex::toBool(SDL_SetHintWithPriority(name.data(), value.data(), priority) );
}
void SDLSystem::clearHints() const noexcept {
	SDL_ClearHints();
}
std::string_view SDLSystem::getBasePath() const noexcept {	
	return _basePath.get();
}
std::string_view SDLSystem::getPrefPath() const noexcept {
	return _prefPath.get();
}
void SDLSystem::printAssertionReport() const noexcept {
	const SDL_assert_data* item = SDL_GetAssertionReport();
	while (item) {
		printf("'%s', %s (%s:%d), triggered %u times, always ignore: %s.\n",
			item->condition, item->function, item->filename,
			item->linenum, item->trigger_count,
			item->always_ignore ? "yes" : "no");
		item = item->next;
	}
}