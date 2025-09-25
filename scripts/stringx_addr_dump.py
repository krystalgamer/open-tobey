import idautils
import ida_funcs
import idaapi

def main():
    
    for funcea in idautils.Functions(0x007D6010, 0x007DA050):
        name = idaapi.get_ea_name(funcea, idaapi.GN_SHORT|idaapi.GN_DEMANGLED)
        print(f'0x{funcea:08X} {name}')

if __name__ == '__main__':
    main()
