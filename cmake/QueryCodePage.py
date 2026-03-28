import platform

def is_windows():
    return platform.system().lower() == "windows"

if is_windows():
    from winreg import OpenKey, QueryValueEx, HKEY_LOCAL_MACHINE, KEY_READ

if __name__ == '__main__':
    if is_windows():
        root = HKEY_LOCAL_MACHINE
        subkey = R'SYSTEM\CurrentControlSet\Control\Nls\CodePage'

        key = OpenKey(root, subkey, 0, KEY_READ)
        name = 'ACP'

        try:
            codepage, _ = QueryValueEx(key, name)
            print(codepage)
        except WindowsError:
            print('Failed to get code page')
