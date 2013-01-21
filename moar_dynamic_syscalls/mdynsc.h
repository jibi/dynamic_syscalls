extern u32 *sct_len_ptr;
extern u32 **sct_ptr;

extern u32 my_sys_call_table[1024];
extern u32 my_sys_call_table_len;

void set_wp(int val);
int find_sysenter_sct_text_ptr(void);
void patch_sysenter_sct_text(u32 *sct, u32 len);

