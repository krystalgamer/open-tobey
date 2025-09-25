import idautils
import ida_funcs
import idaapi

def main():
    
    for funcea in idautils.Functions(0x007D6010, 0x007DA050):
        name = idaapi.get_ea_name(funcea, idaapi.GN_SHORT|idaapi.GN_DEMANGLED)
        mangled = ida_funcs.get_func_name(funcea)
        print(f'PATCH_PUSH_RET(0x{funcea:08X}, {name})')
        print(f'PATCH_PUSH_RET_POLY(0x{funcea:08X}, {name}, "{mangled}")')

if __name__ == '__main__':
    main()
