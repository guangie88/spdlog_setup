#!/usr/bin/env sh
if ! which docker >/dev/null; then
    echo "You need 'docker' command to run clang-format for this repository"
    exit 1
fi

docker run \
    -v $(pwd):/workdir -w /workdir \
    -u "$(id -u):$(id -g)" \
    --rm -t \
    guangie88/clang-cmake:7 \
    find include/ src/ -type f \( -iname *.h -o -iname *.cpp \) -print0 | \
        xargs -0 -I {} clang-format -i {}
