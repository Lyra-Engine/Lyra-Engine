set shell := ["sh", "-cu"]
set windows-shell := ["powershell.exe", "-NoLogo", "-Command"]

list:
    @cmake --list-presets=all

configure preset:
    cmake --preset {{preset}}

build preset:
    cmake --build --preset {{preset}}

test preset:
    cmake --build --preset {{preset}} --target regression

run preset target:
    cmake --build --preset {{preset}} --target {{target}}

[confirm("This will delete all build tree. Proceed (y/n)?")]
clean:
    @rm -rf Scratch
