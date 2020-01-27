#pragma once
#include "..\include\SDL.h"
#include "SDLex.h"
class SDLSystem {
	Uint32 _flags;	
	SDLex::CharPtr _basePath; //the application data directory
	SDLex::CharPtr _prefPath; //the only safe place to write files 	
	static SDL_version _compiled;
	static SDL_version _linked;
public:
	SDLSystem(Uint32 flags = SDL_INIT_VIDEO|SDL_INIT_EVENTS);
	~SDLSystem();
	void printAssertionReport() const noexcept;
	void initSubSystem(Uint32 flags) const;
	void quitSubSystem(Uint32 flags) const noexcept;
	bool isUp(Uint32 flags = 0) const noexcept;
	const SDL_version& getCompiledVersion() const noexcept;
	const SDL_version& getLinkedVersion() const noexcept;
	bool setHint(const std::string_view name, const std::string_view value) const noexcept; //https://wiki.libsdl.org/CategoryHints#Hints
	bool setHintWithPriority(const std::string_view name, const std::string_view value, SDL_HintPriority priority) const noexcept;
	std::string_view getHint(const std::string_view name) const noexcept;
	void clearHints() const noexcept;
	std::string_view getBasePath() const noexcept;
	std::string_view getPrefPath() const noexcept;
};