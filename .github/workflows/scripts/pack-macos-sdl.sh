#!/bin/sh

if [ $# -lt 1 ]; then
  >&2 echo "Invalid amount of arguments"
  exit 1;
fi

SRC_BUNDLE="$1"
DEST_BASE_FOLDER=$(dirname $SRC_BUNDLE)

rm -rf "$DEST_BASE_FOLDER/packed"
mkdir "$DEST_BASE_FOLDER/packed"

cp -rp $SRC_BUNDLE "$DEST_BASE_FOLDER/packed/"

SDL2_LIB=$(otool -L -X "$DEST_BASE_FOLDER/packed/fb_sdl.app/Contents/MacOS/fb_sdl" | awk '{$1=$1};1' | cut -d' ' -f1 | grep libSDL2)

if [[ -z "$SDL2_LIB" ]]; then
  >&2 echo "No reference to libSDL2 found in binary"
  exit 1;
fi

SDL2_LIB_NAME=$(basename $SDL2_LIB)
echo "💡 Found libSDL2 at $SDL2_LIB ($SDL2_LIB_NAME)"

mkdir -p "$DEST_BASE_FOLDER/packed/fb_sdl.app/Contents/Frameworks"
cp $SDL2_LIB "$DEST_BASE_FOLDER/packed/fb_sdl.app/Contents/Frameworks/"

echo "✍️ Patching binary"
install_name_tool -change $SDL2_LIB "@executable_path/../Frameworks/$SDL2_LIB_NAME" "$DEST_BASE_FOLDER/packed/fb_sdl.app/Contents/MacOS/fb_sdl"

echo "✔️ Done!"
echo "👉 The packed binary is available at $DEST_BASE_FOLDER/packed/fb_sdl.app"
