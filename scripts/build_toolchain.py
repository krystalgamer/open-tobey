import sys
import os
from pathlib import Path

def gen_reg(path):
    return fr'''Windows Registry Editor Version 5.00

[HKEY_CURRENT_USER\Software\Microsoft\DevStudio\6.0\Build System\Components\Platforms\Win32 (x86)\Directories]
"Path Dirs"="{path}\\Common\\MSDev98\\Bin;{path}\\VC98\\BIN;{path}\\Common\\TOOLS;{path}\\Common\\TOOLS\\WINNT;C:\\WINDOWS\\system32;C:\\WINDOWS;C:\\WINDOWS\\System32\\Wbem"
"Include Dirs"="{path}\\VC98\\INCLUDE;{path}\\VC98\\MFC\\INCLUDE;{path}\\VC98\\ATL\\INCLUDE"
"Library Dirs"="{path}\\VC98\\LIB;{path}\\VC98\\MFC\\LIB"
"Source Dirs"="{path}\\VC98\\MFC\\SRC;{path}\\VC98\\MFC\\INCLUDE;{path}\\VC98\\ATL\\INCLUDE;{path}\\VC98\\CRT\\SRC"
'''

def main():
    p = Path(sys.argv[1]).absolute()
    print(gen_reg(f'{p}'.strip('"').replace('\\', '\\\\')))

    return

if __name__ == '__main__':
    main()
    sys.exit(0)
