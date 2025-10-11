#ifndef MY_VALIDATIONS_H
#define MY_VALIDATIONS_H

void validate_stringx(void);
void validate_string_buf(void);

void validate_vm_stack(void);
void validate_vm_thread(void);
void validate_script_object_instance(void);
void validate_so_data_block(void);
void validate_vm_symbol(void);
void validate_os_file(void);
void validate_ini_parser(void);
void validate_os_developer_options(void);

void patch_vm_stack(void);
void patch_vm_thread(void);
void patch_alloc(void);
void patch_so_data_block(void);
void patch_vm_symbol(void);
void patch_os_file(void);
void patch_ini_parser(void);
#endif
