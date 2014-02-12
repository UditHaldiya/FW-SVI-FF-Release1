#ifndef MNIPCFF_H_
#define MNIPCFF_H_
extern void mnipc_ReadVars(T_FBIF_PTB *p_PTB, const u8 *receive_buff);
extern void mnipc_WriteDOswitches(const T_FBIF_BLOCK_INSTANCE *p_block_instance, u8 *send_buff);
extern void mnipc_WriteVars(T_FBIF_PTB *p_PTB, u8 *send_buff, u8 *send_length);

//borrowed utils - find a place for them
extern void PTB_ConvertWorkPos_InTravelUnits(const T_FBIF_PTB *p_PTB);
#endif //MNIPCFF_H_
