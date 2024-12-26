# Emscripten

## Build

```
autoreconf -i
emconfigure ./configure 'CXXFLAGS=-O3 -flto -fno-rtti -fno-exceptions'
emmake make
```

## Link

```
em++ -flto -O3 -fno-rtti -fno-exceptions *.o -o index.html -sUSE_SDL=2 -sUSE_SDL_MIXER=2 -sSDL2_MIXER_FORMATS='["wav","ogg"]' -sUSE_SDL_IMAGE=2 -sSDL2_IMAGE_FORMATS=png -sUSE_SDL_TTF=2 -sASYNCIFY -sASYNCIFY_IGNORE_INDIRECT -sASYNCIFY_ONLY=@funcs.txt -sENVIRONMENT=web --preload-file resources/ --closure 1 -sEXPORTED_RUNTIME_METHODS=['allocate']
```
